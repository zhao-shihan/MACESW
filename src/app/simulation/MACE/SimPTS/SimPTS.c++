#include "MACE/SimPTS/DefaultMacro.h++"
#include "MACE/SimPTS/RunManager.h++"
#include "MACE/SimPTS/SimPTS.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::SimPTS {

SimPTS::SimPTS() :
    Subprogram{"SimPTS", "Simulation of particle transportation in positron transport system (PTS)."} {}

auto SimPTS::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<512> random{cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimPTS
