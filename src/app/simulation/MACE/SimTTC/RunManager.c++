#include "MACE/SimTTC/Action/ActionInitialization.h++"
#include "MACE/SimTTC/Action/DetectorConstruction.h++"
#include "MACE/SimTTC/Analysis.h++"
#include "MACE/SimTTC/RunManager.h++"
#include "MACE/Simulation/Physics/StandardPhysicsList.h++"

#include "Mustard/Env/BasicEnv.h++"

#include "muc/utility"

namespace MACE::SimTTC {

RunManager::RunManager() :
    MPIRunManager{},
    fAnalysis{std::make_unique_for_overwrite<Analysis>()} {

    SetUserInitialization(new StandardPhysicsList);

    const auto detectorConstruction{new DetectorConstruction};
    detectorConstruction->SetCheckOverlaps(Mustard::Env::VerboseLevelReach<'I'>());
    SetUserInitialization(detectorConstruction);

    SetUserInitialization(new ActionInitialization);
}

RunManager::~RunManager() = default;

} // namespace MACE::SimTTC
