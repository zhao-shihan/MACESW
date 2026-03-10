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

#include "MACE/Algorithm/ExtrapolationUtility.h++"
#include "MACE/Algorithm/HPTrackStateUtility.h++"
#include "MACE/Data/HPTrackStateModel.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Math/GeometryRepresentation.h++"
#include "Mustard/Math/POCA.h++"

#include "muc/math"
#include "muc/numeric"

#include <cmath>
#include <limits>
#include <numbers>
#include <optional>
#include <utility>

namespace MACE::inline Algorithm {

/// @brief Extrapolate the track geometrically along the helix by arc length s in-place
/// @param track The track
/// @param s Arc length to extrapolate (may be negative)
/// @note Updates time t, position x0, direction d0, and phi0 accordingly
auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, double deltaS) -> void;

/// @brief Extrapolate the track geometrically to the POCA to a given z axis in-place
/// @param track The track
/// @param axis Target z axis (represented by 2D point in XY plane)
/// @param mode Extrapolation mode (default: Nearest)
/// @param nominalBz Nominal axial magnetic field
/// @return An instance of ExtrapolationResult if succeeded (see `struct ExtrapolationResult`)
/// @note The track will remain unchanged if extrapolation failed
auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, Mustard::Point2D axis,
                 ExtrapolationMode mode = ExtrapolationMode::Nearest) -> std::optional<ExtrapolationResult>;

/// @brief Extrapolate the track geometrically to the POCA to a given position, within
/// a given maximum allowed change in azimuthal angle (maxDeltaPhi), in-place.
/// @param track The track
/// @param point Target position
/// @param mode Extrapolation mode (default: Nearest)
/// @param maxDeltaPhi Maximum allowed change in azimuthal angle (default: pi, should be positive)
/// @param nTrialPts Number of trial points for initial grid search. If zero or negative,
/// skip both the unimodality check and initial grid search; if 1, determine the actual
/// number automatically (default: 1, i.e. determined automatically)
/// @param maxIter Maximum number of iterations for minimization (default: 300)
/// @param absTol Absolute tolerance in phi for minimization (default: muc::default_abs_tol<float> ~ 1.2e-7)
/// @param relTol Relative tolerance in phi for minimization (default: std::numeric_limits<float>::epsilon() ~ 1.2e-7)
/// @return An instance of ExtrapolationResult if succeeded (see `struct ExtrapolationResult`)
/// @note The track will remain unchanged if extrapolation failed
auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, Mustard::Point3D point,
                 ExtrapolationMode mode = ExtrapolationMode::Nearest,
                 double maxDeltaPhi = std::numbers::pi, int nTrialPts = 1,
                 int maxIter = 300, double absTol = muc::default_abs_tol<float>,
                 double relTol = std::numeric_limits<float>::epsilon()) -> std::optional<ExtrapolationResult>;

/// @brief Extrapolate the track geometrically to the POCA to a given line, within
/// a given maximum allowed change in azimuthal angle (maxDeltaPhi), in-place.
/// @param track The track
/// @param line Target line
/// @param mode Extrapolation mode (default: Nearest)
/// @param maxDeltaPhi Maximum allowed change in azimuthal angle (default: pi, should be positive)
/// @param nTrialPts Number of trial points for initial grid search. If zero or negative,
/// skip both the unimodality check and initial grid search; if 1, determine the actual
/// number automatically (default: 1, i.e. determined automatically)
/// @param maxIter Maximum number of iterations for minimization (default: 300)
/// @param absTol Absolute tolerance in phi for minimization (default: muc::default_abs_tol<float> ~ 1.2e-7)
/// @param relTol Relative tolerance in phi for minimization (default: std::numeric_limits<float>::epsilon() ~ 1.2e-7)
/// @return An instance of ExtrapolationResult if succeeded (see `struct ExtrapolationResult`)
/// @note The track will remain unchanged if extrapolation failed
auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, Mustard::Line3D line,
                 ExtrapolationMode mode = ExtrapolationMode::Nearest,
                 double maxDeltaPhi = std::numbers::pi, int nTrialPts = 1,
                 int maxIter = 300, double absTol = muc::default_abs_tol<float>,
                 double relTol = std::numeric_limits<float>::epsilon()) -> std::optional<ExtrapolationResult>;

} // namespace MACE::inline Algorithm

#include "MACE/Algorithm/ExtrapolateHPTrackState.inl"
