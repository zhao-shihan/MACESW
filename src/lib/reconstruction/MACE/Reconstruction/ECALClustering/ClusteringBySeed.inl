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

namespace MACE::inline Reconstruction::ECALClustering {

inline auto ClusteringBySeed(int seedID,
                             const std::vector<MACE::Detector::Description::ECAL::ArrayInformation::Module>& moduleList) -> std::unordered_set<int> {
    std::unordered_set<int> modulesInCluster;

    modulesInCluster.insert(seedID);
    for (auto&& neighbor : moduleList.at(seedID).neighborModuleID) {
        modulesInCluster.insert(neighbor);
        modulesInCluster.insert(
            moduleList.at(neighbor).neighborModuleID.begin(),
            moduleList.at(neighbor).neighborModuleID.end());
    }

    return modulesInCluster;
}

} // namespace MACE::inline Reconstruction::ECALClustering
