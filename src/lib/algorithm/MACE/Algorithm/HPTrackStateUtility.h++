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
#include "MACE/Detector/Description/MMSField.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Math/GeometryRepresentation.h++"
#include "Mustard/Math/Vector.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "muc/array"
#include "muc/math"

#include <cmath>

namespace MACE::inline Algorithm {

/// @brief Convert HPTrackState to Helix representation
/// @param track The track
/// @return An instance of Helix representing the helix state of the track
auto AsHelix(const Mustard::Data::SuperTuple<HPTrackState> auto& track) -> Mustard::Helix;

/// @brief Calculate kinematic state from known helix state in-place
/// @param track The track
/// @param nominalB Nominal axial magnetic field
auto UpdateKinematicState(Mustard::Data::SuperTuple<HPTrackState> auto& track,
                          double nominalB = Detector::Description::MMSField::Instance().NominalField()) -> void;

/// @brief Calculate helix state from known kinematic state in-place
/// @param track The track
/// @param nominalB Nominal axial magnetic field
auto UpdateHelixState(Mustard::Data::SuperTuple<HPTrackState> auto& track,
                      double nominalB = Detector::Description::MMSField::Instance().NominalField()) -> void;

} // namespace MACE::inline Algorithm

#include "MACE/Algorithm/HPTrackStateUtility.inl"
