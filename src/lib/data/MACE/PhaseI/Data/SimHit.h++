#pragma once

#include "MACE/PhaseI/Data/Hit.h++"

#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"

namespace MACE::PhaseI::Data {

namespace internal {

using SimHitVertexTruth = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "TrkID", "MC Track ID">,
    Mustard::Data::Value<int, "MotherID", "MC Mother Track ID">,
    Mustard::Data::Value<int, "PDGID", "Particle PDG ID (MC truth)">,
    Mustard::Data::Value<double, "t0", "Vertex time (MC truth)">,
    Mustard::Data::Value<muc::array3f, "x0", "Vertex position (MC truth)">,
    Mustard::Data::Value<float, "Ek0", "Vertex kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p0", "Vertex momentum (MC truth)">,
    Mustard::Data::Value<std::string, "CreatProc", "Track creator process (MC truth)">>;

} // namespace internal

using SciFiSimHit = Mustard::Data::TupleModel<
    SciFiHit,
    Mustard::Data::Value<int, "nOptPho", "Number of optical photon hits on SiPM (MC truth)">,
    Mustard::Data::Value<muc::array3f, "x", "Hit position (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum (MC truth)">,
    Mustard::Data::Value<double, "Ek", "Kinetic energy in hit position">,
    internal::SimHitVertexTruth>;

using MRPCSimHit = Mustard::Data::TupleModel<
    MRPCHit,
    Mustard::Data::Value<bool, "Trig", "Trigger flag">,
    Mustard::Data::Value<float, "Edep", "Energy deposition">,
    Mustard::Data::Value<float, "Ek", "Hit kinetic energy (MC truth)">,
    Mustard::Data::Value<muc::array3f, "p", "Hit momentum (MC truth)">,
    internal::SimHitVertexTruth>;

} // namespace MACE::PhaseI::Data
