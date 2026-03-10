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

#include "MACE/Detector/Definition/MMSBeamPipe.h++"
#include "MACE/Detector/Description/MMSBeamPipe.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/Vacuum.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;
using namespace Mustard::LiteralUnit::Temperature;

auto MMSBeamPipe::Construct(G4bool checkOverlaps) -> void {
    const auto& beamPipe{Description::MMSBeamPipe::Instance()};
    const auto& mmsField{Description::MMSField::Instance()};
    const auto& vacuum{Description::Vacuum::Instance()};

    const auto nist{G4NistManager::Instance()};
    auto vacuumMaterial{nist->FindMaterial(vacuum.Name())};
    if (not vacuumMaterial) {
        vacuumMaterial = nist->BuildMaterialWithNewDensity(vacuum.Name(), "G4_AIR", vacuum.Density(), 293.15_K, vacuum.Pressure());
    }

    { // Beryllium pipe section
        const auto solidPipe{Make<G4Tubs>(
            beamPipe.Name(),
            0,
            beamPipe.InnerRadius() + beamPipe.BerylliumThickness(),
            beamPipe.BerylliumLength() / 2,
            0,
            2_pi)};
        const auto logicPipe{Make<G4LogicalVolume>(
            solidPipe,
            G4NistManager::Instance()->FindOrBuildMaterial("G4_Be"),
            beamPipe.Name())};
        Make<G4PVPlacement>(
            G4Transform3D{},
            logicPipe,
            beamPipe.Name(),
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);

        const auto vacuumName{beamPipe.Name() + "Vacuum"};
        const auto solidVacuum{Make<G4Tubs>(
            vacuumName,
            0,
            beamPipe.InnerRadius(),
            solidPipe->GetZHalfLength(),
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
    { // Aluminium pipe section
        const auto solidPipe{Make<G4Tubs>(
            beamPipe.Name(),
            0,
            beamPipe.InnerRadius() + beamPipe.AluminiumThickness(),
            (mmsField.Length() - beamPipe.BerylliumLength()) / 4,
            0,
            2_pi)};
        const auto logicPipe{Make<G4LogicalVolume>(
            solidPipe,
            G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"),
            beamPipe.Name())};
        const auto z0{(mmsField.Length() + beamPipe.BerylliumLength()) / 4};
        Make<G4PVPlacement>( // clang-format off
            G4Transform3D{{}, {0, 0, -z0}}, // clang-format on
            logicPipe,
            beamPipe.Name(),
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);
        Make<G4PVPlacement>( // clang-format off
            G4Transform3D{{}, {0, 0, z0}}, // clang-format on
            logicPipe,
            beamPipe.Name(),
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);

        const auto vacuumName{beamPipe.Name() + "Vacuum"};
        const auto solidVacuum{Make<G4Tubs>(
            vacuumName,
            0,
            beamPipe.InnerRadius(),
            solidPipe->GetZHalfLength(),
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
