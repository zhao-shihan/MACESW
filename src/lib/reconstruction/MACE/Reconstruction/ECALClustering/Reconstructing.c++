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
#include "MACE/Reconstruction/ECALClustering/Reconstructing.h++"

namespace MACE::inline Reconstruction::ECALClustering {

auto Reconstructing(int seedID,
                    const gtl::flat_hash_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>>& hitDict,
                    double energyThreshold,
                    bool useOpticalResponse,
                    int peCountThreshold) -> ClusterResult {
    ClusterResult result;

    const auto& ecal{MACE::Detector::Description::ECAL::Instance()};
    const auto& moduleList{ecal.Array().moduleList};

    const auto cluster{ClusteringBySeed(seedID)};
    for (auto&& module : cluster) {
        const auto hitIt{hitDict.find(module)};
        if (hitIt == hitDict.end()) {
            continue;
        }
        const auto& [moduleID, hit]{*hitIt};

        const auto energy{Get<"Edep">(*hit)};
        if (energy < energyThreshold) {
            continue;
        }

        result.weightedPosition += energy * moduleList.at(moduleID).centroid;
        result.energy += energy;

        if (useOpticalResponse) {
            const auto hitPE{Get<"nOptPho">(*hit)};
            if (hitPE > peCountThreshold) {
                result.peCount += hitPE;
            }
        }
    }

    if (result.energy != 0) {
        result.position = result.weightedPosition / result.energy;
    }

    return result;
}

} // namespace MACE::inline Reconstruction::ECALClustering
