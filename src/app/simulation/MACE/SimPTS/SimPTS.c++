#include "MACE/SimPTS/Action/DetectorConstruction.h++"
#include "MACE/SimPTS/DefaultMacro.h++"
#include "MACE/SimPTS/RunManager.h++"
#include "MACE/SimPTS/SimPTS.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/Env/Geant4Env.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"

namespace MACE::SimPTS {

SimPTS::SimPTS() :
    Subprogram{"SimPTS", "Simulation of particle transportation in positron transport system (PTS)."} {}

auto SimPTS::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<Mustard::CLI::DetectorDescriptionModule<DetectorConstruction::ProminentDescription>> cli;
    Mustard::Env::Geant4Env env{argc, argv, cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimPTS
