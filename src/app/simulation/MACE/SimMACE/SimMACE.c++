#include "MACE/SimMACE/Action/DetectorConstruction.h++"
#include "MACE/SimMACE/DefaultMacro.h++"
#include "MACE/SimMACE/RunManager.h++"
#include "MACE/SimMACE/SimMACE.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/Env/Geant4Env.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"

namespace MACE::SimMACE {

SimMACE::SimMACE() :
    Subprogram{"SimMACE", "Simulation of events in the Muonium-to-Antimuonium Conversion Experiment (MACE)."} {}

auto SimMACE::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<Mustard::CLI::DetectorDescriptionModule<DetectorConstruction::ProminentDescription>> cli;
    Mustard::Env::Geant4Env env{argc, argv, cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimMACE
