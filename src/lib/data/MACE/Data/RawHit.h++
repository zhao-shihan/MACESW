// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

namespace MACE::Data {

using CDCRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "CellID", "Hit cell ID">,
    Mustard::Data::Value<double, "t", "Signal time">,
    Mustard::Data::Value<float, "Edep", "Energy deposition">>;

using TTCRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "TileID", "Hit detector ID">,
    Mustard::Data::Value<double, "t", "Hit time">,
    Mustard::Data::Value<float, "Edep", "Energy deposition">>;

using MCPRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<double, "t", "Hit time">,
    Mustard::Data::Value<muc::array2f, "x", "Hit position">>;

using ECALRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "ModID", "Hit module ID">,
    Mustard::Data::Value<double, "t", "Hit time">,
    Mustard::Data::Value<float, "Edep", "Energy deposition">>;

using MRPCRawHit = Mustard::Data::TupleModel<
    Mustard::Data::Value<short, "ModID", "Hit module ID">,
    Mustard::Data::Value<double, "t", "Hit time">,
    Mustard::Data::Value<muc::array3f, "x", "Hit position">>;

} // namespace MACE::Data
