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

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Data/Value.h++"
#include "Mustard/Math/Norm.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "muc/array"
#include "muc/math"

#include <cmath>
#include <vector>

namespace MACE::Data {

using MMSTrack = Mustard::Data::TupleModel<
    Mustard::Data::Value<int, "EvtID", "Event ID">,
    Mustard::Data::Value<int, "TrkID", "Track ID">,
    Mustard::Data::Value<std::vector<int>, "HitID", "Hit(ID)s in this track">,
    Mustard::Data::Value<float, "chi2", "Goodness of fit (chi^{2})">,
    Mustard::Data::Value<double, "t0", "Vertex time">,
    // vertex information
    Mustard::Data::Value<int, "PDGID", "Particle PDG ID">,
    Mustard::Data::Value<muc::array3f, "x0", "Vertex position">,
    Mustard::Data::Value<float, "Ek0", "Vertex kinetic energy">,
    Mustard::Data::Value<muc::array3f, "p0", "Vertex momentum">,
    // helix information
    Mustard::Data::Value<muc::array2f, "c0", "Transverse center">,
    Mustard::Data::Value<float, "r0", "Transverse radius">,
    Mustard::Data::Value<float, "phi0", "Vertex azimuth angle">,
    Mustard::Data::Value<float, "z0", "Vertex z coordinate">,
    Mustard::Data::Value<float, "theta0", "Reference zenith angle">>;

using MMSSimTrack = Mustard::Data::TupleModel<
    MMSTrack,
    Mustard::Data::Value<std::string, "CreatProc", "Track creator process (MC truth)">>;

/// @brief Calculate helix information from known vertex information in-place.
/// @param track The track
/// @param magneticFluxDensity Magnetic field B0
constexpr auto CalculateHelix(Mustard::Data::SuperTuple<Mustard::Data::Tuple<MMSTrack>> auto& track, double magneticFluxDensity) -> void {
    using namespace Mustard::VectorArithmeticOperator;
    using Mustard::PhysicalConstant::c_light;

    const auto charge{Get<"PDGID">(track) > 0 ? -1 : 1};
    const auto x0{Get<"x0">(track).template As<muc::array3d>()};
    const auto p0{Get<"p0">(track).template As<muc::array3d>()};

    const auto absPXY{muc::hypot(p0[0], p0[1])};
    const auto pXY{charge < 0 ? absPXY : -absPXY};
    const auto r0{pXY / (-charge * magneticFluxDensity * c_light)};
    const muc::array2d x0Local{r0 * (p0[1] / pXY),
                               r0 * (-p0[0] / pXY)};
    const auto phi0{std::atan2(-x0Local[1], -x0Local[0])};
    const muc::array2d c0{x0[0] - x0Local[0],
                          x0[1] - x0Local[1]};

    // const auto absDeltaPhi{std::acos(-c0 * x0Local) / std::sqrt(Mustard::Math::NormSq(c0) * Mustard::Math::NormSq(x0Local))};
    // const auto deltaPhi{x0Local[0] * c0[1] - c0[0] * x0Local[1] > 0 ? absDeltaPhi : -absDeltaPhi};
    const auto theta0{std::atan2(pXY, p0[2])};
    // const auto z0{x0[3] - r0 * deltaPhi / std::tan(theta0)};
    const auto z0{x0[2]};

    Get<"c0">(track) = c0;
    Get<"r0">(track) = r0;
    Get<"phi0">(track) = phi0;
    Get<"z0">(track) = z0;
    Get<"theta0">(track) = theta0;
}

/// @brief Calculate vertex information from known helix information in-place.
/// @param track The track
/// @param magneticFluxDensity Magnetic field B0
constexpr auto CalculateVertex(Mustard::Data::SuperTuple<Mustard::Data::Tuple<MMSTrack>> auto& track, double magneticFluxDensity) -> void {
    using Mustard::PhysicalConstant::c_light;
    using Mustard::PhysicalConstant::electron_mass_c2;

    const auto c0{Get<"c0">(track)};
    const auto r0{Get<"r0">(track)};
    const auto phi0{Get<"phi0">(track)};
    const auto z0{Get<"z0">(track)};
    const auto theta0{Get<"theta0">(track)};

    const auto charge{theta0 > 0 ? -1 : 1};
    const auto pdgID{charge < 0 ? 11 : -11};

    const auto cos0{std::cos(phi0)};
    const auto sin0{std::sin(phi0)};
    const muc::array3d x0{c0[0] + r0 * cos0,
                          c0[1] + r0 * sin0,
                          z0};

    const auto pXY{-charge * magneticFluxDensity * c_light * r0};
    const auto pZ{pXY / std::tan(theta0)};
    const auto ek0{std::sqrt(muc::hypot_sq(pXY, pZ) + muc::pow(electron_mass_c2, 2)) - electron_mass_c2};
    const muc::array3d p0{-pXY * sin0,
                          pXY * cos0,
                          pZ};

    Get<"PDGID">(track) = pdgID;
    Get<"x0">(track) = x0;
    Get<"Ek0">(track) = ek0;
    Get<"p0">(track) = p0;
}

} // namespace MACE::Data
