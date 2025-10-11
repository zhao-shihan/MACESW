#pragma once

#include "MACE/Data/RawHit.h++"

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

namespace MACE::Data {

namespace internal {

using HitEventIDHitID = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "EvtID", "Event ID">,
    Mustard::Data::Value<int, "HitID", "Hit ID">>;

} // namespace internal

using CDCHit = Mustard::Data::TupleModel<
    internal::HitEventIDHitID,
    CDCRawHit,
    Mustard::Data::Value<float, "d", "Drift distance">,
    Mustard::Data::Value<bool, "Good", "Good flag">>;

using TTCHit = Mustard::Data::TupleModel<
    internal::HitEventIDHitID,
    TTCRawHit,
    Mustard::Data::Value<bool, "Good", "Good flag">,
    Mustard::Data::Value<std::vector<float>, "ADC", "SiPM readout amplitude">>;

using MCPHit = Mustard::Data::TupleModel<
    internal::HitEventIDHitID,
    MCPRawHit>;

using ECALHit = Mustard::Data::TupleModel<
    internal::HitEventIDHitID,
    ECALRawHit>;

using MRPCHit = Mustard::Data::TupleModel<
    internal::HitEventIDHitID,
    MRPCRawHit>;

} // namespace MACE::Data
