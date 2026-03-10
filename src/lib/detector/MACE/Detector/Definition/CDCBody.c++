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

#include "MACE/Detector/Definition/CDCBody.h++"
#include "MACE/Detector/Description/CDC.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Polycone.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"

#include "muc/math"

#include <array>
#include <cmath>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::Density;
using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto CDCBody::Construct(G4bool checkOverlaps) -> void {
    const auto& cdc{Description::CDC::Instance()};
    const auto name{cdc.Name() + "Body"};

    const auto endCapZExtension{cdc.EndCapThickness() * std::sqrt(1 + 1 / muc::pow(cdc.EndCapSlope(), 2))};

    const auto nist{G4NistManager::Instance()};
    { // End cap
        const auto zI{cdc.GasInnerLength() / 2 + endCapZExtension};
        const auto zO{cdc.GasOuterLength() / 2 + endCapZExtension};
        const auto rI{cdc.GasInnerRadius()};
        const auto rO{cdc.GasOuterRadius()};

        const std::array zPlaneList{-zO, -zI, zI, zO};
        const std::array rInnerList{rO, rI, rI, rO};
        const std::array rOuterList{rO, rO, rO, rO};

        const auto solidEndCap{Make<G4Polycone>(
            name,
            0,
            2_pi,
            zPlaneList.size(),
            zPlaneList.data(),
            rInnerList.data(),
            rOuterList.data())};
        const auto logicEndCap{Make<G4LogicalVolume>(
            solidEndCap,
            nist->FindOrBuildMaterial(cdc.EndCapMaterialName()),
            name)};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicEndCap,
            name,
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);
    }
    { // Inner shell Al Mylar foil
        const auto solidInnerShellAl{Make<G4Tubs>(
            name,
            cdc.GasInnerRadius() - cdc.InnerShellAlThickness(),
            cdc.GasInnerRadius(),
            cdc.GasInnerLength() / 2 + endCapZExtension,
            0,
            2_pi)};
        const auto logicInnerShellAl{Make<G4LogicalVolume>(
            solidInnerShellAl,
            nist->FindOrBuildMaterial("G4_Al"),
            name)};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicInnerShellAl,
            name,
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);

        const auto solidInnerShellMylar{Make<G4Tubs>(
            name,
            cdc.GasInnerRadius() - cdc.InnerShellAlThickness() - cdc.InnerShellMylarThickness(),
            cdc.GasInnerRadius() - cdc.InnerShellAlThickness(),
            cdc.GasInnerLength() / 2 + endCapZExtension,
            0,
            2_pi)};
        const auto logicInnerShellMylar{Make<G4LogicalVolume>(
            solidInnerShellMylar,
            nist->FindOrBuildMaterial("G4_MYLAR"),
            name)};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicInnerShellMylar,
            name,
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);
    }
    { // Outer shell CFRP
        const auto solidOuterShell{Make<G4Tubs>(
            name,
            cdc.GasOuterRadius(),
            cdc.GasOuterRadius() + cdc.OuterShellThickness(),
            cdc.GasOuterLength() / 2 + endCapZExtension,
            0,
            2_pi)};
        const auto logicOuterShell{Make<G4LogicalVolume>(
            solidOuterShell,
            nist->BuildMaterialWithNewDensity("CDCOuterShellCFRP", "G4_C", cdc.OuterShellCFRPDensity()),
            name)};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicOuterShell,
            name,
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);
    }
}

} // namespace MACE::Detector::Definition
