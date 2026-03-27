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
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

#include <cstdint>

namespace MACE::Data {

enum struct LPTrackType : std::int8_t {
    Unknown, // default value or candidate track
    Normal,  // real track from physical event
    Fake     // fake track
};

enum struct LPTrackRefPtType : std::int8_t {
    Unknown, // default value
    Virtual, // virtual point
    Vertex,  // vertex
    MCPHit   // MCP hit
};

/// @brief Low-momentum (low-p) track model, for tracks in PTS
/// @note For real data and simulation data
using LPTrackModel = Mustard::Data::TupleModel<
    Mustard::Data::Value<std::int32_t, "EvtID", "Event ID">,
    Mustard::Data::Value<std::int32_t, "TrkID", "Track ID">,
    Mustard::Data::Value<float, "wt", "Track weight">,
    Mustard::Data::Value<std::underlying_type_t<LPTrackType>, "TrkType", "Track type">,
    Mustard::Data::Value<std::underlying_type_t<LPTrackRefPtType>, "RefPtType", "Reference point type">,
    Mustard::Data::Value<std::int32_t, "PDGID", "Particle PDG ID">,
    Mustard::Data::Value<double, "t0", "[ns] Reference time">,
    Mustard::Data::Value<muc::array3f, "x0", "[mm] Reference position">,
    Mustard::Data::Value<std::int32_t, "MCPHitID", "MCP hit in this track">,
    Mustard::Data::Value<std::vector<std::int32_t>, "ECALClsID", "ECAL hit clusters for this track">>;
using LPTrack = Mustard::Data::Tuple<LPTrackModel>;

} // namespace MACE::Data
