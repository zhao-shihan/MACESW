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

#include "MACE/Detector/Definition/CDCSuperLayer.h++"
#include "MACE/Detector/Description/CDC.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4PVPlacement.hh"
#include "G4Polycone.hh"
#include "G4Transform3D.hh"

#include "gsl/gsl"

#include "fmt/format.h"

#include <array>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto CDCSuperLayer::Construct(G4bool checkOverlaps) -> void {
    const auto& cdc{Description::CDC::Instance()};
    const auto name{cdc.Name() + "SuperLayer"};
    const auto layerConfig{cdc.LayerConfiguration()};

    for (gsl::index superLayerID{}; superLayerID < cdc.NSuperLayer(); ++superLayerID) {
        const auto& super{layerConfig[superLayerID]};

        const auto zI{super.innerHalfLength};
        const auto zO{super.outerHalfLength};
        const auto rI{super.innerRadius};
        const auto rO{super.outerRadius};

        const std::array zPlaneList{-zO, -zI, zI, zO};
        const std::array rInnerList{rO, rI, rI, rO};
        const std::array rOuterList{rO, rO, rO, rO};

        const auto solid{Make<G4Polycone>(
            name,
            0,
            2_pi,
            zPlaneList.size(),
            zPlaneList.data(),
            rInnerList.data(),
            rOuterList.data())};
        const auto logic{Make<G4LogicalVolume>(
            solid,
            cdc.GasMaterial(),
            name)};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logic,
            fmt::format("{}_{}", name, superLayerID),
            Mother().LogicalVolume(),
            false,
            superLayerID,
            checkOverlaps);
    }
}

} // namespace MACE::Detector::Definition
