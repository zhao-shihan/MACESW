#pragma once

#include "MACE/Data/SensorHit.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Geant4X/Memory/UseG4Allocator.h++"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"

namespace MACE::inline Simulation::inline Hit {

class ECALPMHit final : public Mustard::Geant4X::UseG4Allocator<ECALPMHit>,
                        public G4VHit,
                        public Mustard::Data::Tuple<Data::ECALPMHit> {};

using ECALPMHitCollection = G4THitsCollection<ECALPMHit>;

} // namespace MACE::inline Simulation::inline Hit
