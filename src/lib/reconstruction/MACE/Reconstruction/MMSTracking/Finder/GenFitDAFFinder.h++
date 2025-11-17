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

#include "MACE/Data/Hit.h++"
#include "MACE/Data/MMSTrack.h++"
#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Reconstruction/MMSTracking/Finder/FinderBase.h++"
#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitDAFFitter.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "CLHEP/Vector/ThreeVector.h"

#include "Eigen/Core"

#include "muc/algorithm"
#include "muc/array"
#include "muc/hash_set"

#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>

namespace MACE::inline Reconstruction::MMSTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit = Data::CDCHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack = Data::MMSTrack>
class GenFitDAFFinder : public FinderBase<AHit, ATrack> {
private:
    using Base = FinderBase<AHit, ATrack>;

public:
    GenFitDAFFinder(double driftErrorRMS);
    virtual ~GenFitDAFFinder() override = default;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
    auto operator()(const std::vector<AHitPointer>& hitData, int nextTrackID) -> Base::template Result<AHitPointer>;

private:
    double fFirstSegmentMaxDeltaPhi;
    double fFirstSegmentMinNHit;
    double fMomentumSeed;

    GenFitDAFFitter<AHit, ATrack> fDAFFitter;
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder

#include "MACE/Reconstruction/MMSTracking/Finder/GenFitDAFFinder.inl"
