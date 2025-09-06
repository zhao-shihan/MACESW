#include "MACE/PhaseI/SimMACEPhaseI/DefaultMacro.h++"
#include "MACE/PhaseI/SimMACEPhaseI/RunManager.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SimMACEPhaseI.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::PhaseI::SimMACEPhaseI {

SimMACEPhaseI::SimMACEPhaseI() :
    Subprogram{"SimMACEPhaseI", "Simulation of events in the Phase-I Muonium-to-Antimuonium Conversion Experiment (MACE Phase-I)."} {}

auto SimMACEPhaseI::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<512> random{cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::PhaseI::SimMACEPhaseI
