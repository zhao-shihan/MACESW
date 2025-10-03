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
