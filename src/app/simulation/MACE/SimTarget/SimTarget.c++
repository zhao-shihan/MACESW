#include "MACE/SimTarget/DefaultMacro.h++"
#include "MACE/SimTarget/RunManager.h++"
#include "MACE/SimTarget/SimTarget.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::SimTarget {

SimTarget::SimTarget() :
    Subprogram{"SimTarget", "Simulation of muonium production from muonium target."} {}

auto SimTarget::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<512> random{cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimTarget
