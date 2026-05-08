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

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/FinderBase.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Math/GeometryRepresentation.h++"
#include "Mustard/Math/POCA.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Minuit2/Minuit2Minimizer.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "Eigen/Core"

#include "muc/algorithm"
#include "muc/array"
#include "muc/hash_set"

#include <Eigen/Dense>
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <ranges>
#include <tuple>

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
class GenFitFinder : public FinderBase<ASciFiHit, ATrack> {
private:
    using Base = FinderBase<ASciFiHit, ATrack>;

public:
    GenFitFinder();
    ~GenFitFinder() override = default;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto operator()(std::vector<AHitPointer>& hitData, int nextTrackID) -> Base::template Result<AHitPointer>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto ClusterHits(std::vector<AHitPointer>& hitData) -> const std::vector<std::vector<AHitPointer>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto DivideHits(const std::vector<std::vector<AHitPointer>>& hitData)
        -> const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto FindCompatibleClusterCombinations(const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>>& hitData)
        -> const std::set<std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto CalCoordinates(const std::set<std::vector<std::vector<AHitPointer>>>& hitData, const muc::array3d& direction)
        -> const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto DividePoints(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
        -> const std::vector<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto EstimateInitialDirection(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
        -> std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto AddStraightFibersByTheta(
        const std::vector<std::vector<AHitPointer>>& currentClusters,
        const std::vector<std::vector<AHitPointer>>& axialClusters,
        double angleThreshold,
        double timeThreshold) const -> std::vector<std::vector<AHitPointer>>;

    auto FindHLMinDistanceSquare(
        double helixR, double helixB, double rotationAngle,
        const muc::array3d line1Point, const muc::array3d line1Dir,
        double initialT, double initialTheta) -> std::tuple<double, double, double>;

    auto FindLLMinDistanceSquare(
        const muc::array3d line1Point, const muc::array3d line1Dir,
        const muc::array3d line2Point, const muc::array3d line2Dir) -> double;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto TrackFit(std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
        -> std::shared_ptr<Mustard::Data::Tuple<ATrack>>;

private:
    double fFirstSegmentMaxDeltaPhi;
    double fFirstSegmentMinNHit;
    double fMomentumSeed;
};

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder

#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/GenFitFinder.inl"
