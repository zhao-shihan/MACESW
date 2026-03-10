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

#include "Mustard/Math/GeometryRepresentation.h++"

namespace MACE::inline Algorithm {

/// @brief Extrapolation mode for extrapolation to POCA to a target object
enum struct ExtrapolationMode {
    Nearest, // extrapolate to the nearest POCA
    Forward, // extrapolate forward only; fail if not possible
    Backward // extrapolate backward only; fail if not possible
};

/// @brief Result of extrapolation to POCA to a target object
struct ExtrapolationResult {
    double doca;           // distance of closest approach
    double deltaS;         // arc length extrapolated
    Mustard::Point3D poca; // point of closest approach on target object
};

} // namespace MACE::inline Algorithm
