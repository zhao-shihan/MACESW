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

#include "MACE/Detector/Description/Target.h++"
#include "MACE/SimDose/Action/ActionInitialization.h++"
#include "MACE/SimDose/Action/DetectorConstruction.h++"
#include "MACE/SimDose/Analysis.h++"
#include "MACE/SimDose/RunManager.h++"

#include "Mustard/Env/BasicEnv.h++"
#include "Mustard/Geant4X/Physics/MuonNLODecayPhysics.h++"
#include "Mustard/Geant4X/Physics/MuoniumNLODecayPhysics.h++"
#include "Mustard/Geant4X/Physics/MuoniumPhysics.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4RadioactiveDecayPhysics.hh"
#include "G4SpinDecayPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4VModularPhysicsList.hh"

#include "muc/utility"

namespace MACE::SimDose {

using namespace Mustard::LiteralUnit::Energy;

RunManager::RunManager(Mustard::CLI::Geant4ReferencePhysicsListModule<"QBBC_EMZ">& cli) :
    MPIRunManager{},
    fAnalysis{std::make_unique_for_overwrite<Analysis>()} {
    const auto verboseLevel{muc::to_underlying(Mustard::Env::BasicEnv::Instance().VerboseLevel())};

    const auto physicsList{cli.PhysicsList()};
    // Radioactivity
    physicsList->RegisterPhysics(new G4RadioactiveDecayPhysics{verboseLevel});
    // Muonium physics
    physicsList->RegisterPhysics(new Mustard::Geant4X::MuoniumPhysics<Detector::Description::Target>{verboseLevel});
    // HP decay for muon and muonium
    physicsList->RegisterPhysics(new Mustard::Geant4X::MuonNLODecayPhysics{verboseLevel});
    physicsList->RegisterPhysics(new Mustard::Geant4X::MuoniumNLODecayPhysics{verboseLevel});
    // Step limit
    physicsList->ReplacePhysics(new G4StepLimiterPhysics);
    SetUserInitialization(physicsList);

    const auto detectorConstruction{new DetectorConstruction};
    detectorConstruction->SetCheckOverlaps(Mustard::Env::VerboseLevelReach<'I'>());
    SetUserInitialization(detectorConstruction);

    SetUserInitialization(new ActionInitialization);
}

RunManager::~RunManager() = default;

} // namespace MACE::SimDose
