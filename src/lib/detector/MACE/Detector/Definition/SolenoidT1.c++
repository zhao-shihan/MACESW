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

#include "MACE/Detector/Definition/SolenoidT1.h++"
#include "MACE/Detector/Description/Solenoid.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

#include "muc/math"

#include <cmath>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto SolenoidT1::Construct(G4bool checkOverlaps) -> void {
    const auto& solenoid{Description::Solenoid::Instance()};
    const auto name{solenoid.Name() + "T1"};

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

    const auto referenceAngularSpacing{2 * std::asin(solenoid.ReferenceCoilSpacing() / (2 * solenoid.T1Radius()))};
    const auto coilAngularThickness{2 * std::asin(solenoid.CoilThickness() / (2 * solenoid.T1Radius()))};
    const auto nCoil{muc::lltrunc(0.5_pi / (coilAngularThickness + referenceAngularSpacing))};
    const auto angularSpacing{0.5_pi / nCoil - referenceAngularSpacing};
    const auto phi0{angularSpacing / 2 + coilAngularThickness / 2}; // clang-format off
    const auto basicTransform{G4TranslateX3D{-solenoid.T1Radius()} *
                              G4Transform3D{Mother().PhysicalVolume()->GetRotation()->inverse(), {}}}; // clang-format on
    for (int k{}; k < nCoil; ++k) {
        Make<G4PVPlacement>(
            G4RotateZ3D{phi0 + k * (angularSpacing + coilAngularThickness)} * basicTransform,
            logic,
            name,
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);
    }
}

} // namespace MACE::Detector::Definition
