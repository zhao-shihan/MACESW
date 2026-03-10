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

#include "MACE/Detector/Definition/MMSField.h++"
#include "MACE/Detector/Description/MMSField.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto MMSField::Construct(G4bool checkOverlaps) -> void {
    const auto& mmsField{Description::MMSField::Instance()};

    const auto mother{Mother().LogicalVolume()};
    const auto solid{Make<G4Tubs>(
        mmsField.Name(),
        0,
        mmsField.Radius(),
        mmsField.Length() / 2,
        0,
        2_pi)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        mother->GetMaterial(),
        mmsField.Name())};
    Make<G4PVPlacement>(
        G4Transform3D(),
        logic,
        mmsField.Name(),
        mother,
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
