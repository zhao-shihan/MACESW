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

#include "MACE/Detector/Description/Solenoid.h++"
#include "MACE/Detector/Description/SolenoidBeamPipe.h++"
#include "MACE/SimPTS/Detector/Definition/VirtualDetectorB.h++"
#include "MACE/SimPTS/Detector/Description/VirtualDetectorB.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"

namespace MACE::SimPTS::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto VirtualDetectorB::Construct(bool checkOverlaps) -> void {
    const auto& solenoid{MACE::Detector::Description::Solenoid::Instance()};
    const auto& solenoidBeamPipe{MACE::Detector::Description::SolenoidBeamPipe::Instance()};
    const auto& virtualDetectorB{Description::VirtualDetectorB::Instance()};

    const auto mother{Mother().LogicalVolume("SolenoidBeamPipeS2Vacuum")};
    const auto solid{Make<G4Tubs>(
        virtualDetectorB.Name(),
        0,
        solenoidBeamPipe.InnerRadius(),
        virtualDetectorB.Thickness() / 2,
        0,
        2_pi)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        mother->GetMaterial(),
        virtualDetectorB.Name())};
    Make<G4PVPlacement>(
        G4TranslateZ3D{-solenoid.S2Length() / 2 + virtualDetectorB.Thickness() / 2},
        logic,
        virtualDetectorB.Name(),
        mother,
        false,
        1,
        checkOverlaps);
}

} // namespace MACE::SimPTS::Detector::Definition
