#include "MACE/SimTTC/DefaultMacro.h++"
#include "MACE/SimTTC/RunManager.h++"
#include "MACE/SimTTC/SimTTC.h++"

#include "Mustard/Env/CLI/Geant4CLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Extension/Geant4X/Interface/MPIExecutive.h++"
#include "Mustard/Utility/UseXoshiro.h++"

namespace MACE::SimTTC {

SimTTC::SimTTC() :
    Subprogram{"SimTTC", "Simulation of events in Michel magnetic spectrometer (MMS)."} {}

auto SimTTC::Main(int argc, char* argv[]) const -> int {
    Mustard::Env::CLI::Geant4CLI<> cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};

    Mustard::UseXoshiro<512> random;
    cli.SeedRandomIfFlagged();

    // PhysicsList, DetectorConstruction, ActionInitialization are instantiated in RunManager constructor.
    // Mutually exclusive random seeds are distributed to all processes upon each BeamOn.
    RunManager runManager;
    Mustard::Geant4X::MPIExecutive{}.StartSession(cli, defaultMacro);

    return EXIT_SUCCESS;
}

} // namespace MACE::SimTTC
