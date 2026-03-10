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

#include "MACE/Data/WaveformModel.h++"
#include "MACE/Data/internal/DigiModelHeader.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

#include <cstdint>

namespace MACE::Data {

/// @brief Simulated photosensor hit model
/// @note Only for simulation data
using SimPhotosensorHitModel = Mustard::Data::TupleModel<
    Mustard::Data::Value<std::int32_t, "EvtID", "Event ID">,
    Mustard::Data::Value<std::int32_t, "ChID", "Channel ID">,
    Mustard::Data::Value<double, "t", "[ns] Hit time">,
    Mustard::Data::Value<muc::array2f, "x", "[mm] Relative hit position">,
    Mustard::Data::Value<muc::array3f, "k", "[1/mm] Photon wave vector">>;
using SimPhotosensorHit = Mustard::Data::Tuple<SimPhotosensorHitModel>;

/// @brief Photosensor digit model
/// @note For real and simulation data
using PhotosensorDigiModel = Mustard::Data::TupleModel<
    internal::DigiModelHeader,
    Mustard::Data::Value<std::int32_t, "ChID", "Channel ID">,
    Mustard::Data::Value<float, "Npe", "Number of photoelectrons">,
    Mustard::Data::Value<double, "t", "[ns] Signal time">,
    WaveformModel>;
using PhotosensorDigi = Mustard::Data::Tuple<PhotosensorDigiModel>;

} // namespace MACE::Data
