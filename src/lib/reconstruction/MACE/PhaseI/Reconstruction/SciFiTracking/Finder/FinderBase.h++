#pragma once

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"

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

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {
template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
class FinderBase {
public:
    using SciFiHit = ASciFiHit;
    using Track = ATrack;

public:
    FinderBase();
    virtual ~FinderBase() = 0;

    auto MinNHit() const -> auto { return fMinNHit; }
    auto MinNHit(int n) -> void { fMinNHit = std::max(1, n); }

protected:
    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
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
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    static auto GoodHitData(const std::vector<AHitPointer>& hitData) -> bool;

private:
    int fMinNHit;
};
} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder

#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/FinderBase.inl"
