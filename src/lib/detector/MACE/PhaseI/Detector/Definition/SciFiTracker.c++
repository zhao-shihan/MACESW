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

    const auto windowPropertiesTable = new G4MaterialPropertiesTable();
    windowPropertiesTable->AddProperty("RINDEX", fEnergyPair, {1.55, 1.55});
    epoxy->SetMaterialPropertiesTable(windowPropertiesTable);

    const auto siliconeOilPropertiesTable{new G4MaterialPropertiesTable()};
    siliconeOilPropertiesTable->AddProperty("RINDEX", fEnergyPair, {1.465, 1.465});
    siliconeOilPropertiesTable->AddProperty("ABSLENGTH", fEnergyPair, {40_cm, 40_cm});
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

    const auto rfSurfacePropertiesTable{new G4MaterialPropertiesTable};
    rfSurfacePropertiesTable->AddProperty("REFLECTIVITY", fEnergyPair, {0, 0});

    /////////////////////////////////////////////
    // Construct Volumes
    /////////////////////////////////////////////

    /////////////////////////////////////////
    ///////////solid and logical/////////////
    /////////////////////////////////////////
    for (int i{}; i < sciFiTracker.NLayer(); i += 2) {
        const auto solidBracket1{Make<G4Tubs>(scifiName + "Bracket",
                                              sciFiTracker.RLayer()->at(i) - sciFiTracker.FiberCladdingWidth() - 2,
                                              sciFiTracker.RLayer()->at(i) - sciFiTracker.FiberCladdingWidth(),
                                              sciFiTracker.FiberLength() / 2, 0,
                                              2_pi)};
        const auto logicalBracket1{
            Make<G4LogicalVolume>(solidBracket1,
                                  G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),
                                  scifiName + "Bracket")};
        Make<G4PVPlacement>(G4Transform3D{},
                            logicalBracket1,
                            scifiName + "Bracket",
                            Mother().LogicalVolume(),
                            false,
                            0,
                            checkOverlaps);
    }

    const auto solidSiPM{
        Make<G4Box>(scifiName + "SiPM", sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiPMWidth() / 2,
                    (sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() + sciFiTracker.EpoxyThickness()) / 2)};
    const auto logicalSiPM{Make<G4LogicalVolume>(solidSiPM, silicon, scifiName + "SiPM")};

    const auto solidEpoxy{
        Make<G4Box>(scifiName + "Epoxy",
                    sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.EpoxyThickness() / 2)};
    const auto logicalEpoxy{Make<G4LogicalVolume>(solidEpoxy, epoxy, scifiName + "Epoxy")};

    const auto solidSiliconeOil{
        Make<G4Box>(scifiName + "SiliconeOil",
                    sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiliconeOilThickness() / 2)};
    const auto logicalSiliconeOil{Make<G4LogicalVolume>(solidSiliconeOil, siliconeOil, scifiName + "SiliconeOil")};

    const auto solidAbsorbLayer{
        Make<G4Box>(scifiName + "AbsorbLayer",
                    sciFiTracker.SiPMLength() / 2,
                    sciFiTracker.SiPMLength() / 2,
                    (sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() + sciFiTracker.EpoxyThickness()) / 2)};
    const auto logicalAbsorbLayer{Make<G4LogicalVolume>(solidAbsorbLayer, silicon, "SciFiAbsorbLayer")};

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

    auto rotationVector{
        [&](double i, double pitch, double x0, double y0, double pmZ) {
            return CLHEP::Hep3Vector(
                std::cos(i) * x0 - std::sin(i) * y0,
                std::cos(i) * y0 + std::sin(i) * x0,
                pmZ * ((sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() + sciFiTracker.EpoxyThickness()) / 2 +
                       sciFiTracker.FiberLength() / 2 + sciFiTracker.LightGuideCurvature() * std::cos(pitch)));
        }};

    auto logicalHelicalFiber{
        [&](auto helicalRadius, auto fiberCladdingWidth, auto fiberCoreWidth, auto pitch) {
            const auto solidHelicalFiberCladding{Make<Mustard::Geant4X::HelicalBox>(
                scifiName + "HelicalFiber",
                helicalRadius,
                fiberCladdingWidth,
                pitch,
                0,
                2_pi,
                0.001)};
            const auto logicalHelicalFiberCladding{Make<G4LogicalVolume>(
                solidHelicalFiberCladding,
                pmma,
                scifiName + "HelicalFiber")};
            const auto solidHelicalFiberCore{Make<Mustard::Geant4X::HelicalBox>(
                scifiName + "HelicalFiberCore",
                helicalRadius,
                fiberCoreWidth,
                pitch,
                0,
                2_pi,
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
        [&](auto curvature, auto fiberCladdingWidth, auto fiberCoreWidth, auto pitch) {
            const auto solidHelicalLightGuideCladding{Make<G4Tubs>(
                scifiName + "HelicalLightGuide",
                curvature - fiberCladdingWidth / 2,
                curvature + fiberCladdingWidth / 2,
                fiberCladdingWidth / 2,
                0,
                0.5_pi - std::abs(pitch))};

            const auto solidHelicalLightGuideCore{Make<G4Tubs>(
                scifiName + "HelicalLightGuideCore",
                curvature - fiberCoreWidth / 2,
                curvature + fiberCoreWidth / 2,
                fiberCoreWidth / 2,
                0,
                0.5_pi - std::abs(pitch))};

            const auto logicalHelicalLightGuideCladding{Make<G4LogicalVolume>(
                solidHelicalLightGuideCladding,
                fp,
                scifiName + "HelicalLightGuide")};

            const auto logicalHelicalLightGuideCore{Make<G4LogicalVolume>(
                solidHelicalLightGuideCore,
                pmma,
                scifiName + "HelicalLightGuide")};

            Make<G4PVPlacement>(G4Transform3D{},
                                logicalHelicalLightGuideCore,
                                scifiName + "HelicalLightGuide",
                                logicalHelicalLightGuideCladding,
                                false,
                                0,
                                checkOverlaps);

            return logicalHelicalLightGuideCladding;
        }};

    auto logicalTransverseFiber{[&](auto transverseFiberCladdingWidth, auto transverseFiberCoreWidth, auto fiberLength) {
        const auto solidTransverseFiber{Make<G4Box>(
            scifiName + "TransverseFiber",
            transverseFiberCladdingWidth / 2,
            transverseFiberCladdingWidth / 2,
            fiberLength / 2)};

        const auto logicalTransverseFiber{Make<G4LogicalVolume>(
            solidTransverseFiber,
            pmma,
            scifiName + "TransverseFiber")};
        const auto solidTransverseCore{Make<G4Box>(
            scifiName + "TransverseFiberCore",
            transverseFiberCoreWidth / 2,
            transverseFiberCoreWidth / 2,
            fiberLength / 2)};

        const auto logicalTransverseCore{
            Make<G4LogicalVolume>(solidTransverseCore,
                                  ps,
                                  scifiName + "TransverseFiberCore")};
        Make<G4PVPlacement>(G4Transform3D{},
                            logicalTransverseCore,
                            scifiName + "TransverseFiber",
                            logicalTransverseFiber,
                            false,
                            0,
                            checkOverlaps);

        return logicalTransverseFiber;
    }};

    auto logicalTransverseLightGuide{[&](auto transverseLightGuideCladdingWidth, auto transverseLightGuideCoreWidth, auto lightGuideLength) {
        const auto solidTransverseLightGuideCladding{Make<G4Box>(
            scifiName + "TransverseLightGuide",
            transverseLightGuideCladdingWidth / 2,
            transverseLightGuideCladdingWidth / 2,
            lightGuideLength / 2)};

        const auto solidTransverseLightGuideCore{Make<G4Box>(
            scifiName + "TransverseLightGuide",
            transverseLightGuideCoreWidth / 2,
            transverseLightGuideCoreWidth / 2,
            lightGuideLength / 2)};

        const auto logicalTransverseLightGuide{
            Make<G4LogicalVolume>(solidTransverseLightGuideCladding,
                                  fp,
                                  scifiName + "TransverseLightGuide")};
        const auto logicalTransverseLightGuideCore{
            Make<G4LogicalVolume>(solidTransverseLightGuideCore,
                                  pmma,
                                  scifiName + "TransverseLightGuide")};
        Make<G4PVPlacement>(G4Transform3D{},
                            logicalTransverseLightGuideCore,
                            scifiName + "TransverseLightGuide",
                            logicalTransverseLightGuide,
                            false,
                            0,
                            checkOverlaps);

        return logicalTransverseLightGuide;
    }};

    /////////////////////////////////
    ///////////placement/////////////
    /////////////////////////////////
    int fiberNumber{};
    int sipmNumber{};
    auto helicalPlacement{
        [&](auto helicalRadius, auto logicalFiber, auto logicalLightGuide, auto nFiber, auto pitch, auto curvature, int second) {
            for (int i{}; i < nFiber; i++) {
                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber},
                    logicalFiber,
                    scifiName + "HelicalFiber_" + std::to_string(fiberNumber),
                    Mother().LogicalVolume(),
                    false,
                    fiberNumber,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber} *
                        G4RotateY3D{0.5_pi} *
                        G4Translate3D{-sciFiTracker.FiberLength() / 2, 0, std::copysign(helicalRadius, pitch)} *
                        G4RotateZ3D{std::abs(pitch)} *
                        G4TranslateX3D{-curvature},
                    logicalLightGuide,
                    scifiName + "HelicalLightGuide",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber} *
                        G4RotateY3D{0.5_pi} *
                        G4Translate3D{sciFiTracker.FiberLength() / 2, 0, std::copysign(helicalRadius, pitch)} *
                        G4RotateZ3D{std::abs(pitch) + 1_pi} *
                        G4TranslateX3D{-curvature},
                    logicalLightGuide,
                    scifiName + "HelicalLightGuide",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                fiberNumber++;
                Make<G4PVPlacement>(
                    G4Transform3D{
                        CLHEP::HepRotationZ{(i + second * 0.5) * 2_pi / nFiber},
                        rotationVector((i + second * 0.5) * 2_pi / nFiber,
                                       pitch,
                                       std::copysign(helicalRadius, pitch),
                                       curvature * (1 - std::abs(std::sin(pitch))), 1)},
                    logicalSiPM,
                    scifiName + "SiPM",
                    Mother().LogicalVolume(),
                    false,
                    sipmNumber,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4Transform3D{
                        CLHEP::HepRotationZ{(i + second * 0.5) * 2_pi / nFiber},
                        rotationVector((i + second * 0.5) * 2_pi / nFiber,
                                       pitch,
                                       -std::copysign(helicalRadius, pitch),
                                       curvature * (1 - std::abs(std::sin(pitch))), -1)},
                    logicalAbsorbLayer,
                    scifiName + "AbsorbLayer",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                sipmNumber++;
            }
        }};

    auto transversePlacement{
        [&](auto radius, auto logicalFiber, auto logicalLightGuide, auto nFiber, int second) {
            for (int i{}; i < nFiber; i++) {
                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber} *
                        G4Transform3D{{}, G4ThreeVector(radius, 0, 0)},
                    logicalFiber,
                    scifiName + "TransverseFiber_" + std::to_string(fiberNumber),
                    Mother().LogicalVolume(),
                    false,
                    fiberNumber,
                    checkOverlaps);
                fiberNumber++;
                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber} *
                        G4Transform3D{{},
                                      G4ThreeVector(radius, 0, sciFiTracker.FiberLength() / 2 + sciFiTracker.TLightGuideLength() / 2)},
                    logicalLightGuide,
                    scifiName + "TransverseLightGuide",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber} *
                        G4Transform3D({}, G4ThreeVector(radius, 0, -(sciFiTracker.FiberLength() / 2 + sciFiTracker.TLightGuideLength() / 2))),
                    logicalLightGuide,
                    scifiName + "TransverseLightGuide",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber} *
                        G4Transform3D{{},
                                      G4ThreeVector(radius,
                                                    0,
                                                    (sciFiTracker.SiPMThickness() + sciFiTracker.SiliconeOilThickness() +
                                                     sciFiTracker.EpoxyThickness() + // clang-format off
                                                     sciFiTracker.FiberLength()) / 2 + 
                                                     sciFiTracker.TLightGuideLength())}, // clang-format on
                    logicalSiPM,
                    scifiName + "SiPM",
                    Mother().LogicalVolume(),
                    false,
                    sipmNumber,
                    checkOverlaps);

                Make<G4PVPlacement>(
                    G4RotateZ3D{(i + second * 0.5) * 2_pi / nFiber} *
                        G4Transform3D{
                            {},
                            G4ThreeVector(radius,
                                          0,
                                          -(sciFiTracker.SiPMThickness() +
                                            sciFiTracker.SiliconeOilThickness() +
                                            sciFiTracker.EpoxyThickness() + // clang-format off
                                            sciFiTracker.FiberLength()) / 2 - 
                                            sciFiTracker.TLightGuideLength())}, // clang-format on
                    logicalAbsorbLayer,
                    scifiName + "AbsorbLayer",
                    Mother().LogicalVolume(),
                    false,
                    0,
                    checkOverlaps);
                sipmNumber++;
            }
        }};

    const auto layerConfig{sciFiTracker.DetectorLayerConfiguration()};
    for (int i{}; i < sciFiTracker.NLayer(); i++) {
        if (layerConfig[i].fiber.layerType == "LHelical") {
            auto logicalLHelicalFiber{logicalHelicalFiber(
                layerConfig[i].fiber.radius,
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch)};

            auto logicalLHelicalLightGuide{logicalHelicalLightGuide(
                sciFiTracker.LightGuideCurvature(),
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch)};

            helicalPlacement(layerConfig[i].fiber.radius,
                             logicalLHelicalFiber,
                             logicalLHelicalLightGuide,
                             layerConfig[i].lastID - layerConfig[i].firstID + 1, // number of fiber is (end-begin+1)
                             layerConfig[i].fiber.pitch,
                             sciFiTracker.LightGuideCurvature(),
                             layerConfig[i].isSecond);
        } else if (layerConfig[i].fiber.layerType == "RHelical") {
            auto logicalRHelicalFiber{logicalHelicalFiber(
                layerConfig[i].fiber.radius,
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch)};

            auto logicalRHelicalLightGuide{logicalHelicalLightGuide(
                sciFiTracker.LightGuideCurvature(),
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                layerConfig[i].fiber.pitch)};

            helicalPlacement(layerConfig[i].fiber.radius,
                             logicalRHelicalFiber,
                             logicalRHelicalLightGuide,
                             layerConfig[i].lastID - layerConfig[i].firstID + 1, // number of fiber is (end-begin+1)
                             layerConfig[i].fiber.pitch,
                             sciFiTracker.LightGuideCurvature(),
                             layerConfig[i].isSecond);
        } else if (layerConfig[i].fiber.layerType == "Transverse") {
            auto logicalTFiber{logicalTransverseFiber(
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                sciFiTracker.FiberLength())};

            auto logicalTLightGuide{logicalTransverseLightGuide(
                sciFiTracker.FiberCladdingWidth(),
                sciFiTracker.FiberCoreWidth(),
                sciFiTracker.TLightGuideLength())};

            transversePlacement(layerConfig[i].fiber.radius,
                                logicalTFiber,
                                logicalTLightGuide,
                                layerConfig[i].lastID - layerConfig[i].firstID + 1, // number of fiber is (end-begin+1)
                                layerConfig[i].isSecond);
        }
    }

    /////////////////////////////////////////////
    // Construct Optical Surface
    /////////////////////////////////////////////

    const auto sipmSurface{new G4OpticalSurface("SiPMSurface", unified, polished, dielectric_metal)};
    sipmSurface->SetMaterialPropertiesTable(sipmSurfacePropertiesTable);
    new G4LogicalSkinSurface{"SiPMSurface", logicalSiPM, sipmSurface};

    const auto absorbSurface{new G4OpticalSurface("AbsorbSurface", unified, polished, dielectric_metal)};
    absorbSurface->SetMaterialPropertiesTable(rfSurfacePropertiesTable);
    new G4LogicalSkinSurface{"AbsorbSurface", logicalAbsorbLayer, absorbSurface};
}

} // namespace MACE::PhaseI::Detector::Definition
