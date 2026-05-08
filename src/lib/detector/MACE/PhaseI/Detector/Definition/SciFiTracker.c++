// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

#include "MACE/PhaseI/Detector/Definition/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"

#include "Mustard/Geant4X/Geometry/HelicalBox.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Torus.hh"
#include "G4Tubs.hh"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <utility>

namespace MACE::PhaseI::Detector::Definition {

using namespace Mustard::PhysicalConstant;
using namespace Mustard::MathConstant;
using namespace Mustard::LiteralUnit;

auto SciFiTracker::Construct(G4bool checkOverlaps) -> void {
    const auto& sciFiTracker{Description::SciFiTracker::Instance()};
    const auto scifiName{sciFiTracker.Name()};
    const auto nistManager{G4NistManager::Instance()};

    const auto silicon{nistManager->FindOrBuildMaterial("G4_Si")};
    const auto carbonElement{nistManager->FindOrBuildElement("C")};
    const auto hydrogenElement{nistManager->FindOrBuildElement("H")};
    const auto oxygenElement{nistManager->FindOrBuildElement("O")};
    const auto siliconElement{nistManager->FindOrBuildElement("Si")};
    const auto fluorineElement{nistManager->FindOrBuildElement("F")};
    const auto chlorineElement{nistManager->FindOrBuildElement("Cl")};

    const auto siliconeOil{new G4Material("silicone_oil", 0.97_g_cm3, 4, kStateLiquid)};
    siliconeOil->AddElement(carbonElement, 2);
    siliconeOil->AddElement(hydrogenElement, 6);
    siliconeOil->AddElement(oxygenElement, 1);
    siliconeOil->AddElement(siliconElement, 1);

    const auto fp = new G4Material("FP", 1.43_g_cm3, 4, kStateSolid);
    fp->AddElement(carbonElement, 2);
    fp->AddElement(hydrogenElement, 2);
    fp->AddElement(chlorineElement, 1);
    fp->AddElement(fluorineElement, 5);

    const auto pmma = new G4Material("PMMA", 1.19_g_cm3, 3, kStateSolid);
    pmma->AddElement(carbonElement, 5);
    pmma->AddElement(hydrogenElement, 8);
    pmma->AddElement(oxygenElement, 2);

    const auto ps = new G4Material("PS", 1.05_g_cm3, 2, kStateSolid);
    ps->AddElement(carbonElement, 1);
    ps->AddElement(hydrogenElement, 1);
    ps->GetIonisation()->SetBirksConstant(0.126); // https://geant4-forum.web.cern.ch/t/birks-constant-for-different-materials/1014/3

    const auto epoxy{new G4Material("Epoxy", 1.18_g_cm3, 3, kStateSolid)};
    epoxy->AddElement(carbonElement, 0.7362);
    epoxy->AddElement(hydrogenElement, 0.0675);
    epoxy->AddElement(oxygenElement, 0.1963);

    //////////////////////////////////////////////////
    // Construct Material Optical Properties Tables
    //////////////////////////////////////////////////

    constexpr auto fLambdaMin = 200_nm;
    constexpr auto fLambdaMax = 700_nm;
    std::vector<G4double> fEnergyPair{h_Planck * c_light / fLambdaMax,
                                      h_Planck * c_light / fLambdaMin};

    std::vector<G4double> scintillationEnergyBin(sciFiTracker.ScintillationWaveLengthBin().size());
    std::ranges::transform(sciFiTracker.ScintillationWaveLengthBin(),
                           scintillationEnergyBin.begin(),
                           [](auto val) { return h_Planck * c_light / (val * 1_nm); });
    std::vector<G4double> scintillationComponent(sciFiTracker.ScintillationComponent1().size());
    for (int i{}; i < std::ssize(sciFiTracker.ScintillationComponent1()); i++) {
        scintillationComponent[i] = sciFiTracker.ScintillationComponent1()[i] * h_Planck * c_light / scintillationEnergyBin[i] / scintillationEnergyBin[i];
    }

    //============================================ Optical Window =====================================

    const auto siliconeOilPropertiesTable{new G4MaterialPropertiesTable()};
    siliconeOilPropertiesTable->AddProperty("RINDEX", fEnergyPair, {1.465, 1.465});
    siliconeOil->SetMaterialPropertiesTable(siliconeOilPropertiesTable);

    const auto epoxyPropertiesTable{new G4MaterialPropertiesTable()};
    epoxyPropertiesTable->AddProperty("RINDEX", fEnergyPair, {1.55, 1.55});
    epoxy->SetMaterialPropertiesTable(epoxyPropertiesTable);

    const auto plasticPropertiesTable = new G4MaterialPropertiesTable();
    plasticPropertiesTable->AddProperty("RINDEX", fEnergyPair, {1.59, 1.59});
    plasticPropertiesTable->AddProperty("ABSLENGTH", fEnergyPair, {4_m, 4_m});
    plasticPropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1", scintillationEnergyBin, scintillationComponent);
    plasticPropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 8000);
    plasticPropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", sciFiTracker.ScintillationTimeConstant1());
    plasticPropertiesTable->AddConstProperty("RESOLUTIONSCALE", 1.0);
    ps->SetMaterialPropertiesTable(plasticPropertiesTable);

    const auto pmmaPropertiesTable{new G4MaterialPropertiesTable()};
    pmmaPropertiesTable->AddProperty("RINDEX", fEnergyPair, {1.49, 1.49});
    pmmaPropertiesTable->AddProperty("ABSLENGTH", fEnergyPair, {4_m, 4_m});
    pmma->SetMaterialPropertiesTable(pmmaPropertiesTable);

    const auto fpPropertiesTable{new G4MaterialPropertiesTable()};
    fpPropertiesTable->AddProperty("RINDEX", fEnergyPair, {1.363, 1.363});
    fpPropertiesTable->AddProperty("ABSLENGTH", fEnergyPair, {4_m, 4_m});
    fp->SetMaterialPropertiesTable(fpPropertiesTable);

    //============================================ Surface ============================================

    const auto sipmSurfacePropertiesTable{new G4MaterialPropertiesTable()};
    sipmSurfacePropertiesTable->AddProperty("REFLECTIVITY", fEnergyPair, {0., 0.});
    sipmSurfacePropertiesTable->AddProperty("EFFICIENCY", sciFiTracker.SiPMEnergyBin(), sciFiTracker.SiPMQuantumEfficiency());

    const auto absorbSurfacePropertiesTable{new G4MaterialPropertiesTable};
    absorbSurfacePropertiesTable->AddProperty("REFLECTIVITY", fEnergyPair, {0, 0});

    /////////////////////////////////////////////
    // Construct Volumes
    /////////////////////////////////////////////

    /////////////////////////////////////////
    ///////////solid and logical/////////////
    /////////////////////////////////////////

    const auto solidBracket{Make<G4Tubs>(scifiName + "Bracket",
                                         sciFiTracker.BracketInnerRadius(),
                                         sciFiTracker.BracketOuterRadius(),
                                         sciFiTracker.FiberLength() / 2, 0,
                                         2_pi)};
    const auto logicalBracket{
        Make<G4LogicalVolume>(solidBracket,
                              G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), // G4_PLASTIC_SC_VINYLTOLUENE or G4_AIR
                              scifiName + "Bracket")};
    Make<G4PVPlacement>(G4Transform3D{},
                        logicalBracket,
                        scifiName + "Bracket",
                        Mother().LogicalVolume(),
                        false,
                        0,
                        checkOverlaps);

    const auto solidSiPM{
        Make<G4Box>(scifiName + "SiPM", sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiPMWidth() / 2,
                    (sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() + sciFiTracker.EpoxyThickness()) / 2)};
    const auto logicalSiPM{Make<G4LogicalVolume>(solidSiPM, silicon, scifiName + "SiPM")};

    const auto solidEpoxy{
        Make<G4Box>(scifiName + "Epoxy",
                    sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiPMWidth() / 2,
                    sciFiTracker.EpoxyThickness() / 2)};
    const auto logicalEpoxy{Make<G4LogicalVolume>(solidEpoxy, epoxy, scifiName + "Epoxy")};

    const auto solidSiliconeOil{
        Make<G4Box>(scifiName + "SiliconeOil",
                    sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiPMWidth() / 2,
                    sciFiTracker.SiliconeOilThickness() / 2)};
    const auto logicalSiliconeOil{Make<G4LogicalVolume>(solidSiliconeOil, siliconeOil, scifiName + "SiliconeOil")};

    const auto solidAbsorbLayer{
        Make<G4Box>(scifiName + "AbsorbLayer",
                    sciFiTracker.FiberCladdingWidth() / 2,
                    sciFiTracker.FiberCladdingWidth() / 2,
                    (sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() + sciFiTracker.EpoxyThickness()) / 2)};
    const auto logicalAbsorbLayer{Make<G4LogicalVolume>(solidAbsorbLayer, silicon, scifiName + "AbsorbLayer")};

    Make<G4PVPlacement>(
        G4TranslateZ3D{(sciFiTracker.SiliconeOilThickness() - sciFiTracker.SiPMThickness()) / 2},
        logicalEpoxy,
        scifiName + "Epoxy",
        logicalSiPM,
        false,
        0,
        checkOverlaps);

    Make<G4PVPlacement>(
        G4TranslateZ3D{-(sciFiTracker.EpoxyThickness() + sciFiTracker.SiPMThickness()) / 2},
        logicalSiliconeOil,
        scifiName + "SiliconeOil",
        logicalSiPM,
        false,
        0,
        checkOverlaps);

    auto logicalHelicalFiber{
        [&](auto helicalRadius, auto fiberCladdingWidth, auto fiberCoreWidth, auto pitch, auto id) {
            const auto solidHelicalFiberCladding{Make<Mustard::Geant4X::HelicalBox>(
                fmt::format("{}HelicalFiber_{}", scifiName, id),
                helicalRadius,
                fiberCladdingWidth,
                pitch,
                0,
                2_pi,
                true,
                true,
                0.001)};
            const auto logicalHelicalFiberCladding{Make<G4LogicalVolume>(
                solidHelicalFiberCladding,
                pmma,
                scifiName + "HelicalFiber")};
            const auto solidHelicalFiberCore{Make<Mustard::Geant4X::HelicalBox>(
                fmt::format("{}HelicalFiberCore_{}", scifiName, id),
                helicalRadius,
                fiberCoreWidth,
                pitch,
                0,
                2_pi,
                true,
                true,
                0.001)};
            const auto logicalHelicalFiberCore{
                Make<G4LogicalVolume>(solidHelicalFiberCore,
                                      ps,
                                      scifiName + "HelicalFiberCore")};

            Make<G4PVPlacement>(G4Transform3D{},
                                logicalHelicalFiberCore,
                                scifiName + "HelicalFiber",
                                logicalHelicalFiberCladding,
                                false,
                                0,
                                checkOverlaps);
            return logicalHelicalFiberCladding;
        }};

    auto logicalHelicalLightGuide{
        [&](auto helicalRadius, auto fiberCladdingWidth, auto fiberCoreWidth, auto pitch, auto rotationAngle, auto id) {
            const auto solidHelicalLightGuideCladding{Make<Mustard::Geant4X::HelicalBox>(
                fmt::format("{}HelicalLightGuide_{}", scifiName, id),
                helicalRadius,
                fiberCladdingWidth,
                pitch,
                0,
                rotationAngle,
                true,
                false,
                0.001)};

            const auto solidHelicalLightGuideCore{Make<Mustard::Geant4X::HelicalBox>(
                fmt::format("{}HelicalLightGuideCore_{}", scifiName, id),
                helicalRadius,
                fiberCoreWidth,
                pitch,
                0,
                rotationAngle,
                true,
                false,
                0.001)};

            const auto logicalHelicalLightGuideCladding{Make<G4LogicalVolume>(
                solidHelicalLightGuideCladding,
                fp,
                scifiName + "HelicalLightGuide")};

            const auto logicalHelicalLightGuideCore{Make<G4LogicalVolume>(
                solidHelicalLightGuideCore,
                pmma,
                scifiName + "HelicalLightGuideCore")};

            Make<G4PVPlacement>(G4Transform3D{},
                                logicalHelicalLightGuideCore,
                                scifiName + "HelicalLightGuide",
                                logicalHelicalLightGuideCladding,
                                false,
                                0,
                                checkOverlaps);

            return logicalHelicalLightGuideCladding;
        }};

    auto logicalAxialFiber{[&](auto axialFiberCladdingWidth, auto axialFiberCoreWidth, auto fiberLength, auto id) {
        const auto solidAxialFiber{Make<G4Box>(
            fmt::format("{}AxialFiber_{}", scifiName, id),
            axialFiberCladdingWidth / 2,
            axialFiberCladdingWidth / 2,
            fiberLength / 2)};

        const auto logicalAxialFiber{Make<G4LogicalVolume>(
            solidAxialFiber,
            pmma,
            scifiName + "AxialFiber")};
        const auto solidAxialCore{Make<G4Box>(
            fmt::format("{}AxialFiberCore_{}", scifiName, id),
            axialFiberCoreWidth / 2,
            axialFiberCoreWidth / 2,
            fiberLength / 2)};

        const auto logicalAxialCore{
            Make<G4LogicalVolume>(solidAxialCore,
                                  ps,
                                  scifiName + "AxialFiberCore")};
        Make<G4PVPlacement>(G4Transform3D{},
                            logicalAxialCore,
                            scifiName + "AxialFiber",
                            logicalAxialFiber,
                            false,
                            0,
                            checkOverlaps);

        return logicalAxialFiber;
    }};

    auto logicalStraightLightGuide{[&](auto axialLightGuideCladdingWidth, auto axialLightGuideCoreWidth, auto lightGuideLength, auto id) {
        const auto solidAxialLightGuideCladding{Make<G4Box>(
            fmt::format("{}AxialLightGuide_{}", scifiName, id),
            axialLightGuideCladdingWidth / 2,
            axialLightGuideCladdingWidth / 2,
            lightGuideLength / 2)};

        const auto solidAxialLightGuideCore{Make<G4Box>(
            fmt::format("{}AxialLightGuideCore_{}", scifiName, id),
            axialLightGuideCoreWidth / 2,
            axialLightGuideCoreWidth / 2,
            lightGuideLength / 2)};

        const auto logicalAxialLightGuide{
            Make<G4LogicalVolume>(solidAxialLightGuideCladding,
                                  fp,
                                  scifiName + "AxialLightGuide")};
        const auto logicalAxialLightGuideCore{
            Make<G4LogicalVolume>(solidAxialLightGuideCore,
                                  pmma,
                                  scifiName + "AxialLightGuideCore")};
        Make<G4PVPlacement>(G4Transform3D{},
                            logicalAxialLightGuideCore,
                            scifiName + "AxialLightGuide",
                            logicalAxialLightGuide,
                            false,
                            0,
                            checkOverlaps);

        return logicalAxialLightGuide;
    }};

    auto logicalTubLightGuide{[&](auto axialLightGuideCladdingWidth, auto axialLightGuideCoreWidth, auto radius, auto pitch, auto id) {
        const auto solidTubLightGuideCladding{Make<G4Tubs>(
            fmt::format("{}AxialLightGuide_{}", scifiName, id),
            radius - axialLightGuideCladdingWidth / 2,
            radius + axialLightGuideCladdingWidth / 2,
            axialLightGuideCladdingWidth / 2, 0, pitch)};

        const auto solidTubLightGuideCore{Make<G4Tubs>(
            fmt::format("{}AxialLightGuideCore_{}", scifiName, id),
            radius - axialLightGuideCoreWidth / 2,
            radius + axialLightGuideCoreWidth / 2,
            axialLightGuideCoreWidth / 2, 0, pitch)};

        const auto logicalTubLightGuide{
            Make<G4LogicalVolume>(solidTubLightGuideCladding,
                                  fp,
                                  scifiName + "AxialLightGuide")};
        const auto logicalTubLightGuideCore{
            Make<G4LogicalVolume>(solidTubLightGuideCore,
                                  pmma,
                                  scifiName + "AxialLightGuideCore")};
        Make<G4PVPlacement>(G4Transform3D{},
                            logicalTubLightGuideCore,
                            scifiName + "AxialLightGuide",
                            logicalTubLightGuide,
                            false,
                            0,
                            checkOverlaps);

        return logicalTubLightGuide;
    }};

    /////////////////////////////////
    ///////////placement/////////////
    /////////////////////////////////
    int fiberID{};
    int sipmID{};
    const auto layerConfig{sciFiTracker.DetectorLayerConfiguration()};
    const auto fiberInformation{sciFiTracker.DetectorFiberInformation()};
    auto helicalPlacement{
        [&](auto layerID, auto logicalFiber, auto logicalHelicalLightGuide, auto logicalStraightLightGuide) {
            double helicalRadius{layerConfig[layerID].fiber.radius};
            int nFiber{layerConfig[layerID].nfiber};
            double pitch{layerConfig[layerID].fiber.pitch};
            double lightGuideAngle{sciFiTracker.HelicalLightGuideAngle().at(layerID / 4)};
            double siPMThickness{sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() + sciFiTracker.EpoxyThickness()};
            for (int i{}; i < nFiber; i++) {
                double rotationAngle{fiberInformation[fiberID].rotationAngle};
                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle},
                    logicalFiber,
                    fmt::format("{}HelicalFiber_{}", scifiName, fiberID),
                    logicalBracket,
                    false,
                    fiberID,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle + ((pitch > 0) ? 0 : 1_pi)} *
                        G4Translate3D{0, 0, -sciFiTracker.FiberLength() / 2 - (helicalRadius * lightGuideAngle * std::abs(std::tan(pitch))) / 2} *
                        G4RotateY3D{1.5 * 1_pi - std::copysign(1, pitch) * 0.5 * 1_pi},
                    logicalHelicalLightGuide,
                    scifiName + "HelicalLightGuide1",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle + ((pitch > 0) ? 0 : 1_pi)} *
                        G4Translate3D{0, 0, sciFiTracker.FiberLength() / 2 + (helicalRadius * lightGuideAngle * std::abs(std::tan(pitch))) / 2} *
                        G4RotateY3D{1.5 * 1_pi + std::copysign(1, pitch) * 0.5 * 1_pi},
                    logicalHelicalLightGuide,
                    scifiName + "HelicalLightGuide1",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle - lightGuideAngle + ((pitch > 0) ? -1_pi : 1.5_pi)} *
                        G4Translate3D{0,
                                      helicalRadius,
                                      sciFiTracker.FiberLength() / 2 + (helicalRadius * lightGuideAngle * std::abs(std::tan(pitch)))} *
                        G4RotateY3D{(3 * 1_pi / 2 + std::abs(pitch)) * std::copysign(1, pitch)} *
                        G4Translate3D{0, 0, sciFiTracker.StraightLightGuideExtensionLength() / 2},
                    logicalStraightLightGuide,
                    fmt::format("{}HelicalLightGuide2", scifiName),
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                fiberID++;
                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle - lightGuideAngle + ((pitch > 0) ? -1_pi : 1.5_pi)} *
                        G4Translate3D{0,
                                      helicalRadius,
                                      sciFiTracker.FiberLength() / 2 + (helicalRadius * lightGuideAngle * std::abs(std::tan(pitch)))} *
                        G4RotateY3D{(3 * 1_pi / 2 + std::abs(pitch)) * std::copysign(1, pitch)} *
                        G4Translate3D{0, 0, sciFiTracker.StraightLightGuideExtensionLength() + siPMThickness / 2},
                    logicalSiPM,
                    fmt::format("{}SiPM", scifiName),
                    Mother().LogicalVolume(),
                    false,
                    sipmID,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle} *
                        G4Translate3D{0,
                                      helicalRadius,
                                      -sciFiTracker.FiberLength() / 2 - (helicalRadius * lightGuideAngle * std::abs(std::tan(pitch)))} *
                        G4RotateY3D{-1_pi / 2 + pitch} *
                        G4Translate3D{0, 0, -std::copysign(1, pitch) * siPMThickness / 2},
                    logicalAbsorbLayer,
                    scifiName + "AbsorbLayer",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                sipmID++;
            }
        }};

    auto axialPlacement{
        [&](auto layerID, auto logicalFiber, auto logicalEntryStraightLightGuide, auto logicalTubLightGuide) {
            double radius{layerConfig[layerID].fiber.radius};
            int nFiber{layerConfig[layerID].nfiber};
            double lightGuideEntryLength{sciFiTracker.LightGuideEntryLength().at(layerID / 4)};
            double lightGuideCurvatureRadius{sciFiTracker.LightGuideCurvatureRadius()};
            double pitch{sciFiTracker.FiberLength() / (2_pi * radius)};
            double siPMThickness{sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() + sciFiTracker.EpoxyThickness()};
            for (int i{}; i < nFiber; i++) {
                double rotationAngle{fiberInformation[fiberID].rotationAngle};
                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle} *
                        G4Transform3D{{}, G4ThreeVector(radius, 0, 0)},
                    logicalFiber,
                    fmt::format("{}AxialFiber_{}", scifiName, fiberID),
                    logicalBracket,
                    false,
                    fiberID,
                    checkOverlaps);
                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle} *
                        G4Transform3D{{},
                                      G4ThreeVector(radius, 0, sciFiTracker.FiberLength() / 2 + lightGuideEntryLength / 2)},
                    logicalEntryStraightLightGuide,
                    scifiName + "AxialLightGuide1",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle} *
                        G4Transform3D({}, G4ThreeVector(radius, 0, -(sciFiTracker.FiberLength() / 2 + sciFiTracker.LightGuideEntryLength().at(layerID / 4) / 2))),
                    logicalEntryStraightLightGuide,
                    scifiName + "AxialLightGuide1",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle} *
                        G4Transform3D({}, G4ThreeVector(lightGuideCurvatureRadius + radius, 0, sciFiTracker.FiberLength() / 2 + lightGuideEntryLength)) *
                        G4RotateX3D{0.5_pi} *
                        G4RotateY3D{1_pi},
                    logicalTubLightGuide,
                    scifiName + "AxialLightGuide2",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                fiberID++;

                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle} *
                        G4Translate3D(radius + lightGuideCurvatureRadius * (1 - std::cos(pitch)), 0,
                                      sciFiTracker.FiberLength() / 2 + lightGuideEntryLength + lightGuideCurvatureRadius * std::sin(pitch)) *
                        G4Translate3D{siPMThickness / 2 * std::sin(pitch), 0, siPMThickness / 2 * std::cos(pitch)} *
                        G4RotateY3D(pitch),
                    logicalSiPM,
                    fmt::format("{}SiPM_{}", scifiName, sipmID),
                    Mother().LogicalVolume(),
                    false,
                    sipmID,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{rotationAngle} *
                        G4Transform3D{
                            {},
                            G4ThreeVector(radius,
                                          0,
                                          -(siPMThickness + // clang-format off
                                            sciFiTracker.FiberLength()) / 2 -
                                            lightGuideEntryLength)}, // clang-format on
                    logicalAbsorbLayer,
                    scifiName + "AbsorbLayer",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                sipmID++;
            }
        }};

    for (int i{}; i < sciFiTracker.NLayer(); i++) {
        if (layerConfig[i].fiber.layerType == "LHelical") {
            auto logicalLHFiber{logicalHelicalFiber(
                layerConfig[i].fiber.radius,
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch, fiberID)};

            auto logicalHLightGuide{logicalHelicalLightGuide(
                layerConfig[i].fiber.radius,
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch,
                sciFiTracker.HelicalLightGuideAngle().at(i / 4),
                fiberID)};

            auto logicalALightGuide{logicalStraightLightGuide(
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                sciFiTracker.StraightLightGuideExtensionLength(), fiberID)};

            helicalPlacement(i,
                             logicalLHFiber,
                             logicalHLightGuide,
                             logicalALightGuide);
        } else if (layerConfig[i].fiber.layerType == "RHelical") {
            auto logicalRHFiber{logicalHelicalFiber(
                layerConfig[i].fiber.radius,
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch, fiberID)};

            auto logicalHLightGuide{logicalHelicalLightGuide(
                layerConfig[i].fiber.radius,
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch,
                sciFiTracker.HelicalLightGuideAngle().at(i / 4),
                fiberID)};

            auto logicalALightGuide{logicalStraightLightGuide(
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                sciFiTracker.StraightLightGuideExtensionLength(), fiberID)};

            helicalPlacement(i,
                             logicalRHFiber,
                             logicalHLightGuide,
                             logicalALightGuide);
        } else if (layerConfig[i].fiber.layerType == "Axial") {
            auto logicalAFiber{logicalAxialFiber(
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                sciFiTracker.FiberLength(), fiberID)};

            auto logicalEntryStraightLightGuide{logicalStraightLightGuide(
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                sciFiTracker.LightGuideEntryLength().at(i / 4), fiberID)};

            auto logicalTLightGuide{logicalTubLightGuide(
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                sciFiTracker.LightGuideCurvatureRadius(),
                sciFiTracker.FiberLength() / (2_pi * layerConfig[i].fiber.radius),
                fiberID)};

            axialPlacement(i,
                           logicalAFiber,
                           logicalEntryStraightLightGuide,
                           logicalTLightGuide);
        }
    }

    /////////////////////////////////////////////
    // Construct Optical Surface
    /////////////////////////////////////////////

    const auto sipmSurface{new G4OpticalSurface("SiPMSurface", unified, polished, dielectric_metal)};
    sipmSurface->SetMaterialPropertiesTable(sipmSurfacePropertiesTable);
    new G4LogicalSkinSurface{"SiPMSurface", logicalSiPM, sipmSurface};

    const auto absorbSurface{new G4OpticalSurface("AbsorbSurface", unified, polished, dielectric_metal)};
    absorbSurface->SetMaterialPropertiesTable(absorbSurfacePropertiesTable);
    new G4LogicalSkinSurface{"AbsorbSurface", logicalAbsorbLayer, absorbSurface};
}

} // namespace MACE::PhaseI::Detector::Definition
