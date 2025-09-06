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
