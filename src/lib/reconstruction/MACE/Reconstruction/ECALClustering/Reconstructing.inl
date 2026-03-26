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

#include "Mustard/Data/Tuple.h++"

namespace MACE::inline Reconstruction::ECALClustering {

inline auto Reconstructing(int seedID,
                           const std::vector<MACE::Detector::Description::ECAL::ArrayInformation::Module>& moduleList,
                           const std::unordered_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>>& hitDict,
                           double energyThreshold,
                           bool useOpticalResponse,
                           int peCountThreshold) -> ClusterTuple {
    ClusterTuple result;

    auto cluster{ClusteringBySeed(seedID, moduleList)};
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
