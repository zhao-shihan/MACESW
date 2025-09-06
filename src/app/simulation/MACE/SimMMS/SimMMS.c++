#include "MACE/SimMMS/DefaultMacro.h++"
#include "MACE/SimMMS/RunManager.h++"
#include "MACE/SimMMS/SimMMS.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::SimMMS {

SimMMS::SimMMS() :
    Subprogram{"SimMMS", "Simulation of events in Michel magnetic spectrometer (MMS)."} {}

auto SimMMS::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<512> random{cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimMMS
