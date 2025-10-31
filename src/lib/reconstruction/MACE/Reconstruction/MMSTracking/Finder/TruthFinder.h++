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
