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

#include "MACE/SimECAL/Detector/ECALTunnel.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"

namespace MACE::SimECAL::Detector {

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;

auto ECALTunnel::Construct(G4bool checkOverlaps) -> void {
    const auto width = 50_m;
    const auto thickness = 18_m;
    const auto radius = 1_m;
    const auto length = 10_m;

    const auto nistManager = G4NistManager::Instance();
    const auto cement = nistManager->FindOrBuildMaterial("G4_CONCRETE");

    const auto box1 = Make<G4Box>(
        "tunnelMaker",
        width / 2,
        thickness,
        width / 2);

    const auto box2 = Make<G4Box>(
        "halfBox",
        26_m,
        8.5_m,
        26_m);

    const auto cylinder = Make<G4Tubs>(
        "tunnel",
        0,
        radius,
        length / 2,
        0,
        2 * pi);

    const auto tunnelMakerSolid = Make<G4SubtractionSolid>("Box - Cylinder", box1, cylinder);
    const auto solid = Make<G4SubtractionSolid>(
        "Box - halfBox",
        tunnelMakerSolid,
        box2,
        G4Transform3D(
            G4RotationMatrix(),
            G4ThreeVector(0, -9.6_m, 0)));
    const auto logic = Make<G4LogicalVolume>(
        solid,
        cement,
        "ECALTunnel");
    Make<G4PVPlacement>(
        G4Transform3D(),
        logic,
        "ECALTunnel",
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::SimECAL::Detector
