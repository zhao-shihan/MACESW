#pragma once

#include "MACE/PhaseI/Data/SensorRawHit.h++"

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

namespace MACE::PhaseI::Data {

namespace internal {

using HitEventIDHitID = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "EvtID", "Event ID">,
    Mustard::Data::Value<int, "HitID", "Hit ID">>;

} // namespace internal

using SciFiSiPMHit = Mustard::Data::TupleModel<
    internal::HitEventIDHitID,
    SciFiSiPMRawHit>;

} // namespace MACE::PhaseI::Data
