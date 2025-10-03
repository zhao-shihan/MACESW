#pragma once

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/FitterBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/IO/PrettyLog.h++"

#include <concepts>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiSimHit> AHit = MACE::PhaseI::Data::SciFiSimHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack = MACE::PhaseI::Data::Track>
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

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter

#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/TruthFitter.inl"
