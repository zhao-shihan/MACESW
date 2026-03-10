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

#include "MACE/SimDose/Action/DetectorConstruction.h++"
#include "MACE/SimDose/DefaultMacro.h++"
#include "MACE/SimDose/RunManager.h++"
#include "MACE/SimDose/SimDose.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/CLI/Module/Geant4ReferencePhysicsListModule.h++"
#include "Mustard/Env/Geant4Env.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"

namespace MACE::SimDose {

SimDose::SimDose() :
    Subprogram{"SimDose", "Simulation of absorbed dose in the Muonium-to-Antimuonium Conversion Experiment (MACE)."} {}

auto SimDose::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<
        Mustard::CLI::Geant4ReferencePhysicsListModule<"QBBC_EMZ">,
        Mustard::CLI::DetectorDescriptionModule<DetectorConstruction::ProminentDescription>>
        cli;
    Mustard::Env::Geant4Env env{argc, argv, cli};

    RunManager runManager{cli};
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimDose
