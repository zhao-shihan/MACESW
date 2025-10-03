#pragma once

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitterBase.h++"

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

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
class GenFitDAFFitter : public GenFitterBase<AHit, ATrack, genfit::DAF> {
private:
    using Base = GenFitterBase<AHit, ATrack, genfit::DAF>;

public:
    using Hit = AHit;
    using Track = ATrack;

public:
    GenFitDAFFitter(double fiberRMS);
    virtual ~GenFitDAFFitter() = default;

    template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
        requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
                 Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
    auto operator()(const std::vector<AHitPointer>& hitData, ASeedPointer seed) -> Base::template Result<AHitPointer>;
};
} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter

#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitDAFFitter.inl"
