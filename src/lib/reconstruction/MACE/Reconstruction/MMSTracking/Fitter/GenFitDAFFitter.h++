#pragma once

#include "MACE/Data/Hit.h++"
#include "MACE/Data/MMSTrack.h++"
#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitterBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "DAF.h"

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
class GenFitDAFFitter : public GenFitterBase<AHit, ATrack, genfit::DAF> {
private:
    using Base = GenFitterBase<AHit, ATrack, genfit::DAF>;

public:
    using Hit = AHit;
    using Track = ATrack;

public:
    explicit GenFitDAFFitter(double driftErrorRMS);
    ~GenFitDAFFitter() override = default;

    template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
        requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
                 Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
    auto operator()(const std::vector<AHitPointer>& hitData, const ASeedPointer& seed) -> Base::template Result<AHitPointer>;
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter

#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitDAFFitter.inl"
