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
