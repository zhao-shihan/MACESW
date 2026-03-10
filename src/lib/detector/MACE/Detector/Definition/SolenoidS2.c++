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

#include "MACE/Detector/Definition/SolenoidS2.h++"
#include "MACE/Detector/Description/Solenoid.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

#include "muc/math"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto SolenoidS2::Construct(G4bool checkOverlaps) -> void {
    const auto& solenoid{Description::Solenoid::Instance()};
    const auto name{solenoid.Name() + "S2"};

    const auto solid{Make<G4Tubs>(
        name,
        solenoid.InnerRadius(),
        solenoid.OuterRadius(),
        solenoid.CoilThickness() / 2,
        0,
        2_pi)};

    const auto logic{Make<G4LogicalVolume>(
        solid,
        G4NistManager::Instance()->FindOrBuildMaterial(solenoid.MaterialName()),
        name)};

    const auto nCoil{muc::lltrunc(solenoid.S2Length() / (solenoid.CoilThickness() + solenoid.ReferenceCoilSpacing()))};
    const auto spacing{solenoid.S2Length() / nCoil - solenoid.ReferenceCoilSpacing()};
    const auto z0{-solenoid.S2Length() / 2 + spacing / 2 + solenoid.CoilThickness() / 2};
    for (int k{}; k < nCoil; ++k) {
        Make<G4PVPlacement>(
            G4TranslateZ3D{z0 + k * (spacing + solenoid.CoilThickness())},
            logic,
            name,
            Mother().LogicalVolume(),
            false,
            k,
            checkOverlaps);
    }
}

} // namespace MACE::Detector::Definition
