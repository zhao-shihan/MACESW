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
#include "Mustard/IO/Print.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "G4Box.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4QuadrangularFacet.hh"
#include "G4TessellatedSolid.hh"

#include "fmt/format.h"

#include <algorithm>
#include <map>
#include <utility>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;

auto ECALCrystal::Construct(G4bool checkOverlaps) -> void {
    const auto& ecal{Description::ECAL::Instance()};
    const auto name{ecal.Name()};

    const auto innerRadius{ecal.InnerRadius()};
    const auto crystalHypotenuse{ecal.CrystalHypotenuse()};

    const auto& vertex{ecal.Array().vertexList};
    const auto& moduleList{ecal.Array().moduleList};

    const auto& moduleSelection{ecal.ModuleSelection()};

    /////////////////////////////////////////////
    // Construct Element and Material
    /////////////////////////////////////////////

    const auto nist{G4NistManager::Instance()};
    const auto cesiumIodide{nist->FindOrBuildMaterial("G4_CESIUM_IODIDE")};

    //////////////////////////////////////////////////
    // Construct Material Optical Properties Tables
    //////////////////////////////////////////////////

    const auto [minPhotonEnergy, maxPhotonEnergy]{std::ranges::minmax(ecal.ScintillationEnergyBin())};
    const auto crystalPropertiesTable{new G4MaterialPropertiesTable};
    crystalPropertiesTable->AddProperty("ABSLENGTH", {minPhotonEnergy, maxPhotonEnergy}, {40_cm, 40_cm});
    crystalPropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1", ecal.ScintillationEnergyBin(), ecal.ScintillationComponent1());
    crystalPropertiesTable->AddConstProperty("SCINTILLATIONYIELD", ecal.ScintillationYield());
    crystalPropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", ecal.ScintillationTimeConstant1());
    crystalPropertiesTable->AddConstProperty("RESOLUTIONSCALE", ecal.ResolutionScale());
    crystalPropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, {1.95, 1.95});
    cesiumIodide->SetMaterialPropertiesTable(crystalPropertiesTable);

    if (Mustard::Env::VerboseLevelReach<'V'>()) {
        crystalPropertiesTable->DumpTable();
    }

    const auto reflectorSurfacePropertiesTable{new G4MaterialPropertiesTable};
    reflectorSurfacePropertiesTable->AddProperty("REFLECTIVITY", {minPhotonEnergy, maxPhotonEnergy}, {0.99, 0.99});

    const auto couplerSurfacePropertiesTable{new G4MaterialPropertiesTable};
    couplerSurfacePropertiesTable->AddProperty("TRANSMITTANCE", {minPhotonEnergy, maxPhotonEnergy}, {1, 1});

    const auto coatingSurfacePropertiesTable{new G4MaterialPropertiesTable};
    coatingSurfacePropertiesTable->AddProperty("REFLECTIVITY", {minPhotonEnergy, maxPhotonEnergy}, {0, 0});

    /////////////////////////////////////////////
    // Construct Volumes
    /////////////////////////////////////////////
    for (auto&& [moduleID, _2, _3, centroid, normal, vertexIndex] : std::as_const(moduleList)) {
        // loop over all ECAL face
        // centroid here refer to the face 'center' of normalized ball

        if ((not moduleSelection.empty()) and std::ranges::find(moduleSelection, moduleID) == moduleSelection.end()) {
            continue;
        }
        if (not moduleSelection.empty() and
            moduleID == moduleSelection.front() and Mustard::Env::VerboseLevelReach<'I'>()) {
            Mustard::MasterPrintLn("Centroid of Selected Seed Module: ");
            Mustard::MasterPrintLn("{} {} {}", centroid.x(), centroid.y(), centroid.z());
            Mustard::MasterPrintLn<'I'>("======================================================\n");
        }

        const auto solidCrystal{
            [&, &centroid = centroid, &vertexIndex = vertexIndex](const auto& name) {
                const auto computeIntersection{[](G4ThreeVector vertexPlane, G4ThreeVector normPlane, G4ThreeVector vertexLine, G4ThreeVector direcLine) {
                    double lambda{normPlane.dot(vertexPlane - vertexLine) / normPlane.dot(direcLine)};
                    return vertexLine + direcLine * lambda;
                }};

                const auto outerRadius{innerRadius + crystalHypotenuse};
                const auto outerCentroid{outerRadius * centroid};
                std::vector<G4ThreeVector> outerVertexes(vertexIndex.size());
                // outer face cut vertex lines
                std::ranges::transform(vertexIndex, outerVertexes.begin(),
                                       [&](const auto& i) { return computeIntersection(outerCentroid, normal, vertex[i], vertex[i]); });
                // consider package thickness
                std::ranges::transform(outerVertexes, outerVertexes.begin(),
                                       [&](const auto& aVertex) { return outerCentroid + (aVertex - outerCentroid).unit() * ((aVertex - outerCentroid).mag() - ecal.CrystalPackageThickness()); });
                // inner face scaled from outer face
                const auto innerCentroid{innerRadius * centroid};
                if (Mustard::Env::VerboseLevelReach<'I'>()) {
                    Mustard::MasterPrintLn("Module {} {} {} {}", moduleID, innerCentroid.x(), innerCentroid.y(), innerCentroid.z());
                }
                const auto innerVertexScaleFactor{innerRadius / outerRadius};
                std::vector<G4ThreeVector> innerVertexes(vertexIndex.size());
                std::ranges::transform(outerVertexes, innerVertexes.begin(),
                                       [&](const auto& anOuterVertex) { return anOuterVertex * innerVertexScaleFactor; });

                // clang-format off
                /* Pentagon:
                 *      0
                 *   /  |  \
                 *  1 - C - 4
                 *  \ / \  /
                 *   2 - 3
                 *
                 * Hexagon:
                 *     0 - 5
                 *   / \  / \
                 *  1 - C - 4
                 *  \ /  \ /
                 *   2 - 3
                 */
                // clang-format on

                const auto solid{Make<G4TessellatedSolid>(name)};
                // inner surface
                solid->AddFacet(new G4TriangularFacet{innerCentroid,
                                                      innerVertexes[0],
                                                      innerVertexes[vertexIndex.size() - 1],
                                                      G4FacetVertexType::ABSOLUTE});
                for (auto i{std::ssize(vertexIndex) - 1}; i > 0; --i) {
                    solid->AddFacet(new G4TriangularFacet{innerCentroid,
                                                          innerVertexes[i],
                                                          innerVertexes[i - 1],
                                                          G4FacetVertexType::ABSOLUTE});
                }
                // side surface
                for (int i{}; i < std::ssize(vertexIndex) - 1; ++i) {
                    solid->AddFacet(new G4QuadrangularFacet{innerVertexes[i],
                                                            innerVertexes[i + 1],
                                                            outerVertexes[i + 1],
                                                            outerVertexes[i],
                                                            G4FacetVertexType::ABSOLUTE});
                }
                solid->AddFacet(new G4QuadrangularFacet{innerVertexes[vertexIndex.size() - 1],
                                                        innerVertexes[0],
                                                        outerVertexes[0],
                                                        outerVertexes[vertexIndex.size() - 1],
                                                        G4FacetVertexType::ABSOLUTE});
                // outer surface
                for (int i{}; i < std::ssize(vertexIndex) - 1; ++i) {
                    solid->AddFacet(new G4TriangularFacet{outerCentroid,
                                                          outerVertexes[i],
                                                          outerVertexes[i + 1],
                                                          G4FacetVertexType::ABSOLUTE});
                }
                solid->AddFacet(new G4TriangularFacet{outerCentroid,
                                                      outerVertexes[vertexIndex.size() - 1],
                                                      outerVertexes[0],
                                                      G4FacetVertexType::ABSOLUTE});
                solid->SetSolidClosed(true);
                return solid;
            }};

        // Crystal

        const auto logicCrystal{
            Make<G4LogicalVolume>(
                solidCrystal(fmt::format("{}Crystal_{}", name, moduleID)),
                cesiumIodide,
                name + "Crystal")};
        const auto physicalCrystal{
            Make<G4PVPlacement>(
                G4Transform3D{},
                logicCrystal,
                fmt::format("{}Crystal_{}", name, moduleID),
                Mother().LogicalVolume(),
                true,
                moduleID,
                checkOverlaps)};

        /////////////////////////////////////////////
        // Construct Optical Surface
        /////////////////////////////////////////////

        const auto reflectorSurface{new G4OpticalSurface("Reflector", unified, polished, dielectric_metal)};
        new G4LogicalSkinSurface{"ReflectorSurface", logicCrystal, reflectorSurface};
        reflectorSurface->SetMaterialPropertiesTable(reflectorSurfacePropertiesTable);

        const auto coatingSurface{new G4OpticalSurface("Coating", unified, polished, dielectric_metal)};
        new G4LogicalBorderSurface{"CoatingSurface", Mother().PhysicalVolume(), physicalCrystal, coatingSurface};
        coatingSurface->SetMaterialPropertiesTable(coatingSurfacePropertiesTable);

        const auto ecalPMCoupler{FindSibling<ECALPhotoSensor>()};
        if (ecalPMCoupler) {
            const auto couplerSurface{new G4OpticalSurface("Coupler", unified, polished, dielectric_dielectric)};
            new G4LogicalBorderSurface{"CouplerSurface",
                                       physicalCrystal,
                                       ecalPMCoupler->PhysicalVolume(name + "PMCoupler", moduleID),
                                       couplerSurface};
            couplerSurface->SetMaterialPropertiesTable(couplerSurfacePropertiesTable);
        }
    }
}

} // namespace MACE::Detector::Definition
