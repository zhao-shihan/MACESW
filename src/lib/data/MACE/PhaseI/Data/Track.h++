#pragma once

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

namespace MACE::PhaseI::Data {

using ReconTrack = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "EvtID", "Event ID">,
    Mustard::Data::Value<muc::array3f, "x", "Hit position">,
    Mustard::Data::Value<muc::array3f, "p", "Momentum vector">,
    Mustard::Data::Value<double, "t", "Optical photon hit time">,
    Mustard::Data::Value<double, "chi2", "Chi2 of fit">>;

} // namespace MACE::PhaseI::Data
