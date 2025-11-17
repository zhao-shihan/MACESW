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

#include "MACE/Data/MMSTrack.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Reconstruction/MMSTracking/Finder/FinderBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/IO/PrettyLog.h++"

#include "CLHEP/Vector/ThreeVector.h"

#include "muc/algorithm"
#include "muc/hash_set"

#include <algorithm>
#include <iterator>
#include <ranges>

namespace MACE::inline Reconstruction::MMSTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<Data::CDCSimHit> AHit = Data::CDCSimHit,
         Mustard::Data::SuperTupleModel<Data::MMSSimTrack> ATrack = Data::MMSSimTrack>
class TruthFinder : public FinderBase<AHit, ATrack> {
private:
    using Base = FinderBase<AHit, ATrack>;

public:
    TruthFinder();
    ~TruthFinder() override = default;

    auto MaxVertexRxy() const -> auto { return fMaxVertexRxy; }
    auto MaxVertexRxy(double r) -> void { fMaxVertexRxy = std::max(0., r); }

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
    auto operator()(const std::vector<AHitPointer>& hitData, int = {}) const -> Base::template Result<AHitPointer>;

private:
    double fMaxVertexRxy;
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder

#include "MACE/Reconstruction/MMSTracking/Finder/TruthFinder.inl"
