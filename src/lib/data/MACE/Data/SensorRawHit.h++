#pragma once

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

namespace MACE::Data {

using TTCSiPMRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "TileID", "Hit detector ID">,
    Mustard::Data::Value<short, "SiPMID", "Hit Silicone ID">,
    Mustard::Data::Value<double, "t", "Optical photon hit time">,
    Mustard::Data::Value<muc::array2f, "x", "Relative hit position on the sensor">,
    Mustard::Data::Value<muc::array3f, "k", "Wave vector of the photon">>;

using ECALPMRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "ModID", "Hit module ID">,
    Mustard::Data::Value<double, "t", "Optical photon hit time">>;

} // namespace MACE::Data
