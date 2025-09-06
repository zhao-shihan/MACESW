#include "MACE/SimDose/DefaultMacro.h++"
#include "MACE/SimDose/RunManager.h++"
#include "MACE/SimDose/SimDose.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/CLI/Module/Geant4ReferencePhysicsListModule.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::SimDose {

SimDose::SimDose() :
    Subprogram{"SimDose", "Simulation of absorbed dose in the Muonium-to-Antimuonium Conversion Experiment (MACE)."} {}

auto SimDose::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<Mustard::CLI::Geant4ReferencePhysicsListModule<"QBBC_EMZ">> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<512> random{cli};

    RunManager runManager{cli};
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimDose
