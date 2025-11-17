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
#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitterBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "KalmanFitter.h"

#include "muc/array"
#include "muc/math"

#include <cmath>
#include <concepts>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace MACE::inline Reconstruction::MMSTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit = Data::CDCHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack = Data::MMSTrack>
class GenFitReferenceKalmanFitter : public GenFitterBase<AHit, ATrack, genfit::KalmanFitter> {
private:
    using Base = GenFitterBase<AHit, ATrack, genfit::DAF>;

public:
    using Hit = AHit;
    using Track = ATrack;

public:
    GenFitReferenceKalmanFitter(double driftErrorRMS);
    virtual ~GenFitReferenceKalmanFitter() = default;

    template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
        requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
                 Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
    auto operator()(const std::vector<AHitPointer>& hitData, ASeedPointer seed) -> Base::template Result<AHitPointer>;
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter

#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitReferenceKalmanFitter.inl"
