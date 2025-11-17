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

namespace MACE::inline Utility {

template<int D>
auto WriteAutocorrelationFunction(const std::vector<std::pair<unsigned, Eigen::Array<double, D, 1>>>& autocorrelationFunction) -> void {
    std::array<TGraph, D> graph;
    for (int i{}; i < D; ++i) {
        graph[i].SetName(fmt::format("ACF_u{}", i).c_str());
        graph[i].SetTitle(fmt::format("Autocorrelation function of u{}", i).c_str());
        graph[i].GetXaxis()->SetTitle("Lag");
        graph[i].GetYaxis()->SetTitle("Autocorrelation");
    }
    for (auto&& [lag, autocorrelation] : autocorrelationFunction) {
        for (int i{}; i < D; ++i) {
            graph[i].AddPoint(lag, autocorrelation[i]);
        }
    }
    for (int i{}; i < D; ++i) {
        graph[i].SetOption("P");
        graph[i].Write();
    }
}

} // namespace MACE::inline Utility
