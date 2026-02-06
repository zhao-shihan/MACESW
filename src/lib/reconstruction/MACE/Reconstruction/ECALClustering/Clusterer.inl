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

auto Clusterer(int seedID,
               const std::vector<MACE::Detector::Description::ECAL::ArrayInformation::Module>& moduleList) -> std::unordered_set<int> {
    // ECALCluster cluster;
    std::unordered_set<int> modulesInCluster;

    modulesInCluster.insert(seedID);
    for (auto&& neighbor : moduleList.at(seedID).neighborModuleID) {
        modulesInCluster.insert(neighbor);
        modulesInCluster.insert(
            moduleList.at(neighbor).neighborModuleID.begin(),
            moduleList.at(neighbor).neighborModuleID.end());
    }

    // CLHEP::Hep3Vector weightedPosition{};

    // for (const auto& module : modulesInCluster) {
    //     auto hitIt = hitDict.find(module);
    //     if (hitIt == hitDict.end()) {
    //         continue;
    //     }

    // const auto& hit = hitIt->second;
    // auto energy = Get<"Edep">(hit);
    // auto pe = Get<"nOptPho">(hit);

    // if (energy < cfg.energyThreshold) {
    //     continue;
    // }
    // if (cfg.useOpticalPhysics and pe < cfg.peThreshold) {
    //     continue;
    // }

    // cluster.energy += energy;
    // cluster.pe += pe;
    // weightedPosition += energy * moduleList.at(module).centroid;
    // }

    // if (cluster.energy > 0) {
    //     cluster.position = weightedPosition / cluster.energy;
    // }

    return modulesInCluster;
}

} // namespace MACE::inline Reconstruction::ECALClustering
