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

auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, double deltaS) -> void {
    using Mustard::MathConstant::pi;
    using Mustard::PhysicalConstant::c_light;

    Get<"t">(track) += deltaS / c_light; // assume beta = 1
    if (std::isinf(Get<"r">(track))) {
        auto x0{Get<"x0", Mustard::Point3D>(track)};
        x0 += deltaS * Get<"d0", Mustard::Vector3D>(track);
        Get<"x0">(track) = x0;
        // d0 and phi0 remain unchanged
    } else {
        const auto [c, r, phi0, z0, lambda]{AsHelix(track)};
        const auto [sinLambda, cosLambda]{muc::sincos(lambda)};
        const auto deltaPhi{deltaS * sinLambda / r};
        const auto phi{std::remainder(phi0 + deltaPhi, 2 * pi)}; // use std::remainder, not std::fmod!
        const auto [sinPhi, cosPhi]{muc::sincos(phi)};
        Get<"x0">(track) = {c.x() + r * cosPhi,
                            c.y() + r * sinPhi,
                            z0 + deltaS * cosLambda};
        Get<"d0">(track) = {-sinPhi * sinLambda,
                            cosPhi * sinLambda,
                            cosLambda};
        Get<"phi0">(track) = phi;
    }
}

auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, Mustard::Point2D axis,
                 ExtrapolationMode mode) -> std::optional<ExtrapolationResult> {
    // deal with straight line case first
    if (std::isinf(Get<"r">(track))) {
        const auto x0{Get<"x0", Mustard::Point3D>(track)};
        const auto d0{Get<"d0", Mustard::Vector3D>(track)};
        const Mustard::Point3D axisPoint{axis.x(), axis.y(), 0};
        const Mustard::Vector3D axisDirection{0, 0, 1};
        const auto [poca1, poca2, doca]{Mustard::POCA({x0, d0}, {axisPoint, axisDirection})};
        const auto deltaS{(poca1 - x0).dot(d0)};
        if ((mode == ExtrapolationMode::Forward and deltaS < 0) or
            (mode == ExtrapolationMode::Backward and deltaS > 0)) {
            return std::nullopt;
        }
        Extrapolate(track, deltaS);
        return ExtrapolationResult{doca, deltaS, poca2};
    }

    // compute POCA
    const auto helix{AsHelix(track)};
    const auto [pocaCCw, pocaCw, doca]{Mustard::POCA(helix, axis)};

    // translate POCA to arc length
    const auto deltaZCCw{pocaCCw.z() - helix.z0};
    const auto deltaZCw{pocaCw.z() - helix.z0};
    const auto secLambda{1 / std::cos(helix.lambda)};
    const auto deltaSCCw{deltaZCCw * secLambda};
    const auto deltaSCw{deltaZCw * secLambda};
    const auto ccwFwd{deltaSCCw >= 0};
    const auto& deltaSFwd{ccwFwd ? deltaSCCw : deltaSCw};
    const auto& deltaSBwd{ccwFwd ? deltaSCw : deltaSCCw};
    const auto& pocaFwd{ccwFwd ? pocaCCw : pocaCw};
    const auto& pocaBwd{ccwFwd ? pocaCw : pocaCCw};

    // decide extrapolation direction
    switch (mode) {
    default:
        [[fallthrough]];
    case ExtrapolationMode::Nearest:
        if (deltaSFwd <= -deltaSBwd) {
            mode = ExtrapolationMode::Forward;
        } else {
            mode = ExtrapolationMode::Backward;
        }
        break;
    case ExtrapolationMode::Forward:
        break;
    case ExtrapolationMode::Backward:
        break;
    }

    // perform extrapolation
    if (mode == ExtrapolationMode::Forward) {
        Extrapolate(track, deltaSFwd);
        const Mustard::Point3D poca{axis.x(), axis.y(), pocaFwd.z()};
        return ExtrapolationResult{doca, deltaSFwd, poca};
    } else {
        Extrapolate(track, deltaSBwd);
        const Mustard::Point3D poca{axis.x(), axis.y(), pocaBwd.z()};
        return ExtrapolationResult{doca, deltaSBwd, poca};
    }
}

auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, Mustard::Point3D point,
                 ExtrapolationMode mode, double maxDeltaPhi, int nTrialPts,
                 int maxIter, double absTol, double relTol) -> std::optional<ExtrapolationResult> {
    // deal with straight line case first
    if (std::isinf(Get<"r">(track))) {
        const auto deltaX{point - Get<"x0", Mustard::Point3D>(track)};
        const auto deltaS{deltaX.dot(Get<"d0", Mustard::Vector3D>(track))};
        if ((mode == ExtrapolationMode::Forward and deltaS < 0) or
            (mode == ExtrapolationMode::Backward and deltaS > 0)) {
            return std::nullopt;
        }
        const auto doca{std::sqrt(deltaX.mag2() - muc::pow(deltaS, 2))};
        Extrapolate(track, deltaS);
        return ExtrapolationResult{doca, deltaS, point};
    }

    // determine phi interval
    const auto helix{AsHelix(track)};
    const auto ccwFwd{helix.lambda >= 0};
    std::pair<double, double> interval;
    switch (mode) {
    default:
        [[fallthrough]];
    case ExtrapolationMode::Nearest:
        interval = {-maxDeltaPhi, maxDeltaPhi};
        break;
    case ExtrapolationMode::Forward:
        [[fallthrough]];
    case ExtrapolationMode::Backward:
        if ((mode == ExtrapolationMode::Forward) == ccwFwd) {
            interval = {0, maxDeltaPhi};
        } else {
            interval = {-maxDeltaPhi, 0};
        }
        break;
    }

    // compute POCA
    const auto pocaResult{Mustard::POCA(helix, point, interval.first, interval.second,
                                        nTrialPts, maxIter, absTol, relTol)};
    if (not pocaResult.has_value()) {
        return std::nullopt;
    }

    // perform extrapolation
    const auto& [poca, doca]{*pocaResult};
    const auto deltaZ{poca.z() - helix.z0};
    const auto deltaS{deltaZ / std::cos(helix.lambda)};
    Extrapolate(track, deltaS);
    return ExtrapolationResult{doca, deltaS, point};
}

auto Extrapolate(Mustard::Data::SuperTuple<HPTrackState> auto& track, Mustard::Line3D line,
                 ExtrapolationMode mode, double maxDeltaPhi, int nTrialPts,
                 int maxIter, double absTol, double relTol) -> std::optional<ExtrapolationResult> {
    // deal with straight line case first
    if (std::isinf(Get<"r">(track))) {
        const auto x0{Get<"x0", Mustard::Point3D>(track)};
        const auto d0{Get<"d0", Mustard::Vector3D>(track)};
        const auto [poca1, poca2, doca]{Mustard::POCA({x0, d0}, line)};
        const auto deltaS{(poca1 - x0).dot(d0)};
        if ((mode == ExtrapolationMode::Forward and deltaS < 0) or
            (mode == ExtrapolationMode::Backward and deltaS > 0)) {
            return std::nullopt;
        }
        Extrapolate(track, deltaS);
        return ExtrapolationResult{doca, deltaS, poca2};
    }

    // determine phi interval
    const auto helix{AsHelix(track)};
    const auto ccwFwd{helix.lambda >= 0};
    std::pair<double, double> interval;
    switch (mode) {
    default:
        [[fallthrough]];
    case ExtrapolationMode::Nearest:
        interval = {-maxDeltaPhi, maxDeltaPhi};
        break;
    case ExtrapolationMode::Forward:
        [[fallthrough]];
    case ExtrapolationMode::Backward:
        if ((mode == ExtrapolationMode::Forward) == ccwFwd) {
            interval = {0, maxDeltaPhi};
        } else {
            interval = {-maxDeltaPhi, 0};
        }
        break;
    }

    // compute POCA
    const auto pocaResult{Mustard::POCA(helix, line, interval.first, interval.second,
                                        nTrialPts, maxIter, absTol, relTol)};
    if (not pocaResult.has_value()) {
        return std::nullopt;
    }

    // perform extrapolation
    const auto& [poca1, poca2, doca]{*pocaResult};
    const auto deltaZ{poca1.z() - helix.z0};
    const auto deltaS{deltaZ / std::cos(helix.lambda)};
    Extrapolate(track, deltaS);
    return ExtrapolationResult{doca, deltaS, poca2};
}

} // namespace MACE::inline Algorithm
