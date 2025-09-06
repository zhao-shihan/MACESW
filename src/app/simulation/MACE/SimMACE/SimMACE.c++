#include "MACE/SimMACE/DefaultMacro.h++"
#include "MACE/SimMACE/RunManager.h++"
#include "MACE/SimMACE/SimMACE.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::SimMACE {

SimMACE::SimMACE() :
    Subprogram{"SimMACE", "Simulation of events in the Muonium-to-Antimuonium Conversion Experiment (MACE)."} {}

auto SimMACE::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<512> random{cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimMACE
