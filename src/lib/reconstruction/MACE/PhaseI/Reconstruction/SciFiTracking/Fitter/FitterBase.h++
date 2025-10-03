#pragma once

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"

#include <memory>
#include <vector>

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
class FitterBase {
public:
    using Hit = AHit;
    using Track = ATrack;

public:
    virtual ~FitterBase() = default;

protected:
    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
    struct Result {
        std::shared_ptr<Mustard::Data::Tuple<ATrack>> track;
        std::vector<AHitPointer> fitted;
        std::vector<AHitPointer> failed;
    };
};
} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter
