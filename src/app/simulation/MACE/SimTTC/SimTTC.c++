#include "MACE/SimTTC/Action/DetectorConstruction.h++"
#include "MACE/SimTTC/DefaultMacro.h++"
#include "MACE/SimTTC/RunManager.h++"
#include "MACE/SimTTC/SimTTC.h++"

#include "Mustard/CLI/Geant4CLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/Env/Geant4Env.h++"
#include "Mustard/Geant4X/Interface/MPIExecutive.h++"

namespace MACE::SimTTC {

SimTTC::SimTTC() :
    Subprogram{"SimTTC", "Simulation of events in Michel magnetic spectrometer (MMS)."} {}

auto SimTTC::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::Geant4CLI<Mustard::CLI::DetectorDescriptionModule<DetectorConstruction::ProminentDescription>> cli;
    Mustard::Env::Geant4Env env{argc, argv, cli};

    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimTTC
