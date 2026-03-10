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

#include "MACE/Detector/Definition/AcceleratorField.h++"
#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/MMSBeamPipe.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto AcceleratorField::Construct(G4bool checkOverlaps) -> void {
    const auto& accelerator{Description::Accelerator::Instance()};
    const auto& beamPipe{Description::MMSBeamPipe::Instance()};
    const auto name{accelerator.Name() + "Field"};

    const auto mother{Mother().LogicalVolume(beamPipe.Name() + "Vacuum")};
    const auto solid{Make<G4Tubs>(
        name,
        0,
        accelerator.FieldRadius(),
        accelerator.FullFieldLength() / 2,
        0,
        2_pi)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        mother->GetMaterial(),
        name)};
    Make<G4PVPlacement>( // clang-format off
        G4Transform3D{{}, {0, 0, (accelerator.DownstreamFieldLength() - accelerator.UpstreamFieldLength()) / 2}}, // clang-format on
        logic,
        name,
        mother,
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
