#include "MACE/PhaseI/MakeGeometry/MakeGeometry.h++"
#include "MACE/PhaseI/PhaseI.h++"
#include "MACE/PhaseI/ReconECAL/ReconECAL.h++"
#include "MACE/PhaseI/ReconSciFi/GenFitTest.h++"
#include "MACE/PhaseI/ReconSciFi/ReconSciFi.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SimMACEPhaseI.h++"

#include "Mustard/Application/SubprogramLauncher.h++"

namespace MACE::PhaseI {

PhaseI::PhaseI() :
    Subprogram{"PhaseI", "Subprograms for the Phase-I Muonium-to-Antimuonium Conversion Experiment (MACE Phase-I)."} {}

auto PhaseI::Main(int argc, char* argv[]) const -> int {
    Mustard::Application::SubprogramLauncher launcher;
    launcher.AddSubprogram<MACE::PhaseI::ReconECAL::ReconECAL>();
    launcher.AddSubprogram<MACE::PhaseI::ReconSciFi::ReconSciFi>();
    launcher.AddSubprogram<MACE::PhaseI::ReconSciFi::GenFitTest>();
    launcher.AddSubprogram<MACE::PhaseI::MakeGeometry::MakeGeometry>();
    launcher.AddSubprogram<MACE::PhaseI::SimMACEPhaseI::SimMACEPhaseI>();
    return launcher.LaunchMain(argc, argv);
}

} // namespace MACE::PhaseI
