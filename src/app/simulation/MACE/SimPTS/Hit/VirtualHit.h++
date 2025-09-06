#pragma once

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/Value.h++"
#include "Mustard/Geant4X/Memory/UseG4Allocator.h++"

#include "G4THitsCollection.hh"
#include "G4VHit.hh"

namespace MACE::SimPTS::inline Hit {

using VirtualHitModel = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "DetID", "Hit detector ID">,
    Mustard::Data::Value<int, "EvtID", "Event ID">,
    Mustard::Data::Value<int, "TrkID", "MC Track ID">,
    Mustard::Data::Value<int, "PDGID", "Particle PDG ID">,
    Mustard::Data::Value<double, "t", "Hit time">,
    Mustard::Data::Value<double, "t0", "Vertex time">,
    Mustard::Data::Value<muc::array2f, "x", "Hit position (projected)">,
    Mustard::Data::Value<muc::array3f, "x0", "Vertex position">,
    Mustard::Data::Value<float, "Ek", "Hit kinetic energy">,
    Mustard::Data::Value<float, "Ek0", "Vertex kinetic energy">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum">,
    Mustard::Data::Value<muc::array3f, "p0", "Vertex momentum">,
    Mustard::Data::Value<std::string, "CreatProc", "Track creator process">>;

class VirtualHit final : public Mustard::Geant4X::UseG4Allocator<VirtualHit>,
                         public G4VHit,
                         public Mustard::Data::Tuple<VirtualHitModel> {};

using VirtualHitCollection = G4THitsCollection<VirtualHit>;

} // namespace MACE::SimPTS::inline Hit
