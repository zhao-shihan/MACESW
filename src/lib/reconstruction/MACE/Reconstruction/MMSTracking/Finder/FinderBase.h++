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

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/IO/PrettyLog.h++"

#include "muc/hash_map"
#include "muc/hash_set"

#include "fmt/core.h"
#include "fmt/ranges.h"

#include <iterator>
#include <memory>
#include <vector>

namespace MACE::inline Reconstruction::MMSTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
class FinderBase {
public:
    using Hit = AHit;
    using Track = ATrack;

public:
    FinderBase();
    virtual ~FinderBase() = 0;

    auto MinNHit() const -> auto { return fMinNHit; }
    auto MinNHit(int n) -> void { fMinNHit = std::max(1, n); }

protected:
    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
    struct Result {
        struct GoodTrack {
            std::vector<AHitPointer> hitData;
            std::shared_ptr<Mustard::Data::Tuple<ATrack>> seed;
        };
        muc::flat_hash_map<int, GoodTrack> good;
        std::vector<AHitPointer> garbage;
    };

protected:
    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
    static auto GoodHitData(const std::vector<AHitPointer>& hitData) -> bool;

private:
    int fMinNHit;
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder

#include "MACE/Reconstruction/MMSTracking/Finder/FinderBase.inl"
