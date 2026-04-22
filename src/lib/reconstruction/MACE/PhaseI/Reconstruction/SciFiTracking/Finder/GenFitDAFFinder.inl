namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
GenFitDAFFinder<ASciFiHit, ATrack>::GenFitDAFFinder() :
    Base{},
    fFirstSegmentMaxDeltaPhi{},
    fFirstSegmentMinNHit{},
    fMomentumSeed{} {}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::operator()(std::vector<AHitPointer>& hitData, int nextTrackID) -> Base::template Result<AHitPointer> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    using Result = Base::template Result<AHitPointer>;
    Result r;

    auto clusterLists{this->ClusterHits(hitData)};
    auto dividedClusters{this->DivideHits(clusterLists)};
    auto hitLists{this->FindCompatibleClusterCombinations(dividedClusters)};

    // Initial coordinates without direction correction.
    auto coordinateMap{this->CalCoordinates(hitLists, muc::array3d{})};
    auto dividedPointMap{this->DividePoints(coordinateMap)};

    // Keep the initial groups and update each group independently in iterations.
    auto initialDivided = dividedPointMap;

    struct Candidate {
        std::vector<int> signature;
        std::vector<AHitPointer> hitData;
        std::shared_ptr<Mustard::Data::Tuple<ATrack>> seed;
        double alignment;
    };
    std::vector<Candidate> candidates;
    for (const auto& initialCluster : initialDivided) {
        auto initialResult = this->EstimateInitialDirection(initialCluster);
        muc::array3d prev_dir = std::get<0>(initialResult);

        constexpr int nIter = 10;
        double minDirDotThreshold{sciFiTracker.MinDirDotThreshold()};
        for (int iter = 0; iter < nIter; ++iter) {
            coordinateMap = this->CalCoordinates(hitLists, prev_dir);
            dividedPointMap = this->DividePoints(coordinateMap);
            muc::array3d initialCentroid = std::get<1>(initialResult);

            auto bestIt = std::ranges::min_element(dividedPointMap,
                                                   std::ranges::less{},
                                                   [&](const auto& group) -> double {
                                                       auto [_, c, __] = this->EstimateInitialDirection(group);
                                                       return (c[0] - initialCentroid[0]) * (c[0] - initialCentroid[0]) + (c[1] - initialCentroid[1]) * (c[1] - initialCentroid[1]) + (c[2] - initialCentroid[2]) * (c[2] - initialCentroid[2]);
                                                   });

            if (bestIt != dividedPointMap.end()) {
                auto nextResult = this->EstimateInitialDirection(*bestIt);
                const auto& curr_dir = std::get<0>(nextResult);
                const double dirDot{prev_dir[0] * curr_dir[0] + prev_dir[1] * curr_dir[1] + prev_dir[2] * curr_dir[2]};

                if (dirDot < minDirDotThreshold) {
                    break;
                }

                initialResult = nextResult;
                prev_dir = curr_dir;
            }
        }

        std::vector<int> signature;
        for (const auto& cluster : std::get<2>(initialResult)) {
            for (const auto& hit : cluster) {
                signature.push_back(Get<"FiberID">(*hit));
            }
        }
        std::ranges::sort(signature);
        signature.erase(std::unique(signature.begin(), signature.end()), signature.end());
        if (signature.empty()) {
            continue;
        }

        auto result{this->TrackFit(initialResult)};

        std::vector<AHitPointer> resultData;
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

    auto overlapFraction = [](const std::vector<int>& lhs, const std::vector<int>& rhs) -> double {
        if (lhs.empty() || rhs.empty()) {
            return 0;
        }

        size_t i{}, j{}, intersection{};
        while (i < lhs.size() && j < rhs.size()) {
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

        const auto denominator = std::min(lhs.size(), rhs.size());
        return static_cast<double>(intersection) / static_cast<double>(denominator);
    };

    std::vector<std::vector<int>> acceptedSignatures;
    for (auto& candidate : candidates) {
        double maxOverlap{};
        for (const auto& signature : acceptedSignatures) {
            maxOverlap = std::max(maxOverlap, overlapFraction(candidate.signature, signature));
        }

        const bool overlapped{maxOverlap > 0.5};
        if (overlapped) {
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
auto GenFitDAFFinder<ASciFiHit, ATrack>::ClusterHits(std::vector<AHitPointer>& hitData) -> const std::vector<std::vector<AHitPointer>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();

    std::vector<std::vector<AHitPointer>> clusterList;

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
auto GenFitDAFFinder<ASciFiHit, ATrack>::DivideHits(const std::vector<std::vector<AHitPointer>>& hitData)
    -> const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();

    std::vector<std::vector<AHitPointer>> lCluster;
    std::vector<std::vector<AHitPointer>> rCluster;
    std::vector<std::vector<AHitPointer>> aCluster;

    for (auto&& cluster : hitData) {
        auto firstSiPMID{Get<"FiberID">(*cluster.front())};
        auto layerType{fiberMap[firstSiPMID].layerType};

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
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::FindCompatibleClusterCombinations(const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>>& hitData)
    -> const std::set<std::vector<std::vector<AHitPointer>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::set<std::vector<std::vector<AHitPointer>>> result;

    auto superLayer = [&fiberMap](int fiberID) -> int {
        return static_cast<int>(fiberMap[fiberID].layerID / 2);
    };

    auto areAdjacentPair = [&superLayer](int lhsID, int rhsID) -> bool {
        return std::abs(superLayer(lhsID) - superLayer(rhsID)) <= 1;
    };

    auto areCompatibleTriple = [&superLayer](int lID, int rID, int aID) -> bool {
        const int gL = superLayer(lID);
        const int gR = superLayer(rID);
        const int gA = superLayer(aID);

        const int minLR = std::min(gL, gR);
        const int maxLR = std::max(gL, gR);
        const int lrGap = maxLR - minLR;

        if (lrGap != 2) {
            return false;
        }

        // ALAR/ARAL with missing middle A: allow one skipped A group between L and R.
        // For |L-R| = 2, accept A at:
        // 1) middle group (between L and R), or
        // 2) outer adjacent group (just outside L/R block).

        const bool isMiddleA = (gA > minLR && gA < maxLR);
        return isMiddleA;
    };

    auto calculateAvgAngle{[&fiberMap](const std::vector<AHitPointer>& cluster) -> double {
        if (cluster.empty())
            return 0.0;

        double sum = 0.0;
        for (const auto& hit : cluster) {
            auto fiberID = Get<"FiberID">(*hit);

            sum += fiberMap[fiberID].rotationAngle;
        }
        return sum / cluster.size();
    }};

    auto calculateAvgTime{[](const std::vector<AHitPointer>& cluster) -> double {
        if (cluster.empty())
            return 0.0;

        double sum = 0.0;
        for (const auto& hit : cluster) {
            sum += Get<"t">(*hit);
        }
        return sum / cluster.size();
    }};

    auto normalizeAngle = [](double angle) -> double {
        angle = std::fmod(angle, 2 * std::numbers::pi);
        if (angle < 0) {
            angle += 2 * std::numbers::pi;
        }
        return angle;
    };

    auto angularDist = [](double a, double b) -> double {
        double delta = std::fmod(std::fabs(a - b), 2 * std::numbers::pi);
        return (delta > std::numbers::pi) ? 2 * std::numbers::pi - delta : delta;
    };

    auto [lCluster, rCluster, aCluster] = hitData;

    struct TripleCandidate {
        size_t lIndex;
        size_t rIndex;
        size_t aIndex;
        double timeResidual;
        double angleResidual;
    };

    std::vector<double> lAvgAngle(lCluster.size()), lAvgTime(lCluster.size());
    std::vector<double> rAvgAngle(rCluster.size()), rAvgTime(rCluster.size());
    std::vector<double> aAvgAngle(aCluster.size()), aAvgTime(aCluster.size());
    std::vector<int> lFrontID(lCluster.size()), rFrontID(rCluster.size()), aFrontID(aCluster.size());

    for (size_t i{}; i < lCluster.size(); ++i) {
        lAvgAngle[i] = calculateAvgAngle(lCluster[i]);
        lAvgTime[i] = calculateAvgTime(lCluster[i]);
        lFrontID[i] = Get<"FiberID">(*lCluster[i].front());
    }
    for (size_t i{}; i < rCluster.size(); ++i) {
        rAvgAngle[i] = calculateAvgAngle(rCluster[i]);
        rAvgTime[i] = calculateAvgTime(rCluster[i]);
        rFrontID[i] = Get<"FiberID">(*rCluster[i].front());
    }
    for (size_t i{}; i < aCluster.size(); ++i) {
        aAvgAngle[i] = calculateAvgAngle(aCluster[i]);
        aAvgTime[i] = calculateAvgTime(aCluster[i]);
        aFrontID[i] = Get<"FiberID">(*aCluster[i].front());
    }

    std::vector<TripleCandidate> tripleCandidates;
    for (size_t li{}; li < lCluster.size(); ++li) {
        for (size_t ri{}; ri < rCluster.size(); ++ri) {
            double S = lAvgAngle[li] + rAvgAngle[ri];
            double angleCondition1 = normalizeAngle(std::fmod(S, 4 * std::numbers::pi) / 2);
            double angleCondition2 = normalizeAngle(std::fmod(S + 2 * std::numbers::pi, 4 * std::numbers::pi) / 2);

            for (size_t ai{}; ai < aCluster.size(); ++ai) {
                if (std::abs(lAvgTime[li] - aAvgTime[ai]) >= sciFiTracker.ThresholdTime() ||
                    std::abs(rAvgTime[ri] - aAvgTime[ai]) >= sciFiTracker.ThresholdTime() ||
                    !areCompatibleTriple(lFrontID[li], rFrontID[ri], aFrontID[ai])) {
                    continue;
                }

                const auto angleResidual = std::min(angularDist(angleCondition1, aAvgAngle[ai]), angularDist(angleCondition2, aAvgAngle[ai]));
                if (angleResidual > 0.05 * std::numbers::pi) {
                    continue;
                }

                const auto timeResidual = std::abs(lAvgTime[li] - aAvgTime[ai]) + std::abs(rAvgTime[ri] - aAvgTime[ai]);
                tripleCandidates.push_back({li, ri, ai, timeResidual, angleResidual});
            }
        }
    }

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
        if (lUsed[candidate.lIndex] || rUsed[candidate.rIndex] || aUsed[candidate.aIndex]) {
            continue;
        }
        result.insert({lCluster[candidate.lIndex], rCluster[candidate.rIndex], aCluster[candidate.aIndex]});
        lUsed[candidate.lIndex] = true;
        rUsed[candidate.rIndex] = true;
        aUsed[candidate.aIndex] = true;
    }

    enum class PairType {
        LA,
        RA,
        LR
    };
    struct PairCandidate {
        PairType type;
        size_t first;
        size_t second;
        double timeResidual;
    };
    std::vector<PairCandidate> pairCandidates;

    for (size_t li{}; li < lCluster.size(); ++li) {
        if (lUsed[li]) {
            continue;
        }
        for (size_t ai{}; ai < aCluster.size(); ++ai) {
            if (aUsed[ai]) {
                continue;
            }
            if (std::abs(lAvgTime[li] - aAvgTime[ai]) < sciFiTracker.ThresholdTime() and
                areAdjacentPair(lFrontID[li], aFrontID[ai])) {
                pairCandidates.push_back({PairType::LA, li, ai, std::abs(lAvgTime[li] - aAvgTime[ai])});
            }
        }
    }

    for (size_t ri{}; ri < rCluster.size(); ++ri) {
        if (rUsed[ri]) {
            continue;
        }
        for (size_t ai{}; ai < aCluster.size(); ++ai) {
            if (aUsed[ai]) {
                continue;
            }
            if (std::abs(rAvgTime[ri] - aAvgTime[ai]) < sciFiTracker.ThresholdTime() and
                areAdjacentPair(rFrontID[ri], aFrontID[ai])) {
                pairCandidates.push_back({PairType::RA, ri, ai, std::abs(rAvgTime[ri] - aAvgTime[ai])});
            }
        }
    }

    for (size_t li{}; li < lCluster.size(); ++li) {
        if (lUsed[li]) {
            continue;
        }
        for (size_t ri{}; ri < rCluster.size(); ++ri) {
            if (rUsed[ri]) {
                continue;
            }
            if (std::abs(lAvgTime[li] - rAvgTime[ri]) < sciFiTracker.ThresholdTime() and
                areAdjacentPair(lFrontID[li], rFrontID[ri])) {
                pairCandidates.push_back({PairType::LR, li, ri, std::abs(lAvgTime[li] - rAvgTime[ri])});
            }
        }
    }

    std::ranges::sort(pairCandidates,
                      [](const PairCandidate& lhs, const PairCandidate& rhs) {
                          return lhs.timeResidual < rhs.timeResidual;
                      });

    for (const auto& pair : pairCandidates) {
        if (pair.type == PairType::LA) {
            if (lUsed[pair.first] or aUsed[pair.second]) {
                continue;
            }
            result.insert({lCluster[pair.first], aCluster[pair.second]});
            lUsed[pair.first] = true;
            aUsed[pair.second] = true;
        } else if (pair.type == PairType::RA) {
            if (rUsed[pair.first] or aUsed[pair.second]) {
                continue;
            }
            result.insert({rCluster[pair.first], aCluster[pair.second]});
            rUsed[pair.first] = true;
            aUsed[pair.second] = true;
        } else {
            if (lUsed[pair.first] or rUsed[pair.second]) {
                continue;
            }
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
auto GenFitDAFFinder<ASciFiHit, ATrack>::CalCoordinates(const std::set<std::vector<std::vector<AHitPointer>>>& hitData, const muc::array3d& direction)
    -> const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> {

    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> coordinateMap;

    auto wrapTo2Pi = [](double angle) -> double {
        angle = std::fmod(angle, 2 * std::numbers::pi);
        if (angle < 0)
            angle += 2 * std::numbers::pi;
        return angle;
    };

    auto calculateLeftZ = [&wrapTo2Pi](double lAngle, double phi, double fiberLength) -> double {
        double delta = wrapTo2Pi(lAngle - phi);
        return (fiberLength / 2) - (delta / (2 * std::numbers::pi)) * fiberLength;
    };

    auto calculateRightZ = [&wrapTo2Pi](double rAngle, double phi, double fiberLength) -> double {
        double delta = wrapTo2Pi(rAngle - phi);

        return -(fiberLength / 2) + (delta / (2 * std::numbers::pi)) * fiberLength;
    };

    auto calculateCoordinates = [&](double lAngle, double rAngle, double aAngle,
                                    double rLLayer, double rRLayer, double rALayer) -> std::vector<muc::array3d> {
        std::vector<muc::array3d> coords;

        if (lAngle >= 0 and rAngle >= 0 and aAngle >= 0) {
            double x0 = rLLayer;
            double rDir = direction[0] * std::cos(aAngle) + direction[1] * std::sin(aAngle);
            double phiDir = -direction[0] * std::sin(aAngle) + direction[1] * std::cos(aAngle);

            double thetaL = (direction[0] != 0 and direction[1] != 0 and direction[2] != 0) ?
                                (rLLayer - rALayer) / rDir * phiDir / rLLayer :
                                0.0;
            double phiL = aAngle + thetaL;
            double x1{x0 * std::cos(phiL)};
            double y1{x0 * std::sin(phiL)};
            double z1{calculateLeftZ(lAngle, phiL, sciFiTracker.FiberLength())};

            x0 = rRLayer;
            double thetaR = (direction[0] != 0 and direction[1] != 0 and direction[2] != 0) ?
                                (rRLayer - rALayer) / rDir * phiDir / rRLayer :
                                0.0;
            double phiR = aAngle + thetaR;
            double x2{x0 * std::cos(phiR)};
            double y2{x0 * std::sin(phiR)};
            double z2{calculateRightZ(rAngle, phiR, sciFiTracker.FiberLength())};

            double S = lAngle + rAngle;
            double trueTID1 = std::fmod(S, 4 * std::numbers::pi) / 2;
            double trueTID2 = std::fmod(S + 2 * std::numbers::pi, 4 * std::numbers::pi) / 2;

            if (trueTID1 < 0)
                trueTID1 += 2 * std::numbers::pi;
            if (trueTID2 < 0)
                trueTID2 += 2 * std::numbers::pi;

            auto angularDist = [](double a, double b) {
                double delta = std::fmod(std::fabs(a - b), 2 * std::numbers::pi);
                return (delta > std::numbers::pi) ? 2 * std::numbers::pi - delta : delta;
            };

            double trueTID = (angularDist(trueTID2, aAngle) < angularDist(trueTID1, aAngle)) ? trueTID2 : trueTID1;

            x0 = (rLLayer + rRLayer) / 2;
            double x3{x0 * std::cos(trueTID)};
            double y3{x0 * std::sin(trueTID)};
            double z3{calculateLeftZ(lAngle, trueTID, sciFiTracker.FiberLength())};

            coords = {
                {x1, y1, z1},
                {x2, y2, z2},
                {x3, y3, z3}
            };
        } else if (rAngle == -1) {
            double x0 = rLLayer;
            double rDir = direction[0] * std::cos(aAngle) + direction[1] * std::sin(aAngle);
            double phiDir = -direction[0] * std::sin(aAngle) + direction[1] * std::cos(aAngle);
            double theta = (direction[0] != 0 && direction[1] != 0 && direction[2] != 0) ? (rLLayer - rALayer) / rDir * phiDir / rLLayer : 0;

            aAngle += theta;
            double x = x0 * std::cos(aAngle);
            double y = x0 * std::sin(aAngle);
            double z = calculateLeftZ(lAngle, aAngle, sciFiTracker.FiberLength());

            coords = {
                {x, y, z}
            };
        } else if (lAngle == -1) {
            double x0 = rRLayer;
            double rDir = direction[0] * std::cos(aAngle) + direction[1] * std::sin(aAngle);
            double phiDir = -direction[0] * std::sin(aAngle) + direction[1] * std::cos(aAngle);
            double theta = (direction[0] != 0 && direction[1] != 0 && direction[2] != 0) ? (rRLayer - rALayer) / rDir * phiDir / rRLayer : 0;

            aAngle += theta;
            double x = x0 * std::cos(aAngle);
            double y = x0 * std::sin(aAngle);
            double z = calculateRightZ(rAngle, aAngle, sciFiTracker.FiberLength());

            coords = {
                {x, y, z}
            };
        } else if (aAngle == -1) {
            double x0 = (rLLayer + rRLayer) / 2;
            double S = lAngle + rAngle;
            double trueTID1 = std::fmod(S, 4 * std::numbers::pi) / 2;
            double trueTID2 = std::fmod(S + 2 * std::numbers::pi, 4 * std::numbers::pi) / 2;

            // first point
            double x1 = x0 * std::cos(trueTID1);
            double y1 = x0 * std::sin(trueTID1);
            double z1 = calculateLeftZ(lAngle, trueTID1, sciFiTracker.FiberLength());

            // second point
            double x2 = x0 * std::cos(trueTID2);
            double y2 = x0 * std::sin(trueTID2);
            double z2 = calculateLeftZ(lAngle, trueTID2, sciFiTracker.FiberLength());

            coords = {
                {x1, y1, z1},
                {x2, y2, z2}
            };
        } else {
            Mustard::Throw<std::runtime_error>("Too many negative angles!");
        }
        return coords;
    };
    for (auto&& hitLists : hitData) {
        double lAngle{-1};
        double rAngle{-1};
        double aAngle{-1};
        double rLLayer{}, rRLayer{}, rALayer{};
        for (auto&& hitList : hitLists) {
            if (fiberMap[Get<"FiberID">(*hitList.front())].layerType == "LHelical") {
                lAngle = 0;
                for (auto&& hit : hitList) {
                    lAngle += fiberMap[Get<"FiberID">(*hit)].rotationAngle;
                    rLLayer += fiberMap[Get<"FiberID">(*hit)].radius;
                }
                lAngle /= hitList.size();
                rLLayer /= hitList.size();
            } else if (fiberMap[Get<"FiberID">(*hitList.front())].layerType == "RHelical") {
                rAngle = 0;
                for (auto&& hit : hitList) {
                    rAngle += fiberMap[Get<"FiberID">(*hit)].rotationAngle;
                    rRLayer += fiberMap[Get<"FiberID">(*hit)].radius;
                }
                rAngle /= hitList.size();
                rRLayer /= hitList.size();
            } else {
                aAngle = 0;
                for (auto&& hit : hitList) {
                    aAngle += fiberMap[Get<"FiberID">(*hit)].rotationAngle;
                    rALayer += fiberMap[Get<"FiberID">(*hit)].radius;
                }
                aAngle /= hitList.size();
                rALayer /= hitList.size();
            }
        }
        auto coordinates = calculateCoordinates(lAngle, rAngle, aAngle, rLLayer, rRLayer, rALayer);

        for (auto&& coordinate : coordinates) {
            coordinateMap[coordinate] = hitLists;
        }
    }
    return coordinateMap;
};

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::DividePoints(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> const std::vector<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};

    using Entry = std::pair<muc::array3d, std::vector<std::vector<AHitPointer>>>;

    auto isConnected = [&](const muc::array3d& x1, const muc::array3d& x2) -> bool {
        double theta1 = std::atan2(x1[1], x1[0]);
        double theta2 = std::atan2(x2[1], x2[0]);
        double deltaTheta = std::fabs(theta1 - theta2);
        if (deltaTheta > std::numbers::pi) {
            deltaTheta = 2 * std::numbers::pi - deltaTheta;
        }
        return std::fabs(x1[2] - x2[2]) < sciFiTracker.CentroidZThreshold() &&
               deltaTheta < sciFiTracker.CentroidThetaThreshold();
    };

    std::vector<Entry> entries;
    entries.reserve(hitData.size());
    for (const auto& p : hitData) {
        entries.push_back(p);
    }
    const size_t n = entries.size();

    if (n == 0)
        return {};

    struct UnionFind {
        std::vector<int> parent, rank;
        UnionFind(size_t sz) :
            parent(sz),
            rank(sz, 0) {
            std::iota(parent.begin(), parent.end(), 0);
        }
        int find(int x) {
            if (parent[x] != x)
                parent[x] = find(parent[x]);
            return parent[x];
        }
        void unite(int x, int y) {
            int px = find(x), py = find(y);
            if (px == py)
                return;
            if (rank[px] < rank[py])
                std::swap(px, py);
            parent[py] = px;
            if (rank[px] == rank[py])
                ++rank[px];
        }
    } uf(n);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (isConnected(entries[i].first, entries[j].first)) {
                uf.unite(static_cast<int>(i), static_cast<int>(j));
            }
        }
    }

    std::map<int, std::vector<size_t>> components;
    for (size_t i = 0; i < n; ++i) {
        components[uf.find(static_cast<int>(i))].push_back(i);
    }

    std::vector<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>> divData;
    for (auto& component : components) {
        auto& idxList = component.second;
        bool hasAnySize3 = false;

        for (size_t idx : idxList) {
            const auto& clusters = entries[idx].second;

            if (clusters.size() == 3) {
                hasAnySize3 = true;
            }
        }
        if (hasAnySize3) {
            std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> groupMap;
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
auto GenFitDAFFinder<ASciFiHit, ATrack>::EstimateInitialDirection(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>> {

    std::vector<std::vector<AHitPointer>> fiberLists;

    Eigen::Vector3d centroid = Eigen::Vector3d::Zero();
    for (const auto& [point, _] : hitData) {
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

    if (hitData.size() == 1) {
        for (const auto& [_, hitLists] : hitData) {
            fiberLists.insert(fiberLists.end(), hitLists.begin(), hitLists.end());
        }
        Eigen::Vector3d direction = centroid;
        if (direction.norm() < 1e-12) {
            direction = Eigen::Vector3d::UnitZ();
        }
        direction.normalize();
        return std::tuple(muc::array3d{direction.x(), direction.y(), direction.z()},
                          muc::array3d{centroid.x(), centroid.y(), centroid.z()},
                          fiberLists);
    }

    centroid /= static_cast<double>(hitData.size());

    std::vector<Eigen::Vector3d> selectedPoints;
    selectedPoints.reserve(hitData.size());

    auto appendUniqueCluster = [&](const std::vector<AHitPointer>& cluster) {
        if (std::ranges::none_of(fiberLists, [&](const auto& existing) {
                return existing == cluster;
            })) {
            fiberLists.push_back(cluster);
        }
    };

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

    Eigen::MatrixXd A(selectedPoints.size(), 3);
    for (size_t i = 0; i < selectedPoints.size(); ++i) {
        A.row(static_cast<Eigen::Index>(i)) = selectedPoints[i] - centroid;
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullV);
    Eigen::Vector3d direction = svd.matrixV().col(0);

    if (direction.norm() < 1e-12) {
        direction = centroid;
    }
    if (direction.norm() < 1e-12) {
        direction = Eigen::Vector3d::UnitZ();
    }
    direction.normalize();
    if (direction[0] * centroid.x() + direction[1] * centroid.y() + direction[2] * centroid.z() < 0) {
        direction *= -1;
    }
    return std::tuple(muc::array3d{direction.x(), direction.y(), direction.z()}, muc::array3d{centroid.x(), centroid.y(), centroid.z()}, fiberLists);
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitDAFFinder<ASciFiHit, ATrack>::Helix(double theta, double r, double b, double rotationAngle) -> const muc::array3d {
    double u{theta + rotationAngle};
    double zOffset{b * std::numbers::pi};
    return {r * std::cos(u), r * std::sin(u), b * theta - zOffset};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitDAFFinder<ASciFiHit, ATrack>::Line(double t, const muc::array3d s0, const muc::array3d d) -> const muc::array3d {
    return muc::array3d{
        s0[0] + t * d[0],
        s0[1] + t * d[1],
        s0[2] + t * d[2]};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitDAFFinder<ASciFiHit, ATrack>::FindHLMinDistanceSquare(
    double HelixR, double HelixB, double rotationAngle,
    const muc::array3d line_p0, const muc::array3d line_dir,
    double initialT, double initialTheta) -> std::tuple<double, double, double> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    ROOT::Minuit2::Minuit2Minimizer minimizer;

    std::function targetFunction{[HelixR, HelixB, rotationAngle, line_p0, line_dir, this](const double* xx) {
        double t{xx[0]};
        double theta{xx[1]};
        muc::array3d sp_point{this->Helix(theta, HelixR, HelixB, rotationAngle)};
        muc::array3d line_point{this->Line(t, line_p0, line_dir)};
        double dis{muc::hypot(sp_point[0] - line_point[0], sp_point[1] - line_point[1], sp_point[2] - line_point[2]) *
                   muc::hypot(sp_point[0] - line_point[0], sp_point[1] - line_point[1], sp_point[2] - line_point[2])};
        return dis;
    }};

    ROOT::Math::Functor f(targetFunction, 2);
    minimizer.SetFunction(f);
    minimizer.SetLimitedVariable(0, "x", initialT, 0.01, -sciFiTracker.FiberLength() / 2, sciFiTracker.FiberLength() / 2);
    minimizer.SetLimitedVariable(1, "y", initialTheta, 0.01, 0, 2 * std::numbers::pi);
    minimizer.Minimize();
    return std::tuple{minimizer.MinValue(), minimizer.State().Parameter(0).Value(), minimizer.State().Parameter(1).Value()};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
auto GenFitDAFFinder<ASciFiHit, ATrack>::FindLLMinDistanceSquare(
    const muc::array3d line1_point, const muc::array3d line1_dir,
    const muc::array3d line2_point, const muc::array3d line2_dir) -> double {

    muc::array3d vector1{line1_point[0] - line2_point[0], line1_point[1] - line2_point[1], line1_point[2] - line2_point[2]};
    muc::array3d cross{
        line1_dir[1] * line2_dir[2] - line2_dir[1] * line1_dir[2],
        line1_dir[2] * line2_dir[0] - line2_dir[2] * line1_dir[0],
        line1_dir[0] * line2_dir[1] - line2_dir[0] * line1_dir[1]};
    if (std::hypot(cross[0], cross[1], cross[2]) < 1e-9) {
        double dot = ((line1_point[0] - line2_point[0]) * line2_dir[0] +
                      (line1_point[1] - line2_point[1]) * line2_dir[1] +
                      (line1_point[2] - line2_point[2]) * line2_dir[2]) /
                     sqrt(line2_dir[0] * line2_dir[0] + line2_dir[1] * line2_dir[1] + line2_dir[2] * line2_dir[2]);

        return ((line1_point[0] - line2_point[0]) * (line1_point[0] - line2_point[0]) +
                (line1_point[1] - line2_point[1]) * (line1_point[1] - line2_point[1]) +
                (line1_point[2] - line2_point[2]) * (line1_point[2] - line2_point[2]) - dot * dot);
        // return DLDistance(line1_point, line2_point, line2_dir);
    }
    double min_dis = ((vector1[0] * cross[0] + vector1[1] * cross[1] + vector1[2] * cross[2]) / std::hypot(cross[0], cross[1], cross[2])) *
                     ((vector1[0] * cross[0] + vector1[1] * cross[1] + vector1[2] * cross[2]) / std::hypot(cross[0], cross[1], cross[2]));
    return min_dis;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::TrackFit(std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> std::shared_ptr<Mustard::Data::Tuple<ATrack>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    auto result = std::make_shared<Mustard::Data::Tuple<ATrack>>();
    auto r{(sciFiTracker.BracketInnerRadius() + sciFiTracker.BracketOuterRadius()) / 2};

    auto [initialDirection, initialCentroid, clusterLists] = hitData;

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
    const double initialTheta{};

    ROOT::Minuit2::Minuit2Minimizer minimizer;
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

            std::unordered_set<int> processedSiPMIDs;
            for (int i{}; i < std::ssize(clusterLists); ++i) {
                auto&& cluster = clusterLists[i];
                for (auto&& hit : cluster) {
                    auto rLayer{fiberMap[Get<"FiberID">(*hit)].radius};
                    if (processedSiPMIDs.count(Get<"FiberID">(*hit)) > 0) {
                        continue;
                    }
                    processedSiPMIDs.insert(Get<"FiberID">(*hit));
                    double rotationAngle{fiberMap[Get<"FiberID">(*hit)].rotationAngle};

                    if (fiberMap.at(Get<"FiberID">(*hit)).layerType == "LHelical") {
                        double b{sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        double minDistance;
                        std::tie(minDistance, t, theta) = this->FindHLMinDistanceSquare(rLayer, b, rotationAngle,
                                                                                        s0, dir, localInitialT, localInitialTheta);
                        distance += minDistance;
                    } else if (fiberMap.at(Get<"FiberID">(*hit)).layerType == "RHelical") {
                        double b{-sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        double minDistance;
                        std::tie(minDistance, t, theta) = this->FindHLMinDistanceSquare(rLayer, b, rotationAngle,
                                                                                        s0, dir, localInitialT, localInitialTheta);
                        distance += minDistance;
                    } else {
                        double x0{rLayer};
                        double y0{};
                        double minDistance;
                        double x = x0 * std::cos(rotationAngle) - y0 * std::sin(rotationAngle);
                        double y = x0 * std::sin(rotationAngle) + y0 * std::cos(rotationAngle);
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
    if (minimizer.Status() != 0) {
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
