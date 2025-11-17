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

#include "MACE/SimTarget/Action/ActionInitialization.h++"
#include "MACE/SimTarget/Action/DetectorConstruction.h++"
#include "MACE/SimTarget/Analysis.h++"
#include "MACE/SimTarget/RunManager.h++"
#include "MACE/Simulation/Physics/StandardPhysicsList.h++"

#include "Mustard/Env/BasicEnv.h++"

#include "muc/utility"

namespace MACE::SimTarget {

RunManager::RunManager() :
    MPIRunManager{},
    fAnalysis{std::make_unique_for_overwrite<Analysis>()} {

    SetUserInitialization(new StandardPhysicsList);

    const auto detectorConstruction{new DetectorConstruction};
    detectorConstruction->SetCheckOverlaps(Mustard::Env::VerboseLevelReach<'I'>());
    SetUserInitialization(detectorConstruction);

    SetUserInitialization(new ActionInitialization);
}

RunManager::~RunManager() = default;

} // namespace MACE::SimTarget
