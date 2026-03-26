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

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Math/Vector.h++"

#include "gtl/phmap.hpp"

#include <memory>

namespace MACE::inline Reconstruction::ECALClustering {

struct ClusterResult {
    double energy{};
    int peCount{};
    Mustard::Vector3D weightedPosition{};
    Mustard::Vector3D position{};
};

auto Reconstructing(int seedID,
                    const gtl::flat_hash_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>>& hitDict,
                    double energyThreshold,
                    bool useOpticalResponse = false,
                    int peCountThreshold = 0) -> ClusterResult;

} // namespace MACE::inline Reconstruction::ECALClustering
