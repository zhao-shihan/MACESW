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

#include "MACE/Data/internal/HitModelHeader.h++"
#include "MACE/Data/internal/SimHitTrackTruthModel.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

#include <cstdint>

namespace MACE::Data {

// TODO: MCPDigi, SimMCPDigi?

/// @brief MCP hit model
/// @note For real and simulation data
using MCPHitModel = Mustard::Data::TupleModel<
    internal::HitModelHeader,
    Mustard::Data::Value<std::int32_t, "DetID", "Detector ID">,
    Mustard::Data::Value<double, "t", "[ns] Hit time">,
    Mustard::Data::Value<muc::array2f, "x", "[mm] Hit position">>;
using MCPHit = Mustard::Data::Tuple<MCPHitModel>;

/// @brief Simulated MCP hit model
/// @note Only for simulation data
/// @extends MCPHitModel
using SimMCPHitModel = Mustard::Data::TupleModel<
    MCPHitModel,
    Mustard::Data::Value<float, "Edep", "[MeV] Energy deposition (MC truth)">,
    Mustard::Data::Value<float, "Ek", "[MeV] Hit kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "[MeV/c] Hit momentum (MC truth)">,
    internal::SimHitTrackTruthModel>;
using SimMCPHit = Mustard::Data::Tuple<SimMCPHitModel>;

} // namespace MACE::Data
