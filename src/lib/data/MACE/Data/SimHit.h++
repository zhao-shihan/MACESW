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

#include "MACE/Data/Hit.h++"

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

#include "muc/array"

#include <string>

namespace MACE::Data {

namespace internal {

using SimHitVertexTruth = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "TrkID", "MC Track ID">,
    Mustard::Data::Value<int, "PDGID", "Particle PDG ID (MC truth)">,
    Mustard::Data::Value<double, "t0", "Vertex time (MC truth)">,
    Mustard::Data::Value<muc::array3f, "x0", "Vertex position (MC truth)">,
    Mustard::Data::Value<float, "Ek0", "Vertex kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p0", "Vertex momentum (MC truth)">,
    Mustard::Data::Value<std::string, "CreatProc", "Track creator process (MC truth)">>;

} // namespace internal

using CDCSimHit = Mustard::Data::TupleModel<
    CDCHit,
    Mustard::Data::Value<double, "tHit", "Hit time (MC truth)">,
    Mustard::Data::Value<muc::array3f, "x", "Hit position (MC truth)">,
    Mustard::Data::Value<float, "Ek", "Hit kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum (MC truth)">,
    internal::SimHitVertexTruth>;

using TTCSimHit = Mustard::Data::TupleModel<
    TTCHit,
    Mustard::Data::Value<std::vector<int>, "nOptPho", "Number of optical photon hits on SiPM (MC truth)">,
    Mustard::Data::Value<muc::array3f, "x", "Hit position (MC truth)">,
    Mustard::Data::Value<float, "Ek", "Hit kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum (MC truth)">,
    internal::SimHitVertexTruth>;

using MCPSimHit = Mustard::Data::TupleModel<
    MCPHit,
    Mustard::Data::Value<bool, "Trig", "Trigger flag">,
    Mustard::Data::Value<float, "Edep", "Energy deposition">,
    Mustard::Data::Value<float, "Ek", "Hit kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum (MC truth)">,
    internal::SimHitVertexTruth>;

using ECALSimHit = Mustard::Data::TupleModel<
    ECALHit,
    Mustard::Data::Value<int, "nOptPho", "Number of optical photon hits on PM (MC truth)">,
    Mustard::Data::Value<muc::array3f, "x", "Hit position (MC truth)">,
    Mustard::Data::Value<float, "Ek", "Hit kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum (MC truth)">,
    internal::SimHitVertexTruth>;

using MRPCSimHit = Mustard::Data::TupleModel<
    MRPCHit,
    Mustard::Data::Value<bool, "Trig", "Trigger flag">,
    Mustard::Data::Value<float, "Edep", "Energy deposition">,
    Mustard::Data::Value<float, "Ek", "Hit kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum (MC truth)">,
    internal::SimHitVertexTruth>;

} // namespace MACE::Data
