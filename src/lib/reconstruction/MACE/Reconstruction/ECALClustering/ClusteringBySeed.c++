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

#include "MACE/Data/SimHit.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Reconstruction/ECALClustering/ClusteringBySeed.h++"

#include "Mustard/Data/Tuple.h++"

namespace MACE::inline Reconstruction::ECALClustering {

auto ClusteringBySeed(int seedID) -> gtl::flat_hash_set<int> {
    gtl::flat_hash_set<int> cluster;

    const auto& ecal{MACE::Detector::Description::ECAL::Instance()};
    const auto& moduleList{ecal.Array().moduleList};

    cluster.insert(seedID);
    for (auto&& neighbor : moduleList.at(seedID).neighborModuleID) {
        cluster.insert(neighbor);
        cluster.insert(
            moduleList.at(neighbor).neighborModuleID.begin(),
            moduleList.at(neighbor).neighborModuleID.end());
    }

    return cluster;
}

} // namespace MACE::inline Reconstruction::ECALClustering
