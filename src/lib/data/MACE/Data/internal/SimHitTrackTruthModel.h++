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

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

#include <cstdint>
#include <string>

namespace MACE::Data::internal {

/// @brief Shared between many simulated hit models
using SimHitTrackTruthModel = Mustard::Data::TupleModel<
    Mustard::Data::Value<std::int32_t, "TrkID", "MC Track ID">,
    Mustard::Data::Value<std::int32_t, "PDGID", "Particle PDG ID (MC truth)">,
    Mustard::Data::Value<double, "t0", "[ns] Track initial time (MC truth)">,
    Mustard::Data::Value<muc::array3f, "x0", "[mm] Track initial position (MC truth)">,
    Mustard::Data::Value<float, "Ek0", "[MeV] Track initial kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p0", "[MeV/c] Track initial momentum (MC truth)">,
    Mustard::Data::Value<std::string, "CreatProc", "Track creator process (MC truth)">>;

} // namespace MACE::Data::internal
