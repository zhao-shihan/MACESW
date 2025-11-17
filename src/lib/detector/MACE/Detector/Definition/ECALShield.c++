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

#include "MACE/Detector/Definition/ECALShield.h++"
#include "MACE/Detector/Description/ECALField.h++"
#include "MACE/Detector/Description/ECALShield.h++"
#include "MACE/Detector/Description/Solenoid.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit;

auto ECALShield::Construct(G4bool checkOverlaps) -> void {
    const auto& shield{Description::ECALShield::Instance()};
    const auto& ecalField{Description::ECALField::Instance()};
    const auto& solenoid{Description::Solenoid::Instance()};

    const auto x0{Mustard::VectorCast<G4ThreeVector>(ecalField.Center())};
    const auto pb{G4NistManager::Instance()->FindOrBuildMaterial(shield.MaterialName())};

    const auto solidBody{Make<G4Tubs>(
        shield.Name(),
        shield.InnerRadius(),
        shield.InnerRadius() + shield.Thickness(),
        shield.InnerLength() / 2,
        0,
        2_pi)};
    const auto logicalBody{Make<G4LogicalVolume>(
        solidBody,
        pb,
        shield.Name())};
    Make<G4PVPlacement>(
        G4Transform3D{{}, x0},
        logicalBody,
        shield.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);

    const auto solidCap{Make<G4Tubs>(
        shield.Name(),
        solenoid.FieldRadius() + shield.GapAroundWindow(),
        shield.InnerRadius() + shield.Thickness(),
        shield.Thickness() / 2,
        0,
        2_pi)};
    const auto logicalCap{Make<G4LogicalVolume>(
        solidCap,
        pb,
        shield.Name())};
    const G4ThreeVector deltaXEnd{0, 0, shield.InnerLength() / 2 + shield.Thickness() / 2};
    Make<G4PVPlacement>(
        G4Transform3D{{}, x0 - deltaXEnd},
        logicalCap,
        shield.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
    Make<G4PVPlacement>(
        G4Transform3D{{}, x0 + deltaXEnd},
        logicalCap,
        shield.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
