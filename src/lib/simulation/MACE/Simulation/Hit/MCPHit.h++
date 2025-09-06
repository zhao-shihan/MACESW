#pragma once

#include "MACE/Data/SimHit.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Geant4X/Memory/UseG4Allocator.h++"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"

namespace MACE::inline Simulation::inline Hit {

class MCPHit final : public Mustard::Geant4X::UseG4Allocator<MCPHit>,
                     public G4VHit,
                     public Mustard::Data::Tuple<Data::MCPSimHit> {};

using MCPHitCollection = G4THitsCollection<MCPHit>;

} // namespace MACE::inline Simulation::inline Hit
