#include "MACE/SimECAL/DefaultMacro.h++"
#include "MACE/SimECAL/RunManager.h++"
#include "MACE/SimECAL/SimECAL.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::SimECAL {

SimECAL::SimECAL() :
    Subprogram{"SimECAL", "Simulation of events in electromagnetic calorimeter (ECAL)."} {}

auto SimECAL::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<512> random{cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimECAL
