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

#pragma once

#include "MACE/Detector/Description/BeamDegrader.h++"
#include "MACE/Detector/Description/BeamMonitor.h++"
#include "MACE/Detector/Description/Target.h++"
#include "MACE/Detector/Description/World.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4VUserDetectorConstruction.hh"

#include <memory>
#include <tuple>

namespace Mustard::Detector::Definition {
class DefinitionBase;
} // namespace Mustard::Detector::Definition

namespace MACE::SimTarget::inline Action {

class DetectorConstruction final : public Mustard::Env::Memory::PassiveSingleton<DetectorConstruction>,
                                   public G4VUserDetectorConstruction {
public:
    DetectorConstruction();

    auto Construct() -> G4VPhysicalVolume* override;

    auto SetCheckOverlaps(G4bool checkOverlaps) -> void { fCheckOverlap = checkOverlaps; }

public:
    using ProminentDescription = std::tuple<Detector::Description::BeamDegrader,
                                            Detector::Description::BeamMonitor,
                                            Detector::Description::Target,
                                            Detector::Description::World>;

private:
    G4bool fCheckOverlap;

    std::unique_ptr<Mustard::Detector::Definition::DefinitionBase> fWorld;
};

} // namespace MACE::SimTarget::inline Action
