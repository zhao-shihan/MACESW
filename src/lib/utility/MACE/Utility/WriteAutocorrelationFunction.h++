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

#include "TAxis.h"
#include "TGraph.h"

#include "Eigen/Core"

#include "fmt/core.h"

#include <array>
#include <utility>
#include <vector>

namespace MACE::inline Utility {

template<int D>
auto WriteAutocorrelationFunction(const std::vector<std::pair<unsigned, Eigen::Array<double, D, 1>>>& autocorrelationFunction) -> void;

} // namespace MACE::inline Utility

#include "MACE/Utility/WriteAutocorrelationFunction.inl"
