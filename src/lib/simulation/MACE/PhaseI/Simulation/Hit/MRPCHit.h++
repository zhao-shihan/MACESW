#pragma once

#include "MACE/PhaseI/Data/SimHit.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Geant4X/Memory/UseG4Allocator.h++"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"

namespace MACE::PhaseI::inline Simulation::inline Hit {

class MRPCHit final : public Mustard::Geant4X::UseG4Allocator<MRPCHit>,
                      public G4VHit,
                      public Mustard::Data::Tuple<Data::MRPCSimHit> {};

using MRPCHitCollection = G4THitsCollection<MRPCHit>;

} // namespace MACE::PhaseI::inline Simulation::inline Hit
