
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

namespace MACE::inline Algorithm {

auto AsHelix(const Mustard::Data::SuperTuple<HPTrackState> auto& track) -> Mustard::Helix {
    return {Get<"c", Mustard::Point2D>(track),
            Get<"r", double>(track),
            Get<"phi0", double>(track),
            static_cast<double>(Get<"x0">(track)[2]),
            Get<"lambda", double>(track)};
}

auto UpdateKinematicState(Mustard::Data::SuperTuple<HPTrackState> auto& track, double nominalB) -> void {
    using Mustard::PhysicalConstant::c_light;

    const auto r{Get<"r", double>(track)};
    const auto [sinPhi0, cosPhi0]{muc::sincos(Get<"phi0", double>(track))};
    if (not std::isinf(r)) { // when r is infinite, x0 and y0 cannot be retrieved; keep them unchanged
        const auto& [cX, cY]{Get<"c">(track)};
        Get<"x0">(track)[0] = cX + r * cosPhi0;
        Get<"x0">(track)[1] = cY + r * sinPhi0;
    }

    const auto [sinLambda, cosLambda]{muc::sincos(Get<"lambda", double>(track))};
    Get<"d0">(track) = {-sinPhi0 * sinLambda,
                        cosPhi0 * sinLambda,
                        cosLambda};
    Get<"p0">(track) = std::abs(Get<"q">(track) * nominalB * r * c_light);
}

auto UpdateHelixState(Mustard::Data::SuperTuple<HPTrackState> auto& track, double nominalB) -> void {
    using Mustard::PhysicalConstant::c_light;

    const auto [d0x, d0y, cosLambda]{Get<"d0", muc::array3d>(track)};
    const auto q{Get<"q">(track)};
    const auto sinLambda{(q < 0) xor (nominalB > 0) ?
                             +muc::hypot(d0x, d0y) :
                             -muc::hypot(d0x, d0y)}; // negatively charged particle cycles right-handed in positive B field

    const auto p0T{Get<"p0">(track) * sinLambda}; // may be +infinity
    const auto r{std::abs(p0T / (q * nominalB * c_light))};

    const auto cosPhi0{d0y / sinLambda};
    const auto sinPhi0{-d0x / sinLambda};

    Get<"c">(track) = {Get<"x0">(track)[0] - r * cosPhi0,
                       Get<"x0">(track)[1] - r * sinPhi0};
    Get<"r">(track) = r;
    Get<"phi0">(track) = std::atan2(sinPhi0, cosPhi0);
    Get<"lambda">(track) = std::atan2(sinLambda, cosLambda);
}

} // namespace MACE::inline Algorithm
