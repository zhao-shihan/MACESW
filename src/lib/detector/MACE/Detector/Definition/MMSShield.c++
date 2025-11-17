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

#include "MACE/Detector/Definition/MMSShield.h++"
#include "MACE/Detector/Description/MMSShield.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"

#include <cmath>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit;

auto MMSShield::Construct(G4bool checkOverlaps) -> void {
    const auto& shield{Description::MMSShield::Instance()};

    const auto material{G4NistManager::Instance()->FindOrBuildMaterial(shield.MaterialName())};

    const auto solidBody{Make<G4Tubs>(
        shield.Name(),
        shield.InnerRadius(),
        shield.InnerRadius() + shield.Thickness(),
        shield.InnerLength() / 2,
        0,
        2_pi)};
    const auto logicBody{Make<G4LogicalVolume>(
        solidBody,
        material,
        shield.Name())};
    Make<G4PVPlacement>(
        G4Transform3D{},
        logicBody,
        shield.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);

    const auto zCap{shield.InnerLength() / 2 + shield.Thickness() / 2};
    const auto solidCap{Make<G4SubtractionSolid>(
        shield.Name(),
        Make<G4Tubs>(
            "_temp",
            0,
            shield.InnerRadius() + shield.Thickness(),
            shield.Thickness() / 2,
            0,
            2_pi),
        Make<G4Tubs>(
            "_temp",
            0,
            shield.WindowRadius(),
            shield.Thickness(),
            0,
            2_pi),
        nullptr,
        G4ThreeVector{zCap * std::tan(shield.BeamSlantAngle()), 0, 0})};
    const auto logicCap{Make<G4LogicalVolume>(
        solidCap,
        material,
        shield.Name())};
    Make<G4PVPlacement>( // clang-format off
        G4Transform3D{{}, {0, 0, -zCap}}, // clang-format on
        logicCap,
        shield.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
    Make<G4PVPlacement>( // clang-format off
        G4Transform3D{{}, {0, 0, zCap}}, // clang-format on
        logicCap,
        shield.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
