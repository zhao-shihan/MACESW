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

/// @brief High-momentum (high-p) track state model.
/// This model assumes ultra-relativistic particles in a uniform axial magnetic field
///
/// Capable of representing both helix tracks and straight line tracks.
/// Consists of two sets of track parameters: physical parameters and helix parameters.
/// Straight line tracks can be represented with infinite transverse radius (r = +infinity).
/// Helix parameters are not automatically synchronized with physical parameters;
/// modifications to either set of parameters do not update the other set.
/// User should call the provided functions to synchronize them.
///
/// @details The helix parametric equations are defined as
/// @f[
/// \begin{equation}
/// x(\phi) & = c_x + r \cos\left( \phi + \phi_0 \right) \quad
/// y(\phi) & = c_y + r \sin\left( \phi + \phi_0 \right) \quad
/// z(\phi) & = z_0 + r \phi \cot\lambda
/// \end{equation}
/// @f]
/// where @f$(c_x, c_y)\in\mathbb{R}^2@f$ is the transverse center,
/// @f$r\in(0, +\infty)@f$ is the transverse radius,
/// @f$\phi_0\in(-\pi, \pi]@f$ is the helix azimuthal angle at the reference point,
/// @f$z_0\in\mathbb{R}@f$ is the reference z coordinate, and
/// @f$\lambda\in(-\pi, \pi)\backslash\{0\}@f$ is the dip angle.
///
/// Although @f$\tan\lambda@f$ has mathematical poles at @f$\pm\pi/2@f$,
/// no common floating-point representation is able to represent @f$\pm\pi/2@f$ exactly,
/// thus there is no value of the argument for which a pole error occurs.
///
/// The relationship between direction and @f$(\phi_0, \lambda)@f$ is given by
/// @f[
/// \begin{equation}
/// d_{0,x} & = -\sin\phi_0 \sin\lambda \quad
/// d_{0,y} & = \cos\phi_0 \sin\lambda \quad
/// d_{0,z} & = \cos\lambda
/// \end{equation}
/// @f]
///
/// @warning This model lacks many transactional information and only for
/// computation. Use extended models for data storage
using HPTrackStateModel = Mustard::Data::TupleModel<
    Mustard::Data::Value<std::int8_t, "q", "[e] Particle charge">,
    Mustard::Data::Value<double, "t", "[ns] Reference time">,
    // kinematic state at reference point
    Mustard::Data::Value<muc::array3f, "x0", "[mm] Reference position">,
    Mustard::Data::Value<muc::array3f, "d0", "Momentum direction (unit vector) at reference point">,
    Mustard::Data::Value<float, "p0", "[MeV/c] Momentum magnitude at reference point">,
    // helix state at reference point
    Mustard::Data::Value<muc::array2f, "c", "[mm] Transverse center at reference point">,
    Mustard::Data::Value<float, "r", "[mm] Transverse radius at reference point">,
    Mustard::Data::Value<float, "phi0", "[rad] Helix azimuthal angle at reference point">,
    Mustard::Data::Value<float, "lambda", "[rad] Dip angle at reference point">>;
using HPTrackState = Mustard::Data::Tuple<HPTrackStateModel>;

} // namespace MACE::Data
