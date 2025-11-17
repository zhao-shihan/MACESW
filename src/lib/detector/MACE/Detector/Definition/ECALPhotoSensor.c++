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

#include "MACE/Detector/Definition/ECALCrystal.h++"
#include "MACE/Detector/Definition/ECALPhotoSensor.h++"
#include "MACE/Detector/Description/ECAL.h++"

#include "Mustard/Detector/Definition/DefinitionBase.h++"
#include "Mustard/Env/BasicEnv.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "G4AssemblyVolume.hh"
#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"

#include "fmt/format.h"

#include <algorithm>
#include <cassert>
#include <utility>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;

auto ECALPhotoSensor::Construct(G4bool checkOverlaps) -> void {
    if (Detector::Description::ECAL::Instance().UseMPPC()) {
        ConstructMPPC(checkOverlaps);
    } else {
        ConstructPMT(checkOverlaps);
    }
}

auto ECALPhotoSensor::ConstructMPPC(G4bool checkOverlaps) -> void {
    const auto& ecal{Description::ECAL::Instance()};
    const auto name{ecal.Name()};

    const auto mppcNPixelRows{ecal.MPPCNPixelRow()};
    const auto mppcPixelSizeSet{ecal.MPPCPixelSizeSet()};
    const auto mppcPitch{ecal.MPPCPitch()};
    const auto mppcThickness{ecal.MPPCThickness()};
    const auto mppcCouplerThickness{ecal.MPPCCouplerThickness()};
    const auto mppcWindowThickness{ecal.MPPCWindowThickness()};

    const auto mppcEnergyBin{ecal.MPPCEnergyBin()};
    const auto mppcEfficiency{ecal.MPPCEfficiency()};

    /////////////////////////////////////////////
    // Define Element and Material
    /////////////////////////////////////////////

    const auto nist{G4NistManager::Instance()};
    const auto hydrogenElement{nist->FindOrBuildElement("H")};
    const auto carbonElement{nist->FindOrBuildElement("C")};
    const auto oxygenElement{nist->FindOrBuildElement("O")};
    const auto siliconElement{nist->FindOrBuildElement("Si")};

    const auto silicon{nist->FindOrBuildMaterial("G4_Si")};

    const auto siliconeGrease{new G4Material("siliconeGrease", 1.06_g_cm3, 4, kStateLiquid)};
    siliconeGrease->AddElement(carbonElement, 2);
    siliconeGrease->AddElement(hydrogenElement, 6);
    siliconeGrease->AddElement(oxygenElement, 1);
    siliconeGrease->AddElement(siliconElement, 1);

    const auto epoxy{new G4Material("epoxy", 1.18_g_cm3, 3, kStateSolid)};
    epoxy->AddElement(carbonElement, 0.7362);
    epoxy->AddElement(hydrogenElement, 0.0675);
    epoxy->AddElement(oxygenElement, 0.1963);

    //////////////////////////////////////////////////
    // Construct Material Optical Properties Tables
    //////////////////////////////////////////////////

    const auto [minPhotonEnergy, maxPhotonEnergy]{std::ranges::minmax(ecal.ScintillationEnergyBin())};

    const auto siliconeGreasePropertiesTable{new G4MaterialPropertiesTable};
    siliconeGreasePropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, {1.46, 1.46}); // EJ-550
    siliconeGreasePropertiesTable->AddProperty("ABSLENGTH", {minPhotonEnergy, maxPhotonEnergy}, {100_cm, 100_cm});
    siliconeGrease->SetMaterialPropertiesTable(siliconeGreasePropertiesTable);

    const auto epoxyPropertiesTable{new G4MaterialPropertiesTable};
    epoxyPropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, {1.57, 1.57});
    epoxy->SetMaterialPropertiesTable(epoxyPropertiesTable);
    const auto couplerSurfacePropertiesTable{new G4MaterialPropertiesTable};
    couplerSurfacePropertiesTable->AddProperty("TRANSMITTANCE", {minPhotonEnergy, maxPhotonEnergy}, {1., 1.});

    const auto cathodeSurfacePropertiesTable{new G4MaterialPropertiesTable};
    cathodeSurfacePropertiesTable->AddProperty("REFLECTIVITY", {minPhotonEnergy, maxPhotonEnergy}, {0., 0.});
    cathodeSurfacePropertiesTable->AddProperty("EFFICIENCY", ecal.MPPCEnergyBin(), ecal.MPPCEfficiency());

    if (Mustard::Env::VerboseLevelReach<'V'>()) {
        cathodeSurfacePropertiesTable->DumpTable();
    }

    /////////////////////////////////////////////
    // Construct Volumes
    /////////////////////////////////////////////
    const auto& faceList{ecal.Mesh().faceList};
    const auto& moduleSelection{ecal.ModuleSelection()};
    std::map<int, std::vector<int>> idListOfType;
    for (int moduleID{}; moduleID < std::ssize(faceList); ++moduleID) {
        idListOfType[faceList.at(moduleID).typeID].emplace_back(moduleID);
    }

    std::vector<int> chosenType;
    chosenType.reserve(moduleSelection.size());
    for (auto&& chosen : moduleSelection) {
        chosenType.emplace_back(faceList.at(chosen).typeID);
    }
    for (auto&& [type, moduleIDList] : idListOfType) { // loop over type(10 total)
        if (not chosenType.empty() and std::ranges::find(chosenType, type) == chosenType.end()) {
            continue;
        }

        const auto mppcNPixelRow{mppcNPixelRows.at(type)};
        const auto mppcPixelSize{mppcPixelSizeSet.at(type)};
        const auto mppcWidth{mppcNPixelRow * (mppcPixelSize + mppcPitch) + mppcPitch};

        const auto solidCoupler{Make<G4Box>("temp", mppcWidth / 2, mppcWidth / 2, mppcCouplerThickness / 2)};
        const auto logicCoupler{Make<G4LogicalVolume>(solidCoupler, siliconeGrease, name + "MPPCCoupler")};

        const auto solidWindow{Make<G4Box>("temp", mppcWidth / 2, mppcWidth / 2, mppcWindowThickness / 2)};
        const auto logicWindow{Make<G4LogicalVolume>(solidWindow, epoxy, name + "MPPCWindow")};

        const auto solidPixel{Make<G4Box>("temp", mppcPixelSize / 2, mppcPixelSize / 2, mppcThickness / 2)};
        const auto logicPixel{Make<G4LogicalVolume>(solidPixel, silicon, name + "PMCathode")};
        for (int copyNo{}; copyNo < mppcNPixelRow * mppcNPixelRow; copyNo++) {
            const auto rowNum{copyNo / mppcNPixelRow};
            const auto colNum{copyNo % mppcNPixelRow};
            const auto xOffSet{(2 * rowNum + 1 - mppcNPixelRow) * ((mppcPixelSize + mppcPitch) / 2)};
            const auto yOffset{(2 * colNum + 1 - mppcNPixelRow) * ((mppcPixelSize + mppcPitch) / 2)};
            const auto zOffset{(mppcWindowThickness - mppcThickness) / 2};
            Make<G4PVPlacement>(
                G4Transform3D({}, {xOffSet, yOffset, zOffset}),
                logicPixel,
                logicPixel->GetName(),
                logicWindow,
                true,
                copyNo,
                checkOverlaps);
        }

        for (auto moduleID : moduleIDList) { // loop over ID.s of a type
            if ((not moduleSelection.empty()) and (std::ranges::find(moduleSelection, moduleID) == moduleSelection.end())) {
                continue;
            }
            const auto couplerTransform{ecal.ComputeTransformToOuterSurfaceWithOffset(moduleID,
                                                                                      mppcCouplerThickness / 2)};
            const auto windowTransform{ecal.ComputeTransformToOuterSurfaceWithOffset(moduleID,
                                                                                     mppcCouplerThickness + mppcWindowThickness / 2)};
            const auto physicalCoupler{Make<G4PVPlacement>(
                couplerTransform,
                logicCoupler,
                name + "MPPCCoupler",
                Mother().LogicalVolume(),
                true,
                moduleID,
                checkOverlaps)};
            // change volume window from epoxy to epoxy&silicon Pixels, may change name "window" later
            Make<G4PVPlacement>(
                windowTransform,
                logicWindow,
                name + "MPPCWindow",
                Mother().LogicalVolume(),
                true,
                moduleID,
                checkOverlaps);
            const auto ecalCrystal{FindSibling<ECALCrystal>()};
            if (ecalCrystal) {
                const auto couplerSurface{new G4OpticalSurface("coupler", unified, polished, dielectric_dielectric)};
                new G4LogicalBorderSurface{"couplerSurface",
                                           ecalCrystal->PhysicalVolume(name + fmt::format("Crystal_{}", moduleID)),
                                           physicalCoupler,
                                           couplerSurface};
                couplerSurface->SetMaterialPropertiesTable(couplerSurfacePropertiesTable);
            }
        }

        /////////////////////////////////////////////
        // Construct Optical Surface
        /////////////////////////////////////////////

        const auto cathodeSurface{new G4OpticalSurface("Cathode", unified, polished, dielectric_metal)};
        new G4LogicalSkinSurface{"cathodeSkinSurface", logicPixel, cathodeSurface};
        cathodeSurface->SetMaterialPropertiesTable(cathodeSurfacePropertiesTable);
    }
}

auto ECALPhotoSensor::ConstructPMT(G4bool checkOverlaps) -> void {
    const auto& ecal{Description::ECAL::Instance()};
    const auto name{ecal.Name()};

    const auto pmtCouplerThickness{ecal.PMTCouplerThickness()};
    const auto pmtWindowThickness{ecal.PMTWindowThickness()};
    const auto pmtCathodeThickness{ecal.PMTCathodeThickness()};

    const auto pmtEnergyBin{ecal.PMTEnergyBin()};
    const auto pmtQuantumEfficiency{ecal.PMTQuantumEfficiency()};

    /////////////////////////////////////////////
    // Define Element and Material
    /////////////////////////////////////////////

    const auto nist{G4NistManager::Instance()};
    const auto hydrogenElement{nist->FindOrBuildElement("H")};
    const auto carbonElement{nist->FindOrBuildElement("C")};
    const auto oxygenElement{nist->FindOrBuildElement("O")};
    const auto siliconElement{nist->FindOrBuildElement("Si")};
    const auto potassiumElement{nist->FindOrBuildElement("K")};
    const auto antimonyElement{nist->FindOrBuildElement("Sb")};
    const auto cesiumElement{nist->FindOrBuildElement("Cs")};

    const auto siliconeGrease{new G4Material("siliconeGrease", 1.06_g_cm3, 4, kStateLiquid)};
    siliconeGrease->AddElement(carbonElement, 2);
    siliconeGrease->AddElement(hydrogenElement, 6);
    siliconeGrease->AddElement(oxygenElement, 1);
    siliconeGrease->AddElement(siliconElement, 1);

    const auto glass{nist->FindOrBuildMaterial("G4_GLASS_PLATE")};
    const auto bialkali{new G4Material("Bialkali", 2.0_g_cm3, 3, kStateSolid)};
    bialkali->AddElement(potassiumElement, 2);
    bialkali->AddElement(cesiumElement, 1);
    bialkali->AddElement(antimonyElement, 1);

    //////////////////////////////////////////////////
    // Construct Material Optical Properties Tables
    //////////////////////////////////////////////////
    const auto [minPhotonEnergy, maxPhotonEnergy]{std::ranges::minmax(ecal.ScintillationEnergyBin())};
    const auto siliconeGreasePropertiesTable{new G4MaterialPropertiesTable};
    siliconeGreasePropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, {1.46, 1.46});
    siliconeGreasePropertiesTable->AddProperty("ABSLENGTH", {minPhotonEnergy, maxPhotonEnergy}, {100_cm, 100_cm});
    siliconeGrease->SetMaterialPropertiesTable(siliconeGreasePropertiesTable);

    const auto epoxyPropertiesTable{new G4MaterialPropertiesTable};
    epoxyPropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, {1.49, 1.49}); // ET 9269B 9956B
    glass->SetMaterialPropertiesTable(epoxyPropertiesTable);

    const std::vector<G4double>& cathodeSurfacePropertiesEnergy{pmtEnergyBin};
    const std::vector<G4double>& cathodeSurfacePropertiesEfficiency{pmtQuantumEfficiency};
    const auto couplerSurfacePropertiesTable{new G4MaterialPropertiesTable};
    couplerSurfacePropertiesTable->AddProperty("TRANSMITTANCE", {minPhotonEnergy, maxPhotonEnergy}, {1., 1.});

    const auto cathodeSurfacePropertiesTable{new G4MaterialPropertiesTable};
    cathodeSurfacePropertiesTable->AddProperty("REFLECTIVITY", {minPhotonEnergy, maxPhotonEnergy}, {0., 0.});
    cathodeSurfacePropertiesTable->AddProperty("EFFICIENCY", cathodeSurfacePropertiesEnergy, cathodeSurfacePropertiesEfficiency);

    if (Mustard::Env::VerboseLevelReach<'V'>()) {
        cathodeSurfacePropertiesTable->DumpTable();
    }

    /////////////////////////////////////////////
    // Construct Volumes
    /////////////////////////////////////////////

    const auto& faceList{ecal.Mesh().faceList};
    const auto& moduleSelection{ecal.ModuleSelection()};
    std::map<int, std::vector<int>> idListOfType;
    for (auto moduleID{0}; moduleID < std::ssize(faceList); ++moduleID) {
        idListOfType[faceList.at(moduleID).typeID].emplace_back(moduleID);
    }

    std::vector<int> chosenType;
    chosenType.reserve(moduleSelection.size());
    for (auto&& chosen : moduleSelection) {
        chosenType.emplace_back(faceList.at(chosen).typeID);
    }
    const auto& pmtDimensions{ecal.PMTDimensions()};
    for (auto&& [typeID, moduleIDList] : std::as_const(idListOfType)) {
        if ((not chosenType.empty()) and (std::ranges::find(chosenType, typeID) == chosenType.end())) {
            continue;
        }

        const auto [pmtDiameter, cathodeDiameter, pmtLength]{pmtDimensions.at(typeID)};

        const auto solidCoupler{Make<G4Tubs>("temp", 0, pmtDiameter / 2, pmtCouplerThickness / 2, 0, 2 * pi)};
        const auto logicCoupler{Make<G4LogicalVolume>(solidCoupler, siliconeGrease, name + "PMTCoupler")};

        const auto solidPMTShell{Make<G4Tubs>("temp", 0, pmtDiameter / 2, pmtLength / 2, 0, 2 * pi)}; // in fact it is empty in the middle(replaced by daughter volume, vacuum)
        const auto logicPMTShell{Make<G4LogicalVolume>(solidPMTShell, glass, name + "PMTShell")};

        const auto solidPMTVacuum{Make<G4Tubs>("temp", 0, pmtDiameter / 2 - pmtWindowThickness, pmtLength / 2 - pmtWindowThickness - pmtCathodeThickness / 2, 0, 2 * pi)};
        const auto logicPMTVacuum{Make<G4LogicalVolume>(solidPMTVacuum, nist->FindOrBuildMaterial("G4_Galactic"), name + "PMTVacuum")};

        const auto solidCathode{Make<G4Tubs>("temp", 0, cathodeDiameter / 2, pmtCathodeThickness / 2, 0, 2 * pi)};
        const auto logicCathode{Make<G4LogicalVolume>(solidCathode, bialkali, name + "PMCathode")};

        for (auto&& moduleID : moduleIDList) {
            if ((not moduleSelection.empty()) and (std::ranges::find(moduleSelection, moduleID) == moduleSelection.end())) {
                continue;
            }

            const auto couplerTransform{ecal.ComputeTransformToOuterSurfaceWithOffset(moduleID,
                                                                                      pmtCouplerThickness / 2)};

            const auto shellTransform{ecal.ComputeTransformToOuterSurfaceWithOffset(moduleID,
                                                                                    pmtCouplerThickness + pmtLength / 2)};

            const auto physicalCoupler{Make<G4PVPlacement>(
                couplerTransform,
                logicCoupler,
                name + "PMTCoupler",
                Mother().LogicalVolume(),
                true,
                moduleID,
                checkOverlaps)};

            Make<G4PVPlacement>(
                shellTransform,
                logicPMTShell,
                name + "PMTShell",
                Mother().LogicalVolume(),
                true,
                moduleID,
                checkOverlaps);
            const auto ecalCrystal{FindSibling<ECALCrystal>()};
            if (ecalCrystal) {
                const auto couplerSurface{new G4OpticalSurface("coupler", unified, polished, dielectric_dielectric)};
                new G4LogicalBorderSurface{"couplerSurface",
                                           ecalCrystal->PhysicalVolume(name + fmt::format("Crystal_{}", moduleID)),
                                           physicalCoupler,
                                           couplerSurface};
                couplerSurface->SetMaterialPropertiesTable(couplerSurfacePropertiesTable);
            }
        }

        Make<G4PVPlacement>(
            G4Transform3D({}, {0, 0, pmtCathodeThickness / 2}),
            logicPMTVacuum,
            name + "PMTVacuum",
            logicPMTShell,
            true,
            typeID,
            checkOverlaps);
        Make<G4PVPlacement>(
            G4Transform3D({}, {0, 0, -(pmtLength - 2 * pmtWindowThickness - pmtCathodeThickness) / 2}),
            logicCathode,
            name + "PMCathode",
            logicPMTShell,
            true,
            typeID,
            checkOverlaps);

        const auto cathodeSurface{new G4OpticalSurface("Cathode", unified, polished, dielectric_metal)};
        new G4LogicalSkinSurface{"cathodeSkinSurface", logicCathode, cathodeSurface};
        cathodeSurface->SetMaterialPropertiesTable(cathodeSurfacePropertiesTable);
    }
}

} // namespace MACE::Detector::Definition
