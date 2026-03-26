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
#include "MACE/Detector/Description/Vacuum.h++"

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
    const auto& ecal{Description::ECAL::Instance()};
    const auto name{ecal.Name()};

    const auto mppcNPixelRowSet{ecal.MPPCNPixelRowSet()};
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
    const auto& moduleList{ecal.Array().moduleList};
    const auto& moduleSelection{ecal.ModuleSelection()};
    std::map<int, std::vector<int>> idListOfType;
    for (auto&& module : std::as_const(moduleList)) {
        idListOfType[module.typeID].emplace_back(module.moduleID);
    }

    std::vector<int> chosenType;
    chosenType.reserve(moduleSelection.size());
    for (auto&& chosen : moduleSelection) {
        chosenType.emplace_back(moduleList.at(chosen).typeID);
    }
    for (auto&& [type, moduleIDList] : idListOfType) { // loop over type(10 total)
        if (not chosenType.empty() and std::ranges::find(chosenType, type) == chosenType.end()) {
            continue;
        }

        const auto mppcNPixelRows{mppcNPixelRowSet.at(type)};
        const auto mppcPixelSize{mppcPixelSizeSet.at(type)};
        const auto mppcWidth{mppcNPixelRows * (mppcPixelSize + mppcPitch) + mppcPitch};

        const auto solidCoupler{Make<G4Box>("temp", mppcWidth / 2, mppcWidth / 2, mppcCouplerThickness / 2)};
        const auto logicCoupler{Make<G4LogicalVolume>(solidCoupler, siliconeGrease, name + "MPPCCoupler")};

        const auto solidWindow{Make<G4Box>("temp", mppcWidth / 2, mppcWidth / 2, mppcWindowThickness / 2)};
        const auto logicWindow{Make<G4LogicalVolume>(solidWindow, epoxy, name + "MPPCWindow")};

        const auto solidPixel{Make<G4Box>("temp", mppcPixelSize / 2, mppcPixelSize / 2, mppcThickness / 2)};
        const auto logicPixel{Make<G4LogicalVolume>(solidPixel, silicon, name + "PMCathode")};
        for (int copyNo{}; copyNo < mppcNPixelRows * mppcNPixelRows; copyNo++) {
            const auto rowNum{copyNo / mppcNPixelRows};
            const auto colNum{copyNo % mppcNPixelRows};
            const auto xOffSet{(2 * rowNum + 1 - mppcNPixelRows) * ((mppcPixelSize + mppcPitch) / 2)};
            const auto yOffset{(2 * colNum + 1 - mppcNPixelRows) * ((mppcPixelSize + mppcPitch) / 2)};
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
                const auto couplerSurface{new G4OpticalSurface("Coupler", unified, polished, dielectric_dielectric)};
                new G4LogicalBorderSurface{"CouplerSurface",
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
        new G4LogicalSkinSurface{"CathodeSkinSurface", logicPixel, cathodeSurface};
        cathodeSurface->SetMaterialPropertiesTable(cathodeSurfacePropertiesTable);
    }
}

} // namespace MACE::Detector::Definition
