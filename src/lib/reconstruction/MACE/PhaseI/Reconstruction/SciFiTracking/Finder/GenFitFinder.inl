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

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
GenFitFinder<ASciFiHit, ATrack>::GenFitFinder() :
    Base{},
    fFirstSegmentMaxDeltaPhi{},
    fFirstSegmentMinNHit{},
    fMomentumSeed{} {}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::operator()(std::vector<AHitPointer>& hitData, int nextTrackID) -> Base::template Result<AHitPointer> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    using Result = Base::template Result<AHitPointer>;
    Result r{};

    const auto clusterLists{this->ClusterHits(hitData)};
    const auto dividedClusters{this->DivideHits(clusterLists)};
    const auto hitLists{this->FindCompatibleClusterCombinations(dividedClusters)};

    // Initial coordinates without direction correction.
    auto coordinateMap{this->CalCoordinates(hitLists, muc::array3d{})};
    auto dividedPointMap{this->DividePoints(coordinateMap)};

    // Keep the initial groups and update each group independently in iterations.
    const auto initialDivided{dividedPointMap};

    struct Candidate {
        std::vector<int> signature{};
        std::vector<AHitPointer> hitData{};
        std::shared_ptr<Mustard::Data::Tuple<ATrack>> seed{};
        double alignment{};
    };
    std::vector<Candidate> candidates{};
    for (const auto& initialCluster : initialDivided) {
        auto initialResult{this->EstimateInitialDirection(initialCluster)};
        muc::array3d preDir{std::get<0>(initialResult)};

        constexpr int nIter{10};
        const double minDirDotThreshold{sciFiTracker.MinDirDotThreshold()};
        for (int iter = 0; iter < nIter; ++iter) {
            coordinateMap = this->CalCoordinates(hitLists, preDir);
            dividedPointMap = this->DividePoints(coordinateMap);
            muc::array3d initialCentroid{std::get<1>(initialResult)};

            auto bestIt{std::ranges::min_element(dividedPointMap,
                                                 std::ranges::less{},
                                                 [&](const auto& group) -> double {
                                                     auto [_1, c, _2] = this->EstimateInitialDirection(group);
                                                     return (c[0] - initialCentroid[0]) * (c[0] - initialCentroid[0]) + (c[1] - initialCentroid[1]) * (c[1] - initialCentroid[1]) + (c[2] - initialCentroid[2]) * (c[2] - initialCentroid[2]);
                                                 })};

            if (bestIt != dividedPointMap.end()) {
                const auto nextResult{this->EstimateInitialDirection(*bestIt)};
                const auto& currentDir{std::get<0>(nextResult)};
                const double dirDot{preDir[0] * currentDir[0] + preDir[1] * currentDir[1] + preDir[2] * currentDir[2]};

                if (dirDot < minDirDotThreshold) {
                    break;
                }

                initialResult = nextResult;
                preDir = currentDir;
            }
        }

        auto& currentClusters{std::get<2>(initialResult)};
        auto extraAxial{this->AddStraightFibersByTheta(
            currentClusters,
            std::get<2>(dividedClusters),
            sciFiTracker.CentroidThetaThreshold(),
            sciFiTracker.ThresholdTime())};
        if (not extraAxial.empty()) {
            currentClusters.insert(currentClusters.end(), extraAxial.begin(), extraAxial.end());
        }

        std::vector<int> signature{};
        for (const auto& cluster : currentClusters) {
            for (const auto& hit : cluster) {
                signature.push_back(Get<"FiberID">(*hit));
            }
        }
        std::ranges::sort(signature);
        signature.erase(std::ranges::unique(signature).end(), signature.end());
        if (signature.empty()) {
            continue;
        }

        auto result{this->TrackFit(initialResult)};

        std::vector<AHitPointer> resultData{};
        for (auto&& cluster : std::get<2>(initialResult)) {
            resultData.insert(resultData.end(), cluster.begin(), cluster.end());
        }

        const auto& x{Get<"x">(*result)};
        const auto& p{Get<"p">(*result)};
        const double xNorm{muc::hypot(x[0], x[1], x[2])};
        const double pNorm{muc::hypot(p[0], p[1], p[2])};
        const double alignment{xNorm > 1e-12 and pNorm > 1e-12 ?
                                   (x[0] * p[0] + x[1] * p[1] + x[2] * p[2]) / (xNorm * pNorm) :
                                   -1};

        candidates.push_back({std::move(signature),
                              std::move(resultData),
                              std::move(result),
                              alignment});
    }

    std::ranges::sort(candidates,
                      [](const auto& lhs, const auto& rhs) {
                          if (std::abs(lhs.alignment - rhs.alignment) > 1e-9) {
                              return lhs.alignment > rhs.alignment;
                          }
                          return lhs.signature.size() > rhs.signature.size();
                      });

    auto overlapFraction{[](const std::vector<int>& lhs, const std::vector<int>& rhs) -> double {
        if (lhs.empty() or rhs.empty()) {
            return 0;
        }

        size_t i{};
        size_t j{};
        size_t intersection{};
        while (i < lhs.size() and j < rhs.size()) {
            if (lhs[i] == rhs[j]) {
                ++intersection;
                ++i;
                ++j;
            } else if (lhs[i] < rhs[j]) {
                ++i;
            } else {
                ++j;
            }
        }

        const auto denominator{std::min(lhs.size(), rhs.size())};
        return static_cast<double>(intersection) / static_cast<double>(denominator);
    }};

    std::vector<std::vector<int>> acceptedSignatures{};
    for (const auto& candidate : candidates) {
        const bool largeOverlap{std::ranges::any_of(
            acceptedSignatures,
            [&](const auto& signature) {
                return overlapFraction(candidate.signature, signature) > 0.5;
            })};
        if (largeOverlap) {
            continue;
        }

        acceptedSignatures.push_back(candidate.signature);
        r.good[nextTrackID] = {candidate.hitData, candidate.seed};
        ++nextTrackID;
    }

    return r;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::ClusterHits(std::vector<AHitPointer>& hitData) -> std::vector<std::vector<AHitPointer>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};

    std::vector<std::vector<AHitPointer>> clusterList{};

    muc::timsort(hitData,
                 [](auto&& hit1, auto&& hit2) {
                     return std::tie(Get<"FiberID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"FiberID">(*hit2), Get<"t">(*hit2));
                 });

    for (auto&& hit : hitData) {
        const auto cluster{std::ranges::find_if(
            clusterList,
            [&](auto&& cluster) {
                return std::ranges::any_of(cluster, [&](auto&& element) {
                    return std::abs(Get<"t">(*hit) - Get<"t">(*element)) < sciFiTracker.ThresholdTime() and
                           fiberMap[Get<"FiberID">(*hit)].layerID / 2 == fiberMap[Get<"FiberID">(*element)].layerID / 2 and
                           std::abs(fiberMap[Get<"FiberID">(*hit)].localID -
                                    fiberMap[Get<"FiberID">(*element)].localID) <= sciFiTracker.ClusterLength();
                });
            })};
        if (cluster != clusterList.end()) {
            cluster->emplace_back(hit);
        } else {
            clusterList.emplace_back().emplace_back(hit);
        }
    }

    return clusterList;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::DivideHits(const std::vector<std::vector<AHitPointer>>& hitData)
    -> std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};

    std::vector<std::vector<AHitPointer>> lCluster{};
    std::vector<std::vector<AHitPointer>> rCluster{};
    std::vector<std::vector<AHitPointer>> aCluster{};

    for (auto&& cluster : hitData) {
        const auto firstSiPMID{Get<"FiberID">(*cluster.front())};
        const auto layerType{fiberMap[firstSiPMID].layerType};

        if (layerType == "LHelical") {
            lCluster.push_back(cluster);
        } else if (layerType == "RHelical") {
            rCluster.push_back(cluster);
        } else {
            aCluster.push_back(cluster);
        }
    }
    return std::tuple{lCluster, rCluster, aCluster};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitFinder<ASciFiHit, ATrack>::SuperLayer(int fiberID) -> int {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};
    return static_cast<int>(fiberMap[fiberID].layerID / 2);
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitFinder<ASciFiHit, ATrack>::AdjacentPair(int lhsID, int rhsID) -> bool {
    return std::abs(SuperLayer(lhsID) - SuperLayer(rhsID)) <= 1;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitFinder<ASciFiHit, ATrack>::CompatibleTriple(int lID, int rID, int aID) -> bool {
    const int gL = SuperLayer(lID);
    const int gR = SuperLayer(rID);
    const int gA = SuperLayer(aID);
    const int minLR = std::min(gL, gR);
    const int maxLR = std::max(gL, gR);
    const int lrGap = maxLR - minLR;
    if (lrGap != 2) {
        return false;
    }
    return (gA > minLR and gA < maxLR);
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::BuildTripleCandidates(
    const std::vector<std::vector<AHitPointer>>& lCluster,
    const std::vector<std::vector<AHitPointer>>& rCluster,
    const std::vector<std::vector<AHitPointer>>& aCluster,
    const ClusterStats& lStats,
    const ClusterStats& rStats,
    const ClusterStats& aStats) -> std::vector<TripleCandidate> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<TripleCandidate> candidates;

    auto normalizeAngle{[&](double angle) -> double {
        angle = std::fmod(angle, 2 * std::numbers::pi);
        if (angle < 0) {
            angle += 2 * std::numbers::pi;
        }
        return angle;
    }};

    auto angularDist{[&](double a, double b) -> double {
        double delta{std::fmod(std::fabs(a - b), 2 * std::numbers::pi)};
        return (delta > std::numbers::pi) ? 2 * std::numbers::pi - delta : delta;
    }};

    for (size_t li{}; li < lCluster.size(); ++li) {
        for (size_t ri{}; ri < rCluster.size(); ++ri) {
            const double s = lStats.avgAngle[li] + rStats.avgAngle[ri];
            const double angleCond1 = normalizeAngle(std::fmod(s, 4 * std::numbers::pi) / 2);
            const double angleCond2 = normalizeAngle(std::fmod(s + 2 * std::numbers::pi, 4 * std::numbers::pi) / 2);
            for (size_t ai{}; ai < aCluster.size(); ++ai) {
                if (std::abs(lStats.avgTime[li] - aStats.avgTime[ai]) >= sciFiTracker.ThresholdTime() or
                    std::abs(rStats.avgTime[ri] - aStats.avgTime[ai]) >= sciFiTracker.ThresholdTime() or
                    not CompatibleTriple(lStats.frontID[li], rStats.frontID[ri], aStats.frontID[ai])) {
                    continue;
                }
                const double angleResidual = std::min(
                    angularDist(angleCond1, aStats.avgAngle[ai]),
                    angularDist(angleCond2, aStats.avgAngle[ai]));
                if (angleResidual > 0.05 * std::numbers::pi) {
                    continue;
                }
                const double timeResidual{std::abs(lStats.avgTime[li] - aStats.avgTime[ai]) +
                                          std::abs(rStats.avgTime[ri] - aStats.avgTime[ai])};
                candidates.push_back({li, ri, ai, timeResidual, angleResidual});
            }
        }
    }
    return candidates;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitFinder<ASciFiHit, ATrack>::BuildAllPairCandidates(
    const ClusterStats& lStats,
    const ClusterStats& rStats,
    const ClusterStats& aStats,
    const std::vector<bool>& lUsed,
    const std::vector<bool>& rUsed,
    const std::vector<bool>& aUsed) -> std::vector<PairCandidate> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<PairCandidate> candidates;

    // LA
    for (size_t li{}; li < lStats.avgTime.size(); ++li) {
        for (size_t ai{}; ai < aStats.avgTime.size(); ++ai) {
            if (aUsed[ai]) {
                continue;
            }
            const double timeResidual{std::abs(lStats.avgTime[li] - aStats.avgTime[ai])};
            if (timeResidual < sciFiTracker.ThresholdTime() and
                AdjacentPair(lStats.frontID[li], aStats.frontID[ai])) {
                candidates.push_back({PairType::LA, li, ai, timeResidual});
            }
        }
    }

    // RA
    for (size_t ri{}; ri < rStats.avgTime.size(); ++ri) {
        for (size_t ai{}; ai < aStats.avgTime.size(); ++ai) {
            if (aUsed[ai]) {
                continue;
            }
            const double timeResidual{std::abs(rStats.avgTime[ri] - aStats.avgTime[ai])};
            if (timeResidual < sciFiTracker.ThresholdTime() and
                AdjacentPair(rStats.frontID[ri], aStats.frontID[ai])) {
                candidates.push_back({PairType::RA, ri, ai, timeResidual});
            }
        }
    }

    // LR
    for (size_t li{}; li < lStats.avgTime.size(); ++li) {
        if (lUsed[li]) {
            continue;
        }
        for (size_t ri{}; ri < rStats.avgTime.size(); ++ri) {
            if (rUsed[ri]) {
                continue;
            }
            const double timeResidual{std::abs(lStats.avgTime[li] - rStats.avgTime[ri])};
            if (timeResidual < sciFiTracker.ThresholdTime() and
                std::abs(SuperLayer(lStats.frontID[li]) - SuperLayer(rStats.frontID[ri])) <= 2) {
                candidates.push_back({PairType::LR, li, ri, timeResidual});
            }
        }
    }

    return candidates;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::FindCompatibleClusterCombinations(const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>>& hitData)
    -> std::set<std::vector<std::vector<AHitPointer>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};
    std::set<std::vector<std::vector<AHitPointer>>> result{};

    auto calculateAvgAngle{[&](const std::vector<AHitPointer>& cluster) -> double {
        if (cluster.empty()) {
            return 0.0;
        }

        double sum{0.0};
        for (const auto& hit : cluster) {
            const auto fiberID{Get<"FiberID">(*hit)};

            sum += fiberMap[fiberID].rotationAngle;
        }
        return sum / cluster.size();
    }};

    auto calculateAvgTime{[&](const std::vector<AHitPointer>& cluster) -> double {
        if (cluster.empty()) {
            return 0.0;
        }

        double sum{0.0};
        for (const auto& hit : cluster) {
            sum += Get<"t">(*hit);
        }
        return sum / cluster.size();
    }};

    auto [lCluster, rCluster, aCluster]{hitData};
    auto buildStats{[&](const std::vector<std::vector<AHitPointer>>& clusters) -> ClusterStats {
        ClusterStats stats{};
        stats.avgAngle.resize(clusters.size());
        stats.avgTime.resize(clusters.size());
        stats.frontID.resize(clusters.size());
        for (size_t i{}; i < clusters.size(); ++i) {
            stats.avgAngle[i] = calculateAvgAngle(clusters[i]);
            stats.avgTime[i] = calculateAvgTime(clusters[i]);
            stats.frontID[i] = Get<"FiberID">(*clusters[i].front());
        }
        return stats;
    }};

    const auto lStats{buildStats(lCluster)};
    const auto rStats{buildStats(rCluster)};
    const auto aStats{buildStats(aCluster)};

    auto tripleCandidates{this->BuildTripleCandidates(lCluster, rCluster, aCluster, lStats, rStats, aStats)};

    std::ranges::sort(tripleCandidates,
                      [](const TripleCandidate& lhs, const TripleCandidate& rhs) {
                          if (std::abs(lhs.timeResidual - rhs.timeResidual) > 1e-9) {
                              return lhs.timeResidual < rhs.timeResidual;
                          }
                          return lhs.angleResidual < rhs.angleResidual;
                      });

    std::vector<bool> lUsed(lCluster.size(), false);
    std::vector<bool> rUsed(rCluster.size(), false);
    std::vector<bool> aUsed(aCluster.size(), false);
    for (const auto& candidate : tripleCandidates) {
        if (lUsed[candidate.lIndex] or rUsed[candidate.rIndex] or aUsed[candidate.aIndex]) {
            continue;
        }
        result.insert({lCluster[candidate.lIndex], rCluster[candidate.rIndex], aCluster[candidate.aIndex]});
        lUsed[candidate.lIndex] = true;
        rUsed[candidate.rIndex] = true;
        aUsed[candidate.aIndex] = true;
    }

    auto pairCandidates{this->BuildAllPairCandidates(lStats, rStats, aStats, lUsed, rUsed, aUsed)};

    std::ranges::sort(pairCandidates,
                      [](const PairCandidate& lhs, const PairCandidate& rhs) {
                          return lhs.timeResidual < rhs.timeResidual;
                      });

    for (const auto& pair : pairCandidates) {
        if (pair.type == PairType::LA) {
            result.insert({lCluster[pair.first], aCluster[pair.second]});
            lUsed[pair.first] = true;
            aUsed[pair.second] = true;
        } else if (pair.type == PairType::RA) {
            result.insert({rCluster[pair.first], aCluster[pair.second]});
            rUsed[pair.first] = true;
            aUsed[pair.second] = true;
        } else {
            result.insert({lCluster[pair.first], rCluster[pair.second]});
            lUsed[pair.first] = true;
            rUsed[pair.second] = true;
        }
    }

    return result;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::CalCoordinates(const std::set<std::vector<std::vector<AHitPointer>>>& hitData, const muc::array3d& direction)
    -> std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> {

    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};
    std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> coordinateMap{};

    auto wrapTo2Pi{[&](double angle) -> double {
        angle = std::fmod(angle, 2 * std::numbers::pi);
        if (angle < 0) {
            angle += 2 * std::numbers::pi;
        }
        return angle;
    }};

    auto calculateLeftZ{[&](double lAngle, double phi, double fiberLength) -> double {
        double delta{wrapTo2Pi(lAngle - phi)};
        return (fiberLength / 2) - (delta / (2 * std::numbers::pi)) * fiberLength;
    }};

    auto calculateRightZ{[&](double rAngle, double phi, double fiberLength) -> double {
        double delta{wrapTo2Pi(rAngle - phi)};

        return -(fiberLength / 2) + (delta / (2 * std::numbers::pi)) * fiberLength;
    }};

    auto angularDist{[](double a, double b) -> double {
        double delta{std::fmod(std::fabs(a - b), 2 * std::numbers::pi)};
        return (delta > std::numbers::pi) ? 2 * std::numbers::pi - delta : delta;
    }};

    auto averageLayerValues{[&](const std::vector<AHitPointer>& hitList, double& angle, double& radius) {
        angle = 0;
        radius = 0;
        for (const auto& hit : hitList) {
            const auto fiberID{Get<"FiberID">(*hit)};
            angle += fiberMap[fiberID].rotationAngle;
            radius += fiberMap[fiberID].radius;
        }
        angle /= hitList.size();
        radius /= hitList.size();
    }};

    auto calculateCoordinates{[&](double lAngle, double rAngle, double aAngle,
                                  double rLLayer, double rRLayer, double rALayer) -> std::vector<muc::array3d> {
        std::vector<muc::array3d> coords{};
        const double fiberLength{sciFiTracker.FiberLength()};
        const bool directionValid{direction[0] != 0 and direction[1] != 0 and direction[2] != 0};

        auto appendCoord{[&](double x, double y, double z) {
            coords.push_back({x, y, z});
        }};

        if (lAngle >= 0 and rAngle >= 0 and aAngle >= 0) {
            double x0{rLLayer};
            const double rDir{direction[0] * std::cos(aAngle) + direction[1] * std::sin(aAngle)};
            const double phiDir{-direction[0] * std::sin(aAngle) + direction[1] * std::cos(aAngle)};

            const double thetaL{directionValid ? (rLLayer - rALayer) / rDir * phiDir / rLLayer : 0.0};
            const double phiL{aAngle + thetaL};
            const double x1{x0 * std::cos(phiL)};
            const double y1{x0 * std::sin(phiL)};
            const double z1{calculateLeftZ(lAngle, phiL, fiberLength)};

            x0 = rRLayer;
            const double thetaR{directionValid ? (rRLayer - rALayer) / rDir * phiDir / rRLayer : 0.0};
            const double phiR{aAngle + thetaR};
            const double x2{x0 * std::cos(phiR)};
            const double y2{x0 * std::sin(phiR)};
            const double z2{calculateRightZ(rAngle, phiR, fiberLength)};

            const double s{lAngle + rAngle};
            double trueTID1{std::fmod(s, 4 * std::numbers::pi) / 2};
            double trueTID2{std::fmod(s + 2 * std::numbers::pi, 4 * std::numbers::pi) / 2};

            if (trueTID1 < 0) {
                trueTID1 += 2 * std::numbers::pi;
            }
            if (trueTID2 < 0) {
                trueTID2 += 2 * std::numbers::pi;
            }
            const double trueTID{(angularDist(trueTID2, aAngle) < angularDist(trueTID1, aAngle)) ? trueTID2 : trueTID1};

            x0 = (rLLayer + rRLayer) / 2;
            const double x3{x0 * std::cos(trueTID)};
            const double y3{x0 * std::sin(trueTID)};
            const double z3{calculateLeftZ(lAngle, trueTID, fiberLength)};

            appendCoord(x1, y1, z1);
            appendCoord(x2, y2, z2);
            appendCoord(x3, y3, z3);
            return coords;
        }

        if (rAngle == -1) {
            double x0{rLLayer};
            const double rDir{direction[0] * std::cos(aAngle) + direction[1] * std::sin(aAngle)};
            const double phiDir{-direction[0] * std::sin(aAngle) + direction[1] * std::cos(aAngle)};
            const double theta{directionValid ? (rLLayer - rALayer) / rDir * phiDir / rLLayer : 0.0};
            const double adjustedAngle{aAngle + theta};
            const double x{x0 * std::cos(adjustedAngle)};
            const double y{x0 * std::sin(adjustedAngle)};
            const double z{calculateLeftZ(lAngle, adjustedAngle, fiberLength)};

            appendCoord(x, y, z);
            return coords;
        }

        if (lAngle == -1) {
            double x0{rRLayer};
            const double rDir{direction[0] * std::cos(aAngle) + direction[1] * std::sin(aAngle)};
            const double phiDir{-direction[0] * std::sin(aAngle) + direction[1] * std::cos(aAngle)};
            const double theta{directionValid ? (rRLayer - rALayer) / rDir * phiDir / rRLayer : 0.0};
            const double adjustedAngle{aAngle + theta};
            const double x{x0 * std::cos(adjustedAngle)};
            const double y{x0 * std::sin(adjustedAngle)};
            const double z{calculateRightZ(rAngle, adjustedAngle, fiberLength)};

            appendCoord(x, y, z);
            return coords;
        }

        if (aAngle == -1) {
            double x0{(rLLayer + rRLayer) / 2};
            const double s{lAngle + rAngle};
            const double trueTID1{std::fmod(s, 4 * std::numbers::pi) / 2};
            const double trueTID2{std::fmod(s + 2 * std::numbers::pi, 4 * std::numbers::pi) / 2};

            // first point
            const double x1{x0 * std::cos(trueTID1)};
            const double y1{x0 * std::sin(trueTID1)};
            const double z1{calculateLeftZ(lAngle, trueTID1, fiberLength)};

            // second point
            const double x2{x0 * std::cos(trueTID2)};
            const double y2{x0 * std::sin(trueTID2)};
            const double z2{calculateLeftZ(lAngle, trueTID2, fiberLength)};

            appendCoord(x1, y1, z1);
            appendCoord(x2, y2, z2);
            return coords;
        }

        Mustard::Throw<std::runtime_error>("Too many negative angles!");
        return coords;
    }};

    for (const auto& hitLists : hitData) {
        double lAngle{-1};
        double rAngle{-1};
        double aAngle{-1};
        double rLLayer{};
        double rRLayer{};
        double rALayer{};
        for (const auto& hitList : hitLists) {
            const auto& layerType{fiberMap[Get<"FiberID">(*hitList.front())].layerType};
            if (layerType == "LHelical") {
                averageLayerValues(hitList, lAngle, rLLayer);
            } else if (layerType == "RHelical") {
                averageLayerValues(hitList, rAngle, rRLayer);
            } else {
                averageLayerValues(hitList, aAngle, rALayer);
            }
        }
        const auto coordinates{calculateCoordinates(lAngle, rAngle, aAngle, rLLayer, rRLayer, rALayer)};

        for (const auto& coordinate : coordinates) {
            coordinateMap[coordinate] = hitLists;
        }
    }
    return coordinateMap;
};

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::DividePoints(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> std::vector<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};

    using Entry = std::pair<muc::array3d, std::vector<std::vector<AHitPointer>>>;

    auto connected{[&](const muc::array3d& x1, const muc::array3d& x2) -> bool {
        double theta1{std::atan2(x1[1], x1[0])};
        double theta2{std::atan2(x2[1], x2[0])};
        double deltaTheta{std::fabs(theta1 - theta2)};
        if (deltaTheta > std::numbers::pi) {
            deltaTheta = 2 * std::numbers::pi - deltaTheta;
        }
        return std::fabs(x1[2] - x2[2]) < sciFiTracker.CentroidZThreshold() and
               deltaTheta < sciFiTracker.CentroidThetaThreshold();
    }};

    std::vector<Entry> entries{};
    entries.reserve(hitData.size());
    for (const auto& p : hitData) {
        entries.push_back(p);
    }
    const size_t n{entries.size()};

    if (n == 0) {
        return {};
    }

    struct UnionFind {
        std::vector<int> parent, rank;
        explicit UnionFind(size_t sz) :
            parent(sz),
            rank(sz, 0) {
            std::iota(parent.begin(), parent.end(), 0);
        }
        auto Find(int x) -> int {
            if (parent[x] != x) {
                parent[x] = Find(parent[x]);
            }
            return parent[x];
        }
        void Unite(int x, int y) {
            int px = Find(x);
            int py = Find(y);
            if (px == py) {
                return;
            }
            if (rank[px] < rank[py]) {
                std::swap(px, py);
            }
            parent[py] = px;
            if (rank[px] == rank[py]) {
                ++rank[px];
            }
        }
    } uf(n);

    for (size_t i{}; i < n; ++i) {
        for (size_t j{i + 1}; j < n; ++j) {
            if (connected(entries[i].first, entries[j].first)) {
                uf.Unite(static_cast<int>(i), static_cast<int>(j));
            }
        }
    }

    std::map<int, std::vector<size_t>> components{};
    for (size_t i{}; i < n; ++i) {
        components[uf.Find(static_cast<int>(i))].push_back(i);
    }

    std::vector<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>> divData{};
    for (auto& component : components) {
        auto& idxList{component.second};
        std::set<std::string> layerTypes{};
        for (size_t idx : idxList) {
            const auto& clusters{entries[idx].second};
            for (const auto& cluster : clusters) {
                for (const auto& hit : cluster) {
                    layerTypes.insert(fiberMap[Get<"FiberID">(*hit)].layerType);
                }
            }
        }

        if (layerTypes.size() == 3) {
            std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> groupMap{};
            for (size_t idx : idxList) {
                auto& [coord, clusters] = entries[idx];
                groupMap[coord] = std::move(clusters);
            }
            divData.push_back(std::move(groupMap));
        }
    }

    return divData;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::EstimateInitialDirection(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>> {

    std::vector<std::vector<AHitPointer>> fiberLists;

    Eigen::Vector3d centroid{Eigen::Vector3d::Zero()};
    for (const auto& [point, _1] : hitData) {
        Eigen::Vector3d c(point[0], point[1], point[2]);
        if (point[0] * centroid.x() + point[1] * centroid.y() + point[2] * centroid.z() >= 0) {
            centroid += c;
        }
    }

    if (hitData.empty()) {
        return std::tuple(muc::array3d{},
                          muc::array3d{},
                          fiberLists);
    }

    if (hitData.size() <= 3) {
        for (const auto& [_1, hitLists] : hitData) {
            fiberLists.insert(fiberLists.end(), hitLists.begin(), hitLists.end());
        }
        Eigen::Vector3d direction{centroid};
        direction.normalize();
        return std::tuple(muc::array3d{direction.x(), direction.y(), direction.z()},
                          muc::array3d{centroid.x(), centroid.y(), centroid.z()},
                          fiberLists);
    }

    centroid /= static_cast<double>(hitData.size());

    std::vector<Eigen::Vector3d> selectedPoints;
    selectedPoints.reserve(hitData.size());

    auto appendUniqueCluster{[&](const std::vector<AHitPointer>& cluster) {
        if (std::ranges::none_of(fiberLists, [&](const auto& existing) {
                return existing == cluster;
            })) {
            fiberLists.push_back(cluster);
        }
    }};

    for (const auto& [point, hitLists] : hitData) {
        if (point[0] * centroid.x() + point[1] * centroid.y() + point[2] * centroid.z() >= 0) {
            selectedPoints.emplace_back(point[0], point[1], point[2]);
            for (const auto& cluster : hitLists) {
                appendUniqueCluster(cluster);
            }
        }
    }

    if (selectedPoints.empty()) {
        for (const auto& [point, hitLists] : hitData) {
            selectedPoints.emplace_back(point[0], point[1], point[2]);
            for (const auto& cluster : hitLists) {
                appendUniqueCluster(cluster);
            }
        }
    }

    Eigen::MatrixXd a(selectedPoints.size(), 3);
    for (size_t i{}; i < selectedPoints.size(); ++i) {
        a.row(static_cast<Eigen::Index>(i)) = selectedPoints[i] - centroid;
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(a, Eigen::ComputeFullV);
    Eigen::Vector3d direction{svd.matrixV().col(0)};

    if (direction.norm() < 1e-12) {
        direction = centroid;
    }

    direction.normalize();
    if (direction[0] * centroid.x() + direction[1] * centroid.y() + direction[2] * centroid.z() < 0) {
        direction *= -1;
    }
    return std::tuple(muc::array3d{direction.x(), direction.y(), direction.z()}, muc::array3d{centroid.x(), centroid.y(), centroid.z()}, fiberLists);
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::AddStraightFibersByTheta(
    const std::vector<std::vector<AHitPointer>>& currentClusters,
    const std::vector<std::vector<AHitPointer>>& axialClusters,
    double angleThreshold,
    double timeThreshold) const -> std::vector<std::vector<AHitPointer>> {

    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};

    double averageTime{};
    size_t referenceCount{};
    for (const auto& cluster : currentClusters) {
        for (const auto& hit : cluster) {
            averageTime += Get<"t">(*hit);
            ++referenceCount;
        }
    }
    if (referenceCount > 0) {
        averageTime /= static_cast<double>(referenceCount);
    }
    struct StraightHitInfo {
        double phi;
        double time;
    };
    std::unordered_map<int, StraightHitInfo> existingStraightHits{}; // fiberID -> hit info
    std::unordered_set<int> usedFiberIDs{};
    for (const auto& cluster : currentClusters) {
        for (const auto& hit : cluster) {
            const int fid{Get<"FiberID">(*hit)};
            if (fiberMap[fid].layerType == "LHelical" or fiberMap[fid].layerType == "RHelical") {
                continue;
            }
            const double hitTime{Get<"t">(*hit)};
            if (std::fabs(hitTime - averageTime) >= timeThreshold) {
                continue;
            }
            if (existingStraightHits.find(fid) == existingStraightHits.end()) {
                existingStraightHits[fid] = {fiberMap[fid].rotationAngle, hitTime};
                usedFiberIDs.insert(fid);
            }
        }
    }

    auto angularDist{[](double a, double b) -> double {
        double delta{std::fabs(a - b)};
        if (delta > std::numbers::pi) {
            delta = 2 * std::numbers::pi - delta;
        }
        return delta;
    }};

    std::vector<std::vector<AHitPointer>> extraClusters;

    for (const auto& cluster : axialClusters) {
        for (const auto& hit : cluster) {
            const int fiberID = Get<"FiberID">(*hit);
            if (usedFiberIDs.contains(fiberID)) {
                continue;
            }

            const double fiberPhi{fiberMap[fiberID].rotationAngle};
            const double fiberTime{Get<"t">(*hit)};

            const bool accepted{std::ranges::any_of(
                existingStraightHits,
                [&](const auto& entry) {
                    const auto& eInfo{entry.second};
                    return angularDist(fiberPhi, eInfo.phi) < angleThreshold and
                           std::fabs(fiberTime - eInfo.time) < timeThreshold;
                })};
            if (accepted) {
                extraClusters.push_back({hit});
                usedFiberIDs.insert(fiberID);
            }
        }
    }

    return extraClusters;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitFinder<ASciFiHit, ATrack>::FindHLMinDistanceSquare(
    double helixR, double helixB, double rotationAngle,
    const muc::array3d linePoint, const muc::array3d lineDir,
    double initialT, double initialTheta) -> std::tuple<double, double, double> {
    const Mustard::Helix helix{
        Mustard::Point2D{0, 0},
        helixR,
        rotationAngle,
        -helixB * std::numbers::pi,
        std::atan2(helixR, helixB)
    };
    const Mustard::Point3D point{linePoint[0], linePoint[1], linePoint[2]};
    const Mustard::Vector3D lineDirection{lineDir[0], lineDir[1], lineDir[2]};
    const Mustard::Line3D line{point, lineDirection};

    const auto pocaResult{Mustard::POCA(helix, line,
                                        0.0, 2 * std::numbers::pi,
                                        1, 300, 1e-7, 1e-7)};
    if (not pocaResult.has_value()) {
        return std::tuple{std::numeric_limits<double>::max(), initialT, initialTheta};
    }

    const auto& [pocaOnHelix, pocaOnLine, doca]{*pocaResult};

    double t{initialT};
    const auto lineDirectionMag2{lineDirection.mag2()};
    if (lineDirectionMag2 > 1e-12) {
        t = (pocaOnLine - point).dot(lineDirection) / lineDirectionMag2;
    }

    auto theta{std::atan2(pocaOnHelix.y(), pocaOnHelix.x()) - rotationAngle};
    while (theta < 0) {
        theta += 2 * std::numbers::pi;
    }
    while (theta >= 2 * std::numbers::pi) {
        theta -= 2 * std::numbers::pi;
    }

    return std::tuple{doca * doca, t, theta};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitFinder<ASciFiHit, ATrack>::FindLLMinDistanceSquare(
    const muc::array3d line1Point, const muc::array3d line1Dir,
    const muc::array3d line2Point, const muc::array3d line2Dir) -> double {
    const auto [poca1, poca2, doca]{
        Mustard::POCA(Mustard::Line3D{Mustard::Point3D{line1Point[0], line1Point[1], line1Point[2]}, Mustard::Vector3D{line1Dir[0], line1Dir[1], line1Dir[2]}},
                      Mustard::Line3D{Mustard::Point3D{line2Point[0], line2Point[1], line2Point[2]}, Mustard::Vector3D{line2Dir[0], line2Dir[1], line2Dir[2]}}
        )
    };
    return doca * doca;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitFinder<ASciFiHit, ATrack>::TrackFit(std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> std::shared_ptr<Mustard::Data::Tuple<ATrack>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap{sciFiTracker.DetectorFiberInformation()};
    auto result{std::make_shared<Mustard::Data::Tuple<ATrack>>()};
    auto r{(sciFiTracker.BracketInnerRadius() + sciFiTracker.BracketOuterRadius()) / 2};

    auto [initialDirection, initialCentroid, clusterLists]{hitData};

    double t{};
    int fiberNum{};
    for (auto&& cluster : clusterLists) {
        for (auto&& hit : cluster) {
            t += Get<"t">(*hit);
            fiberNum++;
        }
    }

    double initialSTheta{std::acos(initialCentroid[2] / muc::hypot(initialCentroid[0], initialCentroid[1], initialCentroid[2]))};
    double initialSPhi{std::atan2(initialCentroid[1], initialCentroid[0])};
    double initialPTheta{std::acos(initialDirection[2] / muc::hypot(initialDirection[0], initialDirection[1], initialDirection[2]))};
    double initialPPhi{std::atan2(initialDirection[1], initialDirection[0])};
    const double initialT{std::abs(initialDirection[2]) > 1e-12 ?
                              (initialCentroid[2] - r * std::cos(initialSTheta)) / initialDirection[2] :
                              0.0};
    const double bRef{sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
    const double initialTheta{initialCentroid[2] / bRef};

    ROOT::Minuit2::Minuit2Minimizer minimizer{};
    std::function targetFunction{
        [&](const double* xx) {
            muc::array3d s0{r * std::cos(xx[1]) * std::sin(xx[0]), r * std::sin(xx[1]) * std::sin(xx[0]), r * std::cos(xx[0])};
            const muc::array3d dir{std::cos(xx[3]) * std::sin(xx[2]), std::sin(xx[3]) * std::sin(xx[2]), std::cos(xx[2])};

            double t{};
            double theta{};
            double distance{};
            // Keep objective deterministic across minimizer evaluations.
            double localInitialT{initialT};
            double localInitialTheta{initialTheta};

            std::unordered_set<int> processedSiPMIDs{};
            for (int i{}; i < std::ssize(clusterLists); ++i) {
                auto&& cluster{clusterLists[i]};
                for (auto&& hit : cluster) {
                    auto rLayer{fiberMap[Get<"FiberID">(*hit)].radius};
                    if (processedSiPMIDs.count(Get<"FiberID">(*hit)) > 0) {
                        continue;
                    }
                    processedSiPMIDs.insert(Get<"FiberID">(*hit));
                    double rotationAngle{fiberMap[Get<"FiberID">(*hit)].rotationAngle};

                    if (fiberMap.at(Get<"FiberID">(*hit)).layerType == "LHelical") {
                        double b{sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        double minDistance{};
                        std::tie(minDistance, t, theta) = this->FindHLMinDistanceSquare(rLayer, b, rotationAngle,
                                                                                        s0, dir, localInitialT, localInitialTheta);
                        distance += minDistance;
                    } else if (fiberMap.at(Get<"FiberID">(*hit)).layerType == "RHelical") {
                        double b{-sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        double minDistance{};
                        std::tie(minDistance, t, theta) = this->FindHLMinDistanceSquare(rLayer, b, rotationAngle,
                                                                                        s0, dir, localInitialT, localInitialTheta);
                        distance += minDistance;
                    } else {
                        double x0{rLayer};
                        double y0{};
                        double minDistance{};
                        double x{x0 * std::cos(rotationAngle) - y0 * std::sin(rotationAngle)};
                        double y{x0 * std::sin(rotationAngle) + y0 * std::cos(rotationAngle)};
                        minDistance = this->FindLLMinDistanceSquare(s0, dir, {x, y, 0}, {0, 0, 1});
                        distance += minDistance;
                    }

                    localInitialT = t;
                    localInitialTheta = theta;
                }
            }
            return distance;
        }};

    ROOT::Math::Functor f(targetFunction, 4);
    minimizer.SetFunction(f);
    minimizer.SetStrategy(2);
    // minimizer.SetPrintLevel(2);
    minimizer.SetLimitedVariable(0, "theta", initialSTheta, 1e-2, initialSTheta - std::numbers::pi / 4, initialSTheta + std::numbers::pi / 4);
    minimizer.SetLimitedVariable(1, "phi", initialSPhi, 1e-2, initialSPhi - std::numbers::pi / 4, initialSPhi + std::numbers::pi / 4);
    minimizer.SetLimitedVariable(2, "thetap", initialPTheta, 1e-2, initialPTheta - std::numbers::pi / 4, initialPTheta + std::numbers::pi / 4);
    minimizer.SetLimitedVariable(3, "phip", initialPPhi, 1e-2, initialPPhi - std::numbers::pi / 4, initialPPhi + std::numbers::pi / 4);
    minimizer.Minimize();
    if (minimizer.Status() >= 1) {
        Get<"EvtID">(*result) = Get<"EvtID">(*clusterLists.front().front());
        Get<"x">(*result) = initialCentroid;
        Get<"p">(*result) = initialDirection;
        Get<"t">(*result) = t / fiberNum;
        return result;
    }
    muc::array3d x{r * std::cos(minimizer.State().Parameter(1).Value()) * std::sin(minimizer.State().Parameter(0).Value()),
                   r * std::sin(minimizer.State().Parameter(1).Value()) * std::sin(minimizer.State().Parameter(0).Value()),
                   r * std::cos(minimizer.State().Parameter(0).Value())};

    muc::array3d p{std::cos(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
                   std::sin(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
                   std::cos(minimizer.State().Parameter(2).Value())};

    if (p[0] * initialCentroid[0] + p[1] * initialCentroid[1] + p[2] * initialCentroid[2] < 0) {
        p[0] *= -1;
        p[1] *= -1;
        p[2] *= -1;
    }

    Get<"EvtID">(*result) = Get<"EvtID">(*clusterLists.front().front());
    Get<"x">(*result) = x;
    Get<"p">(*result) = p;
    Get<"t">(*result) = t / fiberNum;
    return result;
}

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder
