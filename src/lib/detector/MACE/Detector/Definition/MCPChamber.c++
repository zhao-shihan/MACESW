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

#include "MACE/Detector/Definition/MCPChamber.h++"
#include "MACE/Detector/Description/ECALField.h++"
#include "MACE/Detector/Description/MCPChamber.h++"
#include "MACE/Detector/Description/SolenoidBeamPipe.h++"
#include "MACE/Detector/Description/Vacuum.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4Orb.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"

#include "muc/math"

#include <cmath>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;
using namespace Mustard::LiteralUnit::Temperature;

auto MCPChamber::Construct(G4bool checkOverlaps) -> void {
    const auto& mcpChamber{Description::MCPChamber::Instance()};
    const auto& beamPipe{Description::SolenoidBeamPipe::Instance()};
    const auto& ecalField{Description::ECALField::Instance()};
    const auto& vacuum{Description::Vacuum::Instance()};

    const auto nist{G4NistManager::Instance()};
    auto vacuumMaterial{nist->FindMaterial(vacuum.Name())};
    if (not vacuumMaterial) {
        vacuumMaterial = nist->BuildMaterialWithNewDensity(vacuum.Name(), "G4_AIR", vacuum.Density(), 293.15_K, vacuum.Pressure());
    }

    const auto zWeld{std::sqrt(muc::pow(mcpChamber.InnerRadius(), 2) - muc::pow(beamPipe.InnerRadius(), 2))};

    { // Spherical chamber
        const auto substractor{Make<G4Box>(
            "_temp",
            mcpChamber.InnerRadius(),
            mcpChamber.InnerRadius(),
            mcpChamber.InnerRadius())};

        const auto solidChamber{Make<G4SubtractionSolid>(
            mcpChamber.Name(),
            Make<G4Orb>(
                "_temp",
                mcpChamber.InnerRadius() + mcpChamber.Thickness()),
            substractor,
            nullptr,
            G4ThreeVector{0, 0, -mcpChamber.InnerRadius() - zWeld})};
        const auto logicChamber{Make<G4LogicalVolume>(
            solidChamber,
            nist->FindOrBuildMaterial("G4_Be"),
            mcpChamber.Name())};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicChamber,
            mcpChamber.Name(),
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);

        const auto vacuumName{mcpChamber.Name() + "Vacuum"};
        const auto solidVacuum{Make<G4SubtractionSolid>(
            vacuumName,
            Make<G4Orb>(
                "_temp",
                mcpChamber.InnerRadius()),
            substractor,
            nullptr,
            G4ThreeVector{0, 0, -mcpChamber.InnerRadius() - zWeld})};
        const auto logicVacuum{Make<G4LogicalVolume>(
            solidVacuum,
            vacuumMaterial,
            vacuumName)};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicVacuum,
            vacuumName,
            logicChamber,
            false,
            0,
            checkOverlaps);
    }

    { // Pipe
        const auto pipeName{mcpChamber.Name() + "Pipe"};
        const auto halfLength{(ecalField.Length() / 2 - zWeld) / 2};
        const auto solidPipe{Make<G4Tubs>(
            pipeName,
            0,
            beamPipe.InnerRadius() + beamPipe.Thickness(),
            halfLength,
            0,
            2_pi)};
        const auto logicPipe{Make<G4LogicalVolume>(
            solidPipe,
            nist->FindOrBuildMaterial("G4_Al"),
            pipeName)};
        Make<G4PVPlacement>(
            G4TranslateZ3D{-halfLength - zWeld},
            logicPipe,
            pipeName,
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);

        const auto vacuumName{pipeName + "Vacuum"};
        const auto solidVacuum{Make<G4Tubs>(
            vacuumName,
            0,
            beamPipe.InnerRadius(),
            halfLength,
            0,
            2_pi)};
        const auto logicVacuum{Make<G4LogicalVolume>(
            solidVacuum,
            vacuumMaterial,
            vacuumName)};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicVacuum,
            vacuumName,
            logicPipe,
            false,
            0,
            checkOverlaps);
    }
}

} // namespace MACE::Detector::Definition
