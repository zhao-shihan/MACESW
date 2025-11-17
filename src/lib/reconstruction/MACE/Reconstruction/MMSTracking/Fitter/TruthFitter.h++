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
#include "MACE/Reconstruction/MMSTracking/Fitter/FitterBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/IO/PrettyLog.h++"

#include <concepts>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace MACE::inline Reconstruction::MMSTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<Data::CDCSimHit> AHit = Data::CDCSimHit,
         Mustard::Data::SuperTupleModel<Data::MMSSimTrack> ATrack = Data::MMSSimTrack>
class TruthFitter : public FitterBase<AHit, ATrack> {
public:
    using Hit = AHit;
    using Track = ATrack;

public:
    virtual ~TruthFitter() = default;

    auto CheckHitDataConsistency() const -> auto { return fCheckHitDataConsistency; }
    auto CheckHitDataConsistency(bool val) -> void { fCheckHitDataConsistency = val; }

    template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
        requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
                 Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
    auto operator()(const std::vector<AHitPointer>& hitData, ASeedPointer seed) -> std::shared_ptr<Mustard::Data::Tuple<ATrack>>;

private:
    bool fCheckHitDataConsistency{true};
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter

#include "MACE/Reconstruction/MMSTracking/Fitter/TruthFitter.inl"
