#pragma once

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

namespace MACE::PhaseI::Data {

using SciFiSiPMRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "FiberID", "Hit Fiber ID">,
    Mustard::Data::Value<short, "SiPMID", "Hit SiPM ID">,
    Mustard::Data::Value<double, "t", "Optical photon hit time">>;

} // namespace MACE::PhaseI::Data
