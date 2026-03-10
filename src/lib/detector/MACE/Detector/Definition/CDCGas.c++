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

#include "MACE/Detector/Definition/CDCGas.h++"
#include "MACE/Detector/Description/CDC.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4PVPlacement.hh"
#include "G4Polycone.hh"
#include "G4Transform3D.hh"

#include "gsl/gsl"

#include <array>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto CDCGas::Construct(G4bool checkOverlaps) -> void {
    const auto& cdc{Description::CDC::Instance()};
    const auto name{cdc.Name() + "Gas"};
    const auto zI{cdc.GasInnerLength() / 2};
    const auto zO{cdc.GasOuterLength() / 2};
    const auto rI{cdc.GasInnerRadius()};
    const auto rO{cdc.GasOuterRadius()};

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
        name,
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
