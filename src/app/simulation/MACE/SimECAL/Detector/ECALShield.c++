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

#include "MACE/Detector/Description/ECALShield.h++"
#include "MACE/Detector/Description/Solenoid.h++"
#include "MACE/SimECAL/Detector/ECALShield.h++"

#include "Mustard/Detector/Description/DescriptionIO.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"

namespace MACE::SimECAL::Detector {

using namespace Mustard::LiteralUnit;

auto ECALShield::Construct(G4bool checkOverlaps) -> void {
    const auto& shield{MACE::Detector::Description::ECALShield::Instance()};
    const auto& solenoid{MACE::Detector::Description::Solenoid::Instance()};

    const auto body{Make<G4Tubs>(
        "_temp",
        shield.InnerRadius(),
        shield.InnerRadius() + shield.Thickness(),
        shield.InnerLength() / 2,
        0,
        2_pi)};
    const auto cap{Make<G4Tubs>(
        "_temp",
        solenoid.FieldRadius() + shield.GapAroundWindow(),
        shield.InnerRadius() + shield.Thickness(),
        shield.Thickness() / 2,
        0,
        2_pi)}; // clang-format off
    const auto temp{Make<G4UnionSolid>(
        "_temp",
        body,
        cap,
        G4Transform3D{{}, {0, 0, -shield.InnerLength() / 2 - shield.Thickness() / 2}})};
    const auto solid{Make<G4UnionSolid>(
        shield.Name(),
        temp,
        cap,
        G4Transform3D{{}, {0, 0, shield.InnerLength() / 2 + shield.Thickness() / 2}})}; // clang-format on
    const auto logic{Make<G4LogicalVolume>(
        solid,
        G4NistManager::Instance()->FindOrBuildMaterial(shield.MaterialName()),
        shield.Name())};
    Make<G4PVPlacement>(
        G4Transform3D(),
        logic,
        shield.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::SimECAL::Detector
