#pragma once

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/FinderBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/IO/PrettyLog.h++"

#include "CLHEP/Vector/ThreeVector.h"

#include "muc/algorithm"
#include "muc/hash_set"

#include <algorithm>
#include <iterator>
#include <ranges>

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiSimHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
class TruthFinder : public FinderBase<AHit, ATrack> {
private:
    using Base = FinderBase<AHit, ATrack>;

public:
    TruthFinder();
    virtual ~TruthFinder() override = default;

    auto MaxVertexRxy() const -> auto { return fMaxVertexRxy; }
    auto MaxVertexRxy(double r) -> void { fMaxVertexRxy = std::max(0., r); }

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
    auto operator()(const std::vector<AHitPointer>& hitData, int = {}) const -> Base::template Result<AHitPointer>;

private:
    double fMaxVertexRxy;
};

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder

#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/TruthFinder.inl"
