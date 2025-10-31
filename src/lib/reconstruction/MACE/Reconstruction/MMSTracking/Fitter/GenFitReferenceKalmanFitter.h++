#pragma once

#include "MACE/Data/Hit.h++"
#include "MACE/Data/MMSTrack.h++"
#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitterBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "KalmanFitterRefTrack.h"

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
class GenFitReferenceKalmanFitter : public GenFitterBase<AHit, ATrack, genfit::KalmanFitterRefTrack> {
private:
    using Base = GenFitterBase<AHit, ATrack, genfit::KalmanFitterRefTrack>;

public:
    using Hit = AHit;
    using Track = ATrack;

public:
    explicit GenFitReferenceKalmanFitter(double driftErrorRMS);
    virtual ~GenFitReferenceKalmanFitter() = default;

    template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
        requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
                 Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
    auto operator()(const std::vector<AHitPointer>& hitData, ASeedPointer seed) -> Base::template Result<AHitPointer>;
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter

#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitReferenceKalmanFitter.inl"
