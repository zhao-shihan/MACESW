#include "MACE/PhaseI/SimMACEPhaseI/PhysicsList.h++"

#include "Mustard/Geant4X/Physics/MuoniumSMAndLFVDecayPhysics.h++"

namespace MACE::PhaseI::SimMACEPhaseI {

PhysicsList::PhysicsList() :
    PassiveSingleton{this},
    StandardPhysicsListBase{} {
    ReplacePhysics(new Mustard::Geant4X::MuoniumSMAndLFVDecayPhysics{verboseLevel});
}

} // namespace MACE::PhaseI::SimMACEPhaseI
