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

#include "MACE/Data/HPTrackStateModel.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace MACE::Data {

/// @brief High-momentum track type enumeration
/// @note Corresponding to the "TrkType" field in BasicHPTrackModel
enum struct HPTrackType : std::int8_t {
    Unknown,  // default value or candidate track
    Normal,   // real track from physical event
    Fake,     // fake track
    CosmicRay // cosmic ray track
};

/// @brief Reference point type enumeration for high-momentum tracks
/// @note Corresponding to the "RefPtType" field in BasicHPTrackModel
enum struct HPTrackRefPtType : std::int8_t {
    Unknown,         // default value
    Virtual,         // virtual point
    Vertex,          // vertex
    FirstHit,        // first hit in the track
    LastHit,         // last hit in the track
    IntermediateHit, // intermediate hit in the track
    POCAToBeamAxis,  // point of closest approach to beam axis
    POCAToOrigin     // point of closest approach to origin
};

/// @brief High-momentum (high-p) track model, for tracks in MMS
/// @note For real data and simulation data
/// @extends HPTrackStateModel
using HPTrackModel = Mustard::Data::TupleModel<
    Mustard::Data::Value<std::int32_t, "EvtID", "Event ID">,
    Mustard::Data::Value<std::int32_t, "TrkID", "Track ID">,
    Mustard::Data::Value<float, "wt", "Track weight">,
    Mustard::Data::Value<float, "chi2", "Goodness of fit (chi^{2})">,
    Mustard::Data::Value<float, "ndf", "Number of degrees of freedom in fit">,
    Mustard::Data::Value<std::underlying_type_t<HPTrackType>, "TrkType", "Track type">,
    Mustard::Data::Value<std::underlying_type_t<HPTrackRefPtType>, "RefPtType", "Reference point type">,
    Mustard::Data::Value<std::int32_t, "PDGID", "Particle PDG ID">,
    HPTrackStateModel,
    Mustard::Data::Value<std::vector<std::int32_t>, "CellHitID", "CDC cell hits in this track">,
    Mustard::Data::Value<std::vector<std::int32_t>, "TTCHitID", "TTC hits in this track">>;
using HPTrack = Mustard::Data::Tuple<HPTrackModel>;

/// @brief Simulated high-momentum (high-p) track model, for tracks in MMS
/// @note Only for simulation data
/// @extends HPTrackModel
using SimHPTrackModel = Mustard::Data::TupleModel<
    HPTrackModel,
    Mustard::Data::Value<std::string, "CreatProc", "Track creator process (MC truth)">>;
using SimHPTrack = Mustard::Data::Tuple<SimHPTrackModel>;

} // namespace MACE::Data
