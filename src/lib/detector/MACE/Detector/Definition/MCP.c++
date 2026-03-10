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

#include "MACE/Detector/Definition/MCP.h++"
#include "MACE/Detector/Description/MCP.h++"
#include "MACE/Detector/Description/MCPChamber.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit;

auto MCP::Construct(G4bool checkOverlaps) -> void {
    const auto& mcp{Description::MCP::Instance()};

    const auto nist{G4NistManager::Instance()};
    const auto mother{Mother().LogicalVolume(Description::MCPChamber::Instance().Name() + "Vacuum")};

    { // MCP
        const auto solid{Make<G4Tubs>(
            mcp.Name(),
            0,
            mcp.Diameter() / 2,
            mcp.Thickness() / 2,
            0,
            2_pi)};
        const auto logic{Make<G4LogicalVolume>(
            solid,
            nist->BuildMaterialWithNewDensity("MCP", "G4_GLASS_LEAD", 3.73_g_cm3),
            mcp.Name())};
        Make<G4PVPlacement>( // clang-format off
        G4Transform3D{{}, {0, 0, mcp.Thickness() / 2}}, // clang-format on
            logic,
            mcp.Name(),
            mother,
            false,
            0,
            checkOverlaps);
    }
    { // Anode
        const auto name{mcp.Name() + "Anode"};
        const auto solid{Make<G4Tubs>(
            name,
            0,
            mcp.Diameter() / 2,
            mcp.AnodeThickness() / 2,
            0,
            2_pi)};
        const auto logic{Make<G4LogicalVolume>(
            solid,
            nist->FindOrBuildMaterial("G4_Al"),
            name)};
        Make<G4PVPlacement>( // clang-format off
        G4Transform3D{{}, {0, 0, mcp.Thickness() + mcp.AnodeDistance()}}, // clang-format on
            logic,
            name,
            mother,
            false,
            0,
            checkOverlaps);
    }
}

} // namespace MACE::Detector::Definition
