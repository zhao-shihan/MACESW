#include "MACE/SimTarget/Action/DetectorConstruction.h++"
#include "MACE/SimTarget/DefaultMacro.h++"
#include "MACE/SimTarget/RunManager.h++"
#include "MACE/SimTarget/SimTarget.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/Env/Geant4Env.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"

namespace MACE::SimTarget {

SimTarget::SimTarget() :
    Subprogram{"SimTarget", "Simulation of muonium production from muonium target."} {}

auto SimTarget::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<Mustard::CLI::DetectorDescriptionModule<DetectorConstruction::ProminentDescription>> cli;
    Mustard::Env::Geant4Env env{argc, argv, cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimTarget
