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

#include "MACE/Detector/Definition/CDCSenseLayer.h++"
#include "MACE/Detector/Description/CDC.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Hype.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

#include "gsl/gsl"

#include "fmt/format.h"

#include <utility>

namespace MACE::Detector::Definition {

auto CDCSenseLayer::Construct(G4bool checkOverlaps) -> void {
    using namespace Mustard::LiteralUnit::MathConstantSuffix;

    const auto& cdc{Description::CDC::Instance()};
    const auto name{cdc.Name() + "SenseLayer"};

    for (auto&& super : std::as_const(cdc.LayerConfiguration())) {
        for (auto&& sense : super.sense) {
            const auto layerInnerRadius{sense.innerRadius};
            const auto layerOuterRadius{&sense != &super.sense.back() ?
                                            sense.outerRadius :
                                            sense.outerRadius + cdc.FieldWireDiameter()};
            const auto solid{
                super.isAxial ?
                    static_cast<G4VSolid*>(Make<G4Tubs>(
                        name,
                        layerInnerRadius,
                        layerOuterRadius,
                        sense.halfLength,
                        0,
                        2_pi)) :
                    static_cast<G4VSolid*>(Make<G4Hype>(
                        name,
                        layerInnerRadius,
                        layerOuterRadius,
                        sense.StereoZenithAngle(layerInnerRadius),
                        sense.StereoZenithAngle(layerOuterRadius),
                        sense.halfLength))};
            const auto logic{Make<G4LogicalVolume>(
                solid,
                cdc.GasMaterial(),
                name)};
            Make<G4PVPlacement>(
                G4Transform3D{},
                logic,
                fmt::format("{}_{}", name, sense.senseLayerID),
                Mother().LogicalVolume(super.superLayerID),
                false,
                sense.senseLayerID,
                checkOverlaps);
        }
    }
}

} // namespace MACE::Detector::Definition
