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

#include "MACE/Detector/Definition/SolenoidFieldT2.h++"
#include "MACE/Detector/Description/Solenoid.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Torus.hh"
#include "G4Transform3D.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto SolenoidFieldT2::Construct(G4bool checkOverlaps) -> void {
    const auto& solenoid{Description::Solenoid::Instance()};
    const auto name{solenoid.Name() + "FieldT2"};

    const auto mother{Mother().LogicalVolume()};
    const auto solid{Make<G4Torus>(
        name,
        0,
        solenoid.FieldRadius(),
        solenoid.T2Radius(),
        0,
        0.5_pi)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        mother->GetMaterial(),
        name)};
    Make<G4PVPlacement>(
        G4Translate3D{Mustard::VectorCast<G4ThreeVector>(solenoid.T2Center())} * G4RotateX3D{-0.5_pi},
        logic,
        name,
        mother,
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
