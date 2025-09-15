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
    using Result = Base::template Result<AHitPointer>;
    Result r;

    auto clusterLists{this->ClusterHits(hitData)};
    auto hitLists{this->PositionTransform(this->DividedHit(clusterLists))};
    auto coordinateMap{this->CalCoorderinates(hitLists, muc::array3d{})};
    auto dividedMap{this->DividedPoint(coordinateMap)};
    for (auto&& hitCluster : dividedMap) {
        auto initialResult = this->DirectionFit(hitCluster);
        auto result{this->TrackFit(initialResult)};
        std::vector<AHitPointer> resultData;
        for (auto&& cluster : std::get<2>(initialResult)) {
            resultData.insert(resultData.end(), cluster.begin(), cluster.end());
        }
        if (resultData.size() >= 4) {
            r.good[nextTrackID] = {resultData, result};
            nextTrackID++;
        }
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
                     return std::tie(Get<"SiPMID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"SiPMID">(*hit2), Get<"t">(*hit2));
                 });

    for (auto&& hit : hitData) {
        const auto cluster{std::ranges::find_if(
            clusterList,
            [&](auto&& cluster) {
                return std::ranges::any_of(cluster, [&](auto&& element) {
                    return std::abs(Get<"t">(*hit) - Get<"t">(*element)) < sciFiTracker.DeadTime() and
                           fiberMap[Get<"SiPMID">(*hit)].layerID / 2 == fiberMap[Get<"SiPMID">(*element)].layerID / 2 and
                           std::abs(fiberMap[Get<"SiPMID">(*hit)].localID -
                                    fiberMap[Get<"SiPMID">(*element)].localID) <= sciFiTracker.ClusterLength();
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
auto GenFitDAFFinder<ASciFiHit, ATrack>::DividedHit(const std::vector<std::vector<AHitPointer>>& hitData)
    -> const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();

    std::vector<std::vector<AHitPointer>> lCluster;
    std::vector<std::vector<AHitPointer>> rCluster;
    std::vector<std::vector<AHitPointer>> tCluster;

    for (auto&& cluster : hitData) {
        auto firstSiPMID{Get<"SiPMID">(*cluster.front())};
        auto layerType{fiberMap[firstSiPMID].layerType};

        if (layerType == "LHelical") {
            lCluster.push_back(cluster);
        } else if (layerType == "RHelical") {
            rCluster.push_back(cluster);
        } else {
            tCluster.push_back(cluster);
        }
    }
    return std::tuple{lCluster, rCluster, tCluster};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::PositionTransform(const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>>& hitData)
    -> const std::set<std::vector<std::vector<AHitPointer>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::set<std::vector<std::vector<AHitPointer>>> result;

    auto AreAdjacent = [&fiberMap](const std::vector<int>& targetIDs) -> bool {
        if (targetIDs.size() < 2) {
            return false;
        }

        std::vector<size_t> indices;
        indices.reserve(targetIDs.size());
        for (int id : targetIDs) {
            indices.push_back(fiberMap[id].layerID / 2);
        }
        std::sort(indices.begin(), indices.end());

        bool is_consecutive{
            std::adjacent_find(
                indices.begin(),
                indices.end(),
                [](size_t a, size_t b) { return std::fabs(b - a) > 1; }) == indices.end()};

        return is_consecutive;
    };

    auto calculateAvgAngle = [&fiberMap](const std::vector<AHitPointer>& cluster) -> double {
        if (cluster.empty())
            return 0.0;

        double sum = 0.0;
        for (const auto& hit : cluster) {
            auto siPMID = Get<"SiPMID">(*hit);

            sum += fiberMap[siPMID].rotationAngle;
        }
        return sum / cluster.size();
    };

    auto calculateAvgTime = [](const std::vector<AHitPointer>& cluster) -> double {
        if (cluster.empty())
            return 0.0;

        double sum = 0.0;
        for (const auto& hit : cluster) {
            sum += Get<"t">(*hit);
        }
        return sum / cluster.size();
    };

    auto [lCluster, rCluster, tCluster] = hitData;
    for (auto&& lHits : lCluster) {
        auto lAvgAngle = calculateAvgAngle(lHits);
        auto lAvgTime = calculateAvgTime(lHits);

        for (auto&& rHits : rCluster) {
            auto rAvgAngle = calculateAvgAngle(rHits);
            auto rAvgTime = calculateAvgTime(rHits);

            for (auto&& tHits : tCluster) {
                auto tAvgAngle = calculateAvgAngle(tHits);
                auto tAvgTime = calculateAvgTime(tHits);

                double angleCondition1 = std::fmod(lAvgAngle + rAvgAngle + std::numbers::pi, 4 * std::numbers::pi) / 2;
                double angleCondition2 = std::fmod(lAvgAngle + rAvgAngle - std::numbers::pi, 4 * std::numbers::pi) / 2;

                if (std::abs(lAvgTime - tAvgTime) < sciFiTracker.ThresholdTime() and
                    std::abs(rAvgTime - tAvgTime) < sciFiTracker.ThresholdTime() and
                    AreAdjacent({Get<"SiPMID">(*lHits.front()), Get<"SiPMID">(*rHits.front()), Get<"SiPMID">(*tHits.front())}) and
                    (std::abs(angleCondition1 - tAvgAngle) <= 0.1 or
                     std::abs(angleCondition2 - tAvgAngle) <= 0.1)) {
                    result.insert({lHits, rHits, tHits});
                }
            }
        }
    }

    for (auto&& lHits : lCluster) {
        for (auto&& tHits : tCluster) {
            auto lAvgTime = calculateAvgTime(lHits);
            auto tAvgTime = calculateAvgTime(tHits);
            if (std::abs(lAvgTime - tAvgTime) < sciFiTracker.ThresholdTime() and
                AreAdjacent({Get<"SiPMID">(*lHits.front()), Get<"SiPMID">(*tHits.front())})) {
                result.insert({lHits, tHits});
            }
        }
    }

    for (auto&& rHits : rCluster) {
        for (auto&& tHits : tCluster) {
            auto rAvgTime = calculateAvgTime(rHits);
            auto tAvgTime = calculateAvgTime(tHits);
            if (std::abs(rAvgTime - tAvgTime) < sciFiTracker.ThresholdTime() and
                AreAdjacent({Get<"SiPMID">(*rHits.front()), Get<"SiPMID">(*tHits.front())})) {
                result.insert({rHits, tHits});
            }
        }
    }

    for (auto&& lHits : lCluster) {
        for (auto&& rHits : rCluster) {
            auto lAvgTime = calculateAvgTime(lHits);
            auto rAvgTime = calculateAvgTime(rHits);
            if (std::abs(lAvgTime - rAvgTime) < sciFiTracker.ThresholdTime() and
                AreAdjacent({Get<"SiPMID">(*lHits.front()), Get<"SiPMID">(*rHits.front())})) {
                result.insert({lHits, rHits});
            }
        }
    }

    return result;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::CalCoorderinates(const std::set<std::vector<std::vector<AHitPointer>>>& hitData, const muc::array3d& direction)
    -> const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> {

    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> coordinateMap;

    auto calculateLeftZ = [](double lAngle, double tAngle, double fiberLength) -> double {
        return (tAngle > lAngle) ?
                   (fiberLength / 2) - (lAngle + 2 * std::numbers::pi - tAngle) / (2 * std::numbers::pi) * fiberLength :
                   (fiberLength / 2) - (lAngle - tAngle) / (2 * std::numbers::pi) * fiberLength;
    };

    auto calculateRightZ = [](double rAngle, double tAngle, double fiberLength) -> double {
        return (tAngle > rAngle + std::numbers::pi) ?
                   -(fiberLength / 2) + std::fmod(rAngle + 3 * std::numbers::pi - tAngle, 2 * std::numbers::pi) / (2 * std::numbers::pi) * fiberLength :
                   -(fiberLength / 2) + std::fmod(rAngle + std::numbers::pi - tAngle, 2 * std::numbers::pi) / (2 * std::numbers::pi) * fiberLength;
    };

    auto calculateCoordinates = [&](double lAngle, double rAngle, double tAngle,
                                    double rLLayer, double rRLayer, double rTLayer) -> std::vector<muc::array3d> {
        std::vector<muc::array3d> coords;

        if (lAngle >= 0 and rAngle >= 0 and tAngle >= 0) {

            double x0 = rLLayer;
            double rDir = direction[0] * std::cos(tAngle) + direction[1] * std::sin(tAngle);
            double phiDir = -direction[0] * std::sin(tAngle) + direction[1] * std::cos(tAngle);
            double theta = (direction[0] != 0 and direction[1] != 0 and direction[2] != 0) ? (rLLayer - rTLayer) / rDir * phiDir / rLLayer : 0;

            tAngle += theta;
            double x1 = x0 * std::cos(tAngle);
            double y1 = x0 * std::sin(tAngle);
            double z1 = calculateLeftZ(lAngle, tAngle, sciFiTracker.FiberLength());

            x0 = rRLayer;
            tAngle -= theta;
            theta = (direction[0] != 0 and direction[1] != 0 and direction[2] != 0) ? (rRLayer - rTLayer) / rDir * phiDir / rRLayer : 0;
            tAngle += theta;

            double x2 = x0 * std::cos(tAngle);
            double y2 = x0 * std::sin(tAngle);
            double z2 = calculateRightZ(rAngle, tAngle, sciFiTracker.FiberLength());

            double trueTID = [&]() {
                double trueTID1 = std::fmod(lAngle + rAngle - std::numbers::pi, 4 * std::numbers::pi) / 2;
                double trueTID2 = std::fmod(lAngle + rAngle + std::numbers::pi, 4 * std::numbers::pi) / 2;

                if (trueTID1 < 0)
                    trueTID1 += 2 * std::numbers::pi;

                return (std::fabs(trueTID2 - tAngle) < std::fabs(trueTID1 - tAngle)) ? trueTID2 : trueTID1;
            }();
            x0 = (rLLayer + rRLayer) / 2;
            double x3 = x0 * std::cos(trueTID);
            double y3 = x0 * std::sin(trueTID);
            double z3 = calculateLeftZ(lAngle, trueTID, sciFiTracker.FiberLength());

            coords = {
                {x1, y1, z1},
                {x2, y2, z2},
                {x3, y3, z3}
            };
        } else if (rAngle == -1) {
            double x0 = rLLayer;
            double rDir = direction[0] * std::cos(tAngle) + direction[1] * std::sin(tAngle);
            double phiDir = -direction[0] * std::sin(tAngle) + direction[1] * std::cos(tAngle);
            double theta = (direction[0] != 0 && direction[1] != 0 && direction[2] != 0) ? (rLLayer - rTLayer) / rDir * phiDir / rLLayer : 0;

            tAngle += theta;
            double x = x0 * std::cos(tAngle);
            double y = x0 * std::sin(tAngle);
            double z = calculateLeftZ(lAngle, tAngle, sciFiTracker.FiberLength());

            coords = {
                {x, y, z}
            };
        } else if (lAngle == -1) {
            double x0 = rRLayer;
            double rDir = direction[0] * std::cos(tAngle) + direction[1] * std::sin(tAngle);
            double phiDir = -direction[0] * std::sin(tAngle) + direction[1] * std::cos(tAngle);
            double theta = (direction[0] != 0 && direction[1] != 0 && direction[2] != 0) ? (rRLayer - rTLayer) / rDir * phiDir / rRLayer : 0;

            tAngle += theta;
            double x = x0 * std::cos(tAngle);
            double y = x0 * std::sin(tAngle);
            double z = calculateRightZ(rAngle, tAngle, sciFiTracker.FiberLength());

            coords = {
                {x, y, z}
            };
        } else if (tAngle == -1) {
            double x0 = (rLLayer + rRLayer) / 2;
            double trueTID1 = std::fmod(lAngle + rAngle - std::numbers::pi, 4 * std::numbers::pi) / 2;
            double trueTID2 = std::fmod(lAngle + rAngle + std::numbers::pi, 4 * std::numbers::pi) / 2;

            // 第一个可能的点
            double x1 = x0 * std::cos(trueTID1);
            double y1 = x0 * std::sin(trueTID1);
            double z1 = calculateLeftZ(lAngle, trueTID1, sciFiTracker.FiberLength());

            // 第二个可能的点
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
        double tAngle{-1};
        double rLLayer{}, rRLayer{}, rTLayer{};
        for (auto&& hitList : hitLists) {
            if (fiberMap[Get<"SiPMID">(*hitList.front())].layerType == "LHelical") {
                lAngle = 0;
                for (auto&& hit : hitList) {
                    lAngle += fiberMap[Get<"SiPMID">(*hit)].rotationAngle;
                    rLLayer += fiberMap[Get<"SiPMID">(*hit)].radius;
                }
                lAngle /= hitList.size();
                rLLayer /= hitList.size();
            } else if (fiberMap[Get<"SiPMID">(*hitList.front())].layerType == "RHelical") {
                rAngle = 0;
                for (auto&& hit : hitList) {
                    rAngle += fiberMap[Get<"SiPMID">(*hit)].rotationAngle;
                    rRLayer += fiberMap[Get<"SiPMID">(*hit)].radius;
                }
                rAngle /= hitList.size();
                rRLayer /= hitList.size();
            } else {
                tAngle = 0;
                for (auto&& hit : hitList) {
                    tAngle += fiberMap[Get<"SiPMID">(*hit)].rotationAngle;
                    rTLayer += fiberMap[Get<"SiPMID">(*hit)].radius;
                }
                tAngle /= hitList.size();
                rTLayer /= hitList.size();
            }
        }
        auto coordinates = calculateCoordinates(lAngle, rAngle, tAngle, rLLayer, rRLayer, rTLayer);

        for (auto&& coordinate : coordinates)
            coordinateMap[coordinate] = hitLists;
    }

    return coordinateMap;
};

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::DividedPoint(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> const std::set<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};

    std::vector<std::vector<std::pair<muc::array3d, std::vector<std::vector<AHitPointer>>>>> tempDivData;

    for (auto&& [x1, clusters] : hitData) {
        auto pairCluster = std::ranges::find_if(
            tempDivData.begin(),
            tempDivData.end(),
            [&](const auto& group) {
                return std::ranges::any_of(
                    group.begin(),
                    group.end(),
                    [&](const auto& element) {
                        const auto& [x2, cluster2] = element;
                        double theta1 = std::atan2(x1[1], x1[0]);
                        double theta2 = std::atan2(x2[1], x2[0]);
                        double deltaTheta = std::fabs(theta1 - theta2);

                        if (deltaTheta > std::numbers::pi) {
                            deltaTheta = 2 * std::numbers::pi - deltaTheta;
                        }

                        return std::fabs(x1[2] - x2[2]) < sciFiTracker.CentroidZThreshold() &&
                               deltaTheta < sciFiTracker.CentroidThetaThreshold();
                    });
            });

        if (pairCluster != tempDivData.end()) {
            pairCluster->emplace_back(x1, clusters);
        } else {
            tempDivData.emplace_back().emplace_back(x1, clusters);
        }
    }

    std::set<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>> divData;
    for (const auto& group : tempDivData) {
        std::map<muc::array3d, std::vector<std::vector<AHitPointer>>> groupMap;
        for (const auto& [coord, clusters] : group) {
            groupMap[coord] = clusters;
        }
        divData.insert(std::move(groupMap));
    }

    return divData;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::DirectionFit(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
    -> std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>> {

    std::vector<std::vector<AHitPointer>> fiberLists;

    Eigen::Vector3d centroid = Eigen::Vector3d::Zero();
    for (auto [point, hitLists] : hitData) {
        Eigen::Vector3d c;
        c.x() = point[0];
        c.y() = point[1];
        c.z() = point[2];

        if (point[0] * centroid.x() + point[1] * centroid.y() + point[2] * centroid.z() >= 0) {
            centroid += c;
        }
    }
    if (hitData.size() <= 4) {
        for (auto [direction, hitLists] : hitData) {
            fiberLists.insert(fiberLists.end(), hitLists.begin(), hitLists.end());
        }
        return std::tuple(muc::array3d{}, muc::array3d{}, fiberLists);
    }
    centroid /= hitData.size();
    Eigen::MatrixXd A(hitData.size(), 3);
    int rownum{};
    for (auto [point, hitLists] : hitData) {
        if ((point[0] * centroid.x() + point[1] * centroid.y() + point[2] * centroid.z()) >= 0) {
            Eigen::Vector3d c;
            c.x() = point[0];
            c.y() = point[1];
            c.z() = point[2];
            A.row(rownum++) = c - centroid;
            fiberLists.insert(fiberLists.end(), hitLists.begin(), hitLists.end());
        }
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullV);
    Eigen::Vector3d direction = svd.matrixV().col(0);
    direction.normalized();
    if (direction[0] * centroid.x() + direction[1] * centroid.y() + direction[2] * centroid.z() < 0) {
        direction *= -1;
    }

    return std::tuple(muc::array3d{direction.x(), direction.y(), direction.z()}, muc::array3d{centroid.x(), centroid.y(), centroid.z()}, fiberLists);
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::Helix(double theta, double r, double b, double rotationAngle) -> const muc::array3d {
    return {-std::copysign(r, b) * std::cos(theta + rotationAngle), -std::copysign(r, b) * std::sin(theta + rotationAngle), b * (theta)};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::Line(double t, const muc::array3d s0, const muc::array3d d) -> const muc::array3d {
    return muc::array3d{
        s0[0] + t * d[0],
        s0[1] + t * d[1],
        s0[2] + t * d[2]};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::FindHLMinDistanceSquare(
    double HelixR, double HelixB, double rotationAngle,
    const muc::array3d line_p0, const muc::array3d line_dir,
    double initialT, double initialTheta) -> std::tuple<double, double, double> {
    const auto& sciFiTracker = MACE::PhaseI::Detector::Description::SciFiTracker::Instance();
    ROOT::Minuit2::Minuit2Minimizer minimizer;

    auto targetFunction = [&](const double* xx) {
        muc::array3d sp_point = Helix(xx[1], HelixR, HelixB, rotationAngle);
        muc::array3d line_point = Line(xx[0], line_p0, line_dir);
        double dx = sp_point[0] - line_point[0];
        double dy = sp_point[1] - line_point[1];
        double dz = sp_point[2] - line_point[2];
        return dx * dx + dy * dy + dz * dz;
    };

    minimizer.SetFunction(ROOT::Math::Functor(targetFunction, 2));

    double halfLength = sciFiTracker.FiberLength() / 2;
    minimizer.SetLimitedVariable(0, "t", initialT, 0.01, -halfLength, halfLength);
    minimizer.SetLimitedVariable(1, "theta", initialTheta, 0.01, -std::numbers::pi, std::numbers::pi);

    minimizer.Minimize();

    return {minimizer.MinValue(),
            minimizer.X()[0],
            minimizer.X()[1]};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto GenFitDAFFinder<ASciFiHit, ATrack>::FindLLMinDistanceSquare(
    const muc::array3d line1_point, const muc::array3d line1_dir,
    const muc::array3d line2_point, const muc::array3d line2_dir) -> double {

    Eigen::Vector3d u = line1_dir;
    Eigen::Vector3d v = line2_dir;
    Eigen::Vector3d w = {line1_point[0] - line2_point[0], line1_point[1] - line2_point[1], line1_point[2] - line2_point[2]};

    double a = u.dot(u);
    double b = u.dot(v);
    double c = u.dot(v);
    double d = u.dot(w);
    double e = u.dot(w);

    double denom = a * c - b * b;

    if (denom < 1e-9) { // 平行线情况
        double t = w.dot(v) / c;
        Eigen::Vector3d closest = {w[0] - t * v[0], w[1] - t * v[1], w[2] - t * v[2]};
        return closest.dot(closest);
    }

    double t = (b * e - c * d) / denom;
    double s = (a * e - b * d) / denom;

    Eigen::Vector3d closest = {w[0] + t * u[0] - s * v[0],
                               w[1] + t * u[1] - s * v[1],
                               w[2] + t * u[2] - s * v[2]};

    return closest.dot(closest);
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
    auto r{sciFiTracker.BracketInnerRadius() + sciFiTracker.BracketOuterRadius() / 2};

    auto [initialDirection, initialCentroid, clusterLists] = hitData;

    double initialSTheta{std::acos(initialCentroid[2] / muc::hypot(initialCentroid[0], initialCentroid[1], initialCentroid[2]))};
    double initialSPhi{std::atan2(initialCentroid[1], initialCentroid[0])};
    double initialPTheta{std::acos(initialDirection[2] / muc::hypot(initialDirection[0], initialDirection[1], initialDirection[2]))};
    double initialPPhi{std::atan2(initialDirection[1], initialDirection[0])};
    double initialT{(-initialCentroid[2] + r * std::cos(initialPTheta)) / initialDirection[2]};
    double initialTheta{};
    double minDistance{};

    ROOT::Minuit2::Minuit2Minimizer minimizer;
    std::function targetFunction{
        [&](const double* xx) {
            muc::array3d p0{r * std::cos(xx[1]) * std::sin(xx[0]), r * std::sin(xx[1]) * std::sin(xx[0]), r * std::cos(xx[0])};
            const muc::array3d dir{std::cos(xx[3]) * std::sin(xx[2]), std::sin(xx[3]) * std::sin(xx[2]), std::cos(xx[2])};
            double t{};
            double theta{};
            double distance{};
            std::unordered_set<int> processedSiPMIDs;
            for (int i{}; i < std::ssize(clusterLists); ++i) {
                auto&& cluster = clusterLists[i];
                for (auto&& hit : cluster) {
                    auto rLayer{fiberMap[Get<"SiPMID">(*hit)].radius};
                    if (processedSiPMIDs.count(Get<"SiPMID">(*hit)) > 0) {
                        continue;
                    }
                    processedSiPMIDs.insert(Get<"SiPMID">(*hit));
                    double rotationAngle{fiberMap[Get<"SiPMID">(*hit)].rotationAngle};

                    if (fiberMap.at(Get<"SiPMID">(*hit)).layerType == "LHelical") {
                        double b{sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        std::tie(minDistance, t, theta) = MACE::PhaseI::ReconSciFi::FindHLMinDistanceSqaure(rLayer, b, rotationAngle, p0, dir, initialT, initialTheta);
                        distance += minDistance;
                    } else if (fiberMap.at(Get<"SiPMID">(*hit)).layerType == "RHelical") {
                        double b{-sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        std::tie(minDistance, t, theta) = MACE::PhaseI::ReconSciFi::FindHLMinDistanceSqaure(rLayer, b, rotationAngle, p0, dir, initialT, initialTheta);
                        distance += minDistance;
                    } else {
                        double x0{rLayer};
                        double y0{};
                        double x = x0 * std::cos(rotationAngle) - y0 * std::sin(rotationAngle);
                        double y = x0 * std::sin(rotationAngle) + y0 * std::cos(rotationAngle);
                        distance += MACE::PhaseI::ReconSciFi::FindLLMinDistanceSqaure(p0, dir, {x, y, 0}, {0, 0, 1});
                    }

                    initialT = t;
                    initialTheta = theta;
                }
            }
            return distance;
        }};

    ROOT::Math::Functor f(targetFunction, 4);
    minimizer.SetFunction(f);
    minimizer.SetStrategy(2);
    minimizer.SetLimitedVariable(0, "theta", initialSTheta, 1e-2, initialSTheta - std::numbers::pi / 4, initialSTheta + std::numbers::pi / 4);
    minimizer.SetLimitedVariable(1, "phi", initialSPhi, 1e-2, initialSPhi - 2 * std::numbers::pi / 4, initialSPhi + 2 * std::numbers::pi / 4);
    minimizer.SetLimitedVariable(2, "thetap", initialPTheta, 1e-2, initialPTheta - std::numbers::pi / 4, initialPTheta + std::numbers::pi / 4);
    minimizer.SetLimitedVariable(3, "phip", initialPPhi, 1e-2, initialPPhi - 2 * std::numbers::pi / 4, initialPPhi + 2 * std::numbers::pi / 4);
    minimizer.Minimize();

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
    return result;
}

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder
