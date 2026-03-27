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

#include "MACE/Data/SimHit.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Reconstruction/ECALClustering/ClusteringBySeed.h++"

#include "Mustard/Data/Tuple.h++"

#include "CLHEP/Vector/ThreeVector.h"

#include <memory>
#include <unordered_map>

namespace MACE::inline Reconstruction::ECALClustering {

struct ClusterTuple {
    double energy{};
    int peCount{};
    Mustard::Vector3D weightedPosition{};
    Mustard::Vector3D position{};
};

inline auto Reconstructing(int seedID,
                           const std::vector<MACE::Detector::Description::ECAL::ArrayInformation::Module>& moduleList,
                           const std::unordered_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>>& hitDict,
                           double energyThreshold,
                           bool useOpticalResponse = false,
                           int peCountThreshold = 0) -> ClusterTuple;

} // namespace MACE::inline Reconstruction::ECALClustering

#include "MACE/Reconstruction/ECALClustering/Reconstructing.inl"
