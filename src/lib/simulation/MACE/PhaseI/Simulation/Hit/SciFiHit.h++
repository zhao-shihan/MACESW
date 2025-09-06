#pragma once

#include "MACE/PhaseI/Data/SimHit.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Geant4X/Memory/UseG4Allocator.h++"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"

namespace MACE::PhaseI::inline Simulation::inline Hit {

class SciFiHit final : public Mustard::Geant4X::UseG4Allocator<SciFiHit>,
                       public G4VHit,
                       public Mustard::Data::Tuple<PhaseI::Data::SciFiSimHit> {};

using SciFiHitCollection = G4THitsCollection<SciFiHit>;

} // namespace MACE::PhaseI::inline Simulation::inline Hit
