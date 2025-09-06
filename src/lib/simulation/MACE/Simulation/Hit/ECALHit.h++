#pragma once

#include "MACE/Data/SimHit.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Geant4X/Memory/UseG4Allocator.h++"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"

namespace MACE::inline Simulation::inline Hit {

class ECALHit final : public Mustard::Geant4X::UseG4Allocator<ECALHit>,
                      public G4VHit,
                      public Mustard::Data::Tuple<Data::ECALSimHit> {};

using ECALHitCollection = G4THitsCollection<ECALHit>;

} // namespace MACE::inline Simulation::inline Hit
