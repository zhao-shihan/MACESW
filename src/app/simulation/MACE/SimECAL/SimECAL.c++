#include "MACE/SimECAL/Action/DetectorConstruction.h++"
#include "MACE/SimECAL/DefaultMacro.h++"
#include "MACE/SimECAL/RunManager.h++"
#include "MACE/SimECAL/SimECAL.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/Env/Geant4Env.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"

namespace MACE::SimECAL {

SimECAL::SimECAL() :
    Subprogram{"SimECAL", "Simulation of events in electromagnetic calorimeter (ECAL)."} {}

auto SimECAL::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<Mustard::CLI::DetectorDescriptionModule<DetectorConstruction::ProminentDescription>> cli;
    Mustard::Env::Geant4Env env{argc, argv, cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimECAL
