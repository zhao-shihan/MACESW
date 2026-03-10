// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

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
