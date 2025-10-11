#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/ReconSciFi/Algorithm.h++"

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Minuit2/Minuit2Minimizer.h"

#include "muc/algorithm"
#include "muc/math"

#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numbers>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace MACE::PhaseI::ReconSciFi {

auto Helix(double theta, double r, double b, double rotationAngle) -> muc::array3d {
    return {-std::copysign(r, b) * std::cos(theta + rotationAngle), -std::copysign(r, b) * std::sin(theta + rotationAngle), b * (theta)};
}

auto LinePoint(double t, const muc::array3d s0, const muc::array3d d) -> muc::array3d {
    return muc::array3d{
        s0[0] + t * d[0],
        s0[1] + t * d[1],
        s0[2] + t * d[2]};
}

auto FindCrossCoordinates(double lAngle, double rAngle, double tAngle, double rLLayer, double rRLayer, double rTLayer, double y0, const muc::array3d dir)
    -> std::vector<muc::array3d> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<muc::array3d> coordinates;
    double x0;

    if (lAngle >= 0 and rAngle >= 0 and tAngle >= 0) {
        x0 = rLLayer;
        double rDir{dir[0] * std::cos(tAngle) + dir[1] * std::sin(tAngle)};
        double phiDir{-dir[0] * std::sin(tAngle) + dir[1] * std::cos(tAngle)};
        double theta;
        if (dir[0] != 0 and dir[1] != 0 and dir[2] != 0) {
            theta = (rLLayer - rTLayer) / rDir * phiDir / rLLayer;
        } else {
            theta = 0;
        }
        // std::cout << dir[0] << " " << dir[1] << " " << dir[2] << std::endl;
        tAngle += theta;

        double x1 = x0 * std::cos(tAngle) - y0 * std::sin(tAngle);
        double y1 = x0 * std::sin(tAngle) + y0 * std::cos(tAngle);
        double z1 =
            (tAngle > lAngle) ?
                (sciFiTracker.FiberLength() / 2) - (lAngle + 2 * std::numbers::pi - tAngle) / (2 * std::numbers::pi) * sciFiTracker.FiberLength() :
                (sciFiTracker.FiberLength() / 2) - (lAngle - tAngle) / (2 * std::numbers::pi) * sciFiTracker.FiberLength();

        x0 = rRLayer;
        tAngle -= theta;
        if (dir[0] != 0 and dir[1] != 0 and dir[2] != 0) {
            theta = (rRLayer - rTLayer) / rDir * phiDir / rRLayer;
        } else {
            theta = 0;
        }
        tAngle += theta;
        // std::cout << x0 << std::endl;
        double x2 = x0 * std::cos(tAngle) - y0 * std::sin(tAngle);
        double y2 = x0 * std::sin(tAngle) + y0 * std::cos(tAngle);
        double z2 =
            (tAngle > rAngle + std::numbers::pi) ?
                -(sciFiTracker.FiberLength() / 2) + fmod(rAngle + 3 * std::numbers::pi - tAngle, 2 * std::numbers::pi) / (2 * std::numbers::pi) * sciFiTracker.FiberLength() :
                -(sciFiTracker.FiberLength() / 2) + fmod(rAngle + std::numbers::pi - tAngle, 2 * std::numbers::pi) / (2 * std::numbers::pi) * sciFiTracker.FiberLength();

        double trueTID1 = std::fmod(lAngle + rAngle - std::numbers::pi, 4 * std::numbers::pi) / 2;
        double trueTID2 = std::fmod(lAngle + rAngle + std::numbers::pi, 4 * std::numbers::pi) / 2;
        if (trueTID1 < 0) {
            trueTID1 += 2 * std::numbers::pi;
        }
        // std::cout << lAngle / lNumber << " " << rAngle / rNumber << " " << tAngle / tNumber << std::endl;
        double trueTID{
            [&] {
                if (std::fabs(trueTID2 - tAngle) <
                    std::fabs(trueTID1 - tAngle))
                    return trueTID2;
                else
                    return trueTID1;
            }()};
        x0 = (rLLayer + rRLayer) / 2;
        // std::cout << x0 << std::endl;
        double x3 = x0 * std::cos(trueTID) - y0 * std::sin(trueTID);
        double y3 = x0 * std::sin(trueTID) + y0 * std::cos(trueTID);
        double z3 = (trueTID > lAngle) ?
                        (sciFiTracker.FiberLength() / 2) - (lAngle + 2 * std::numbers::pi - trueTID) / (2 * std::numbers::pi) * sciFiTracker.FiberLength() :
                        (sciFiTracker.FiberLength() / 2) - (lAngle - trueTID) / (2 * std::numbers::pi) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x1, y1, z1});
        coordinates.push_back(muc::array3d{x2, y2, z2});
        coordinates.push_back(muc::array3d{x3, y3, z3});
        // std::cout << x1 << " " << y1 << " " << z1 << " " << std::endl
        //           << x2 << " " << y2 << " " << z2 << " " << std::endl
        //           << x3 << " " << y3 << " " << z3 << " " << std::endl;
        // std::cout << x0 << std::endl;
        return coordinates;
    } else if (rAngle == -1) {
        x0 = rLLayer;
        double rDir{dir[0] * std::cos(tAngle) + dir[1] * std::sin(tAngle)};
        double phiDir{-dir[0] * std::sin(tAngle) + dir[1] * std::cos(tAngle)};
        double theta;
        if (dir[0] != 0 and dir[1] != 0 and dir[2] != 0) {
            theta = (rLLayer - rTLayer) / rDir * phiDir / rLLayer;
        } else {
            theta = 0;
        }
        tAngle += theta;
        double x = x0 * std::cos(tAngle) - y0 * std::sin(tAngle);
        double y = x0 * std::sin(tAngle) + y0 * std::cos(tAngle);
        double z =
            (tAngle > lAngle) ?
                (sciFiTracker.FiberLength() / 2) - (lAngle + 2 * std::numbers::pi - tAngle) / (2 * std::numbers::pi) * sciFiTracker.FiberLength() :
                (sciFiTracker.FiberLength() / 2) - (lAngle - tAngle) / (2 * std::numbers::pi) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x, y, z});
        // std::cout << x0 << std::endl;
        return coordinates;
    } else if (lAngle == -1) {
        x0 = rRLayer;
        double rDir{dir[0] * std::cos(tAngle) + dir[1] * std::sin(tAngle)};
        double phiDir{-dir[0] * std::sin(tAngle) + dir[1] * std::cos(tAngle)};
        double theta{};
        if (dir[0] != 0 and dir[1] != 0 and dir[2] != 0) {
            theta = (rRLayer - rTLayer) / rDir * phiDir / rRLayer;
        } else {
            theta = 0;
        }
        tAngle += theta;
        double x = x0 * std::cos(tAngle) - y0 * std::sin(tAngle);
        double y = x0 * std::sin(tAngle) + y0 * std::cos(tAngle);
        double z =
            (tAngle > rAngle + std::numbers::pi) ?
                -(sciFiTracker.FiberLength() / 2) + fmod(rAngle + 3 * std::numbers::pi - tAngle, 2 * std::numbers::pi) / (2 * std::numbers::pi) * sciFiTracker.FiberLength() :
                -(sciFiTracker.FiberLength() / 2) + fmod(rAngle + std::numbers::pi - tAngle, 2 * std::numbers::pi) / (2 * std::numbers::pi) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x, y, z});
        // std::cout << x0 << std::endl;
        return coordinates;
    } else if (tAngle == -1) {
        x0 = (rLLayer + rRLayer) / 2;
        double trueTID1 = std::fmod(lAngle + rAngle - std::numbers::pi, 4 * std::numbers::pi) / 2;
        double trueTID2 = std::fmod(lAngle + rAngle + std::numbers::pi, 4 * std::numbers::pi) / 2;
        double x1 = x0 * std::cos(trueTID1) - y0 * std::sin(trueTID1);
        double y1 = x0 * std::sin(trueTID1) + y0 * std::cos(trueTID1);
        double z1 = (trueTID1 > lAngle) ?
                        (sciFiTracker.FiberLength() / 2) - (lAngle + 2 * std::numbers::pi - trueTID1) / (2 * std::numbers::pi) * sciFiTracker.FiberLength() :
                        (sciFiTracker.FiberLength() / 2) - (lAngle - trueTID1) / (2 * std::numbers::pi) * sciFiTracker.FiberLength();

        double x2 = x0 * std::cos(trueTID2) - y0 * std::sin(trueTID2);
        double y2 = x0 * std::sin(trueTID2) + y0 * std::cos(trueTID2);
        double z2 = (trueTID2 > lAngle) ?
                        (sciFiTracker.FiberLength() / 2) - (lAngle + 2 * std::numbers::pi - trueTID2) / (2 * std::numbers::pi) * sciFiTracker.FiberLength() :
                        (sciFiTracker.FiberLength() / 2) - (lAngle - trueTID2) / (2 * std::numbers::pi) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x1, y1, z1});
        coordinates.push_back(muc::array3d{x2, y2, z2});
        return coordinates;
    } else {
        Mustard::Throw<std::runtime_error>(fmt::format("Too much ID are negitive!"));
        return coordinates;
    }
}

auto FindLayerID(int id) -> int {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    for (int i{}; i < sciFiTracker.NLayer(); i++) {
        if (sciFiTracker.FirstIDOfLayer()->at(i) > id) {
            return i - 1;
        }
    }
    return (sciFiTracker.NLayer() - 1);
}

template<typename... Args>
auto AreAdjacent(Args... args) -> bool {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::unordered_set<int> targetIDs{{(int)args...}};

    if (targetIDs.size() < 2) {
        return false;
    }

    std::vector<size_t> indices;
    indices.reserve(targetIDs.size());
    for (int id : targetIDs) {
        indices.push_back(fiberMap.at(id).layerID / 2);
    }
    std::sort(indices.begin(), indices.end());

    bool is_consecutive{
        std::adjacent_find(
            indices.begin(),
            indices.end(),
            [](size_t a, size_t b) { return std::fabs(b - a) > 1; }) == indices.end()};

    if (is_consecutive) {
        return true;
    }

    return false;
}

auto FindHLMinDistanceSqaure(
    double HelixR, // helix radius
    double HelixB, // helix alpha
    double rotationAngle,
    const muc::array3d line_p0, // point in line
    const muc::array3d line_dir,
    double initialT, double initialTheta) -> std::tuple<double, double, double> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    ROOT::Minuit2::Minuit2Minimizer minimizer;

    std::function targetFunction{[HelixR, HelixB, rotationAngle, line_p0, line_dir](const double* xx) {
        double t{xx[0]};
        double theta{xx[1]};
        muc::array3d sp_point{Helix(theta, HelixR, HelixB, rotationAngle)};
        muc::array3d line_point{LinePoint(t, line_p0, line_dir)};
        double dis{muc::hypot(sp_point[0] - line_point[0], sp_point[1] - line_point[1], sp_point[2] - line_point[2]) *
                   muc::hypot(sp_point[0] - line_point[0], sp_point[1] - line_point[1], sp_point[2] - line_point[2])};
        return dis;
    }};

    ROOT::Math::Functor f(targetFunction, 2);
    minimizer.SetFunction(f);
    minimizer.SetLimitedVariable(0, "x", initialT, 0.01, -sciFiTracker.FiberLength() / 2, sciFiTracker.FiberLength() / 2);
    minimizer.SetLimitedVariable(1, "y", initialTheta, 0.01, -1 * std::numbers::pi, std::numbers::pi);
    minimizer.Minimize();
    return std::tuple{minimizer.MinValue(), minimizer.State().Parameter(0).Value(), minimizer.State().Parameter(1).Value()};
}

auto FindLLMinDistanceSqaure(
    const muc::array3d line1_p0,
    const muc::array3d line1_dir,
    const muc::array3d line2_p0,
    const muc::array3d line2_dir) -> double {
    muc::array3d vector1{line1_p0[0] - line2_p0[0], line1_p0[1] - line2_p0[1], line1_p0[2] - line2_p0[2]};
    muc::array3d cross{
        line1_dir[1] * line2_dir[2] - line2_dir[1] * line1_dir[2],
        line1_dir[2] * line2_dir[0] - line2_dir[2] * line1_dir[0],
        line1_dir[0] * line2_dir[1] - line2_dir[0] * line1_dir[1]};
    if (std::hypot(cross[0], cross[1], cross[2]) < 1e-9) {
        double dot = ((line1_p0[0] - line2_p0[0]) * line2_dir[0] +
                      (line1_p0[1] - line2_p0[1]) * line2_dir[1] +
                      (line1_p0[2] - line2_p0[2]) * line2_dir[2]) /
                     sqrt(line2_dir[0] * line2_dir[0] + line2_dir[1] * line2_dir[1] + line2_dir[2] * line2_dir[2]);

        return ((line1_p0[0] - line2_p0[0]) * (line1_p0[0] - line2_p0[0]) +
                (line1_p0[1] - line2_p0[1]) * (line1_p0[1] - line2_p0[1]) +
                (line1_p0[2] - line2_p0[2]) * (line1_p0[2] - line2_p0[2]) - dot * dot);
        // return DLDistance(line1_p0, line2_p0, line2_dir);
    }
    double min_dis = ((vector1[0] * cross[0] + vector1[1] * cross[1] + vector1[2] * cross[2]) / std::hypot(cross[0], cross[1], cross[2])) *
                     ((vector1[0] * cross[0] + vector1[1] * cross[1] + vector1[2] * cross[2]) / std::hypot(cross[0], cross[1], cross[2]));
    // std::cout << min_dis << std::endl;
    return min_dis;
}

auto HitNumber(std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> clusterList;
    muc::timsort(data,
                 [](auto&& hit1, auto&& hit2) {
                     return std::tie(Get<"SiPMID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"SiPMID">(*hit2), Get<"t">(*hit2));
                 });
    for (auto&& hit : data) {
        const auto cluster{std::ranges::find_if(
            clusterList,
            [&](auto&& cluster) {
                return std::ranges::any_of(cluster, [&](auto&& element) {
                    return std::abs(Get<"t">(*hit) - Get<"t">(*element)) < deltaTime and
                           fiberMap.at(Get<"SiPMID">(*hit)).layerID / 2 == fiberMap.at(Get<"SiPMID">(*element)).layerID / 2 and
                           std::abs(fiberMap.at(Get<"SiPMID">(*hit)).localID -
                                    fiberMap.at(Get<"SiPMID">(*element)).localID) <= sciFiTracker.ClusterLength();
                });
            })};
        if (cluster != clusterList.end()) {
            cluster->emplace_back(hit);
        } else {
            clusterList.emplace_back().emplace_back(hit);
        }
    }
    // for (auto&& cluster : clusterList) {
    //     std::cout << "llllllll" << std::endl;
    //     for (auto&& hit : cluster) {
    //         auto layerID{FindLayerID(Get<"SiPMID">(*hit))};

    // std::cout << Get<"SiPMID">(*hit) << " " << Get<"nOptPho">(*hit) << " " << layerID << " " << fiberMap.at(Get<"SiPMID">(*hit)).localID << " " << sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) << " " << sciFiTracker.RLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) << std::endl;
    // }
    // }
    return clusterList;
}

// auto DividedHit(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>& data, double deltaTime)
//     -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> {
//     const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
//     const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> data0;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> lData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> rData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> tData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> usedLData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> usedRData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> usedTData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> newLData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> newRData;
//     std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> newTData;
//     for (auto&& cluster : data) {
//         if (fiberMap.at(Get<"SiPMID">(*cluster.front())).layerType == "LHelical") {
//             lData.emplace_back(cluster);
//         } else if (
//             fiberMap.at(Get<"SiPMID">(*cluster.front())).layerType == "RHelical") {
//             rData.emplace_back(cluster);
//         } else {
//             tData.emplace_back(cluster);
//         }
//     }

// if (std::ssize(lData) != 0 and std::ssize(rData) != 0 and std::ssize(tData) != 0) {
//     for (auto it1{lData.begin()}; it1 != lData.end();) {
//         for (auto it2{rData.begin()}; it2 != rData.end();) {
//             for (auto it3{tData.begin()}; it3 != tData.end();) {
//                 double avarageLAngle{}, avarageRAngle{}, avarageTAngle{};
//                 int lNOptPho{}, rNOptPho{}, tNOptPho{};
//                 double lTime{}, rTime{}, tTime{};

// for (auto&& hit : *it1) {
//     // if (sciFiTracker.IsSecond()->at(fiberMap.at(Get<"SiPMID">(*hit)).layerID) == 1) {
//     //     avarageLNumber += Get<"nOptPho">(*hit) * 0.5;
//     // }
//     if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
//         avarageLAngle += fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle * Get<"nOptPho">(*hit);
//         lNOptPho += Get<"nOptPho">(*hit);
//         lTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
//     }
// }
// for (auto&& hit : *it2) {
//     // if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
//     //     avarageRNumber += Get<"nOptPho">(*hit) * 0.5;
//     // }
//     if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
//         avarageRAngle += fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle * Get<"nOptPho">(*hit);
//         rNOptPho += Get<"nOptPho">(*hit);
//         rTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
//     }
// }

// for (auto&& hit : *it3) {
//     // if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
//     //     avarageTNumber += Get<"nOptPho">(*hit) * 0.5;
//     // }
//     if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
//         avarageTAngle += fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle * Get<"nOptPho">(*hit);
//         tNOptPho += Get<"nOptPho">(*hit);
//         tTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
//     }
// }
// avarageLAngle = avarageLAngle / lNOptPho;
// avarageRAngle = avarageRAngle / rNOptPho;
// avarageTAngle = avarageTAngle / tNOptPho;
// double avarageLTime = lTime / lNOptPho;
// double avarageRTime = rTime / rNOptPho;
// double avarageTTime = tTime / tNOptPho;

// if (std::abs(avarageTTime - avarageLTime) < deltaTime and
//     std::abs(avarageTTime - avarageRTime) < deltaTime and
//     AreAdjacent((Get<"SiPMID">(*it1->front())),
//                 (Get<"SiPMID">(*it2->front())),
//                 (Get<"SiPMID">(*it3->front()))) and
//     ((((std::fmod(avarageLAngle + avarageRAngle + std::numbers::pi, 4 * std::numbers::pi) / 2) - avarageTAngle) <= 0.1) or
//      (((std::fmod(avarageLAngle + avarageRAngle - std::numbers::pi, 4 * std::numbers::pi) / 2) - avarageTAngle) <= 0.1))) {

// usedLData.push_back(*it1);
// usedRData.push_back(*it2);
// usedTData.push_back(*it3);
// data0.push_back(*it1);
// data0.back().insert(data0.back().end(), it2->begin(), it2->end());
// data0.back().insert(data0.back().end(), it3->begin(), it3->end());
// }
// ++it3;
// }
// ++it2;
// }
// ++it1;
// }
// }

// std::set_difference(lData.begin(), lData.end(), usedLData.begin(), usedLData.end(), std::back_inserter(newLData));
// std::set_difference(rData.begin(), rData.end(), usedRData.begin(), usedRData.end(), std::back_inserter(newRData));
// std::set_difference(tData.begin(), tData.end(), usedTData.begin(), usedTData.end(), std::back_inserter(newTData));
// if (std::ssize(newLData) != 0 and std::ssize(rData) != 0) {

// for (auto&& cluster1 : newLData) {
//     for (auto&& cluster2 : rData) {
//         if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime and
//             AreAdjacent(Get<"SiPMID">(*cluster1.front()), Get<"SiPMID">(*cluster2.front()))) {
//             data0.push_back(cluster1);
//             data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
//         }
//     }
// }
// }

// if (std::ssize(usedLData) != 0 and std::ssize(newRData) != 0) {

// for (auto&& cluster1 : usedLData) {
//     for (auto&& cluster2 : newRData) {
//         if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime and
//             AreAdjacent(Get<"SiPMID">(*cluster1.front()), Get<"SiPMID">(*cluster2.front()))) {
//             data0.push_back(cluster1);
//             data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
//         }
//     }
// }
// }
// if (std::ssize(lData) != 0 and std::ssize(newTData) != 0) {

// for (auto&& cluster1 : lData) {
//     for (auto&& cluster2 : newTData) {
//         if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime and
//             AreAdjacent(Get<"SiPMID">(*cluster1.front()), Get<"SiPMID">(*cluster2.front()))) {
//             data0.push_back(cluster1);
//             data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
//         }
//     }
// }
// }

// if (std::ssize(newLData) != 0 and std::ssize(usedTData) != 0) {

// for (auto&& cluster1 : newLData) {
//     for (auto&& cluster2 : usedTData) {
//         if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime and
//             AreAdjacent(Get<"SiPMID">(*cluster1.front()), Get<"SiPMID">(*cluster2.front()))) {
//             data0.push_back(cluster1);
//             data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
//         }
//     }
// }
// }

// if (std::ssize(newRData) != 0 and std::ssize(tData) != 0) {

// for (auto&& cluster1 : newRData) {
//     for (auto&& cluster2 : tData) {
//         if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime and
//             AreAdjacent(Get<"SiPMID">(*cluster1.front()), Get<"SiPMID">(*cluster2.front()))) {
//             data0.push_back(cluster1);
//             data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
//         }
//     }
// }
// }

// if (std::ssize(usedRData) != 0 and std::ssize(newTData) != 0) {

// for (auto&& cluster1 : usedRData) {
//     for (auto&& cluster2 : newTData) {
//         if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime and
//             AreAdjacent(Get<"SiPMID">(*cluster1.front()), Get<"SiPMID">(*cluster2.front()))) {
//             data0.push_back(cluster1);
//             data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
//         }
//     }
// }
// }
// for (auto&& cluster : data0) {
// std::cout << "iiiiiiiii" << std::endl;
// for (auto&& hit : cluster) {
//     auto layerID1{FindLayerID(Get<"SiPMID">(*hit))};
//     auto firstID1{sciFiTracker.FirstIDOfLayer()->at(layerID1)};
//     auto lastID1{sciFiTracker.LastIDOfLayer()->at(layerID1)};
//     double rotationAngle1{(Get<"SiPMID">(*hit) - firstID1) / (lastID1 - firstID1 + 1.) * 2 * std::numbers::pi};
//     std::cout << "" << Get<"SiPMID">(*hit) << " " << rotationAngle1 << std::endl;
// }
// }

// return data0;
// }

auto DividedHit(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> data0;

    struct ClusterInfo {
        double avgTime{};
        double avgAngle{};
        int totalOptPho{};
        std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>> cluster;
        bool used{false};
    };

    std::vector<ClusterInfo> lClusters, rClusters, tClusters;
    for (auto&& cluster : data) {
        if (cluster.empty())
            continue;

        int firstSiPMID{Get<"SiPMID">(*cluster.front())};
        auto layerType{fiberMap.at(firstSiPMID).layerType};

        ClusterInfo info;
        info.cluster = cluster;

        double totalTime{};
        double totalAngle{};
        int totalOptPho{};

        for (auto&& hit : cluster) {
            auto sipmID{Get<"SiPMID">(*hit)};

            if (fiberMap.at(sipmID).localID != 0) {
                double angle{fiberMap.at(sipmID).rotationAngle};
                int nOptPho{Get<"nOptPho">(*hit)};
                totalTime += Get<"t">(*hit) * nOptPho;
                totalAngle += angle * nOptPho;
                totalOptPho += nOptPho;
            }
        }

        if (totalOptPho > 0) {
            info.avgTime = totalTime / totalOptPho;
            info.avgAngle = totalAngle / totalOptPho;
        }
        info.totalOptPho = totalOptPho;

        if (layerType == "LHelical") {
            lClusters.push_back(info);
        } else if (layerType == "RHelical") {
            rClusters.push_back(info);
        } else {
            tClusters.push_back(info);
        }
    }

    // sort by time
    muc::timsort(lClusters,
                 [](auto&& cluster1, auto&& cluster2) {
                     return cluster1.avgTime > cluster2.avgTime;
                 });
    muc::timsort(rClusters,
                 [](auto&& cluster1, auto&& cluster2) {
                     return cluster1.avgTime > cluster2.avgTime;
                 });
    muc::timsort(tClusters,
                 [](auto&& cluster1, auto&& cluster2) {
                     return cluster1.avgTime > cluster2.avgTime;
                 });

    auto findCandidates = [&](std::vector<ClusterInfo>& clusters, double time) {
        std::vector<ClusterInfo*> result;
        auto it = std::lower_bound(clusters.begin(), clusters.end(), time - deltaTime,
                                   [](const ClusterInfo& info, double val) { return info.avgTime < val; });

        for (; it != clusters.end() && it->avgTime <= time + deltaTime; ++it) {
            result.push_back(&(*it));
        }
        return result;
    };

    for (auto& t : tClusters) {
        auto lCandidates{findCandidates(lClusters, t.avgTime)};
        auto rCandidates{findCandidates(rClusters, t.avgTime)};
        for (auto l : lCandidates) {
            for (auto r : rCandidates) {
                if (l->used or r->used or t.used) {
                    continue;
                }
                if (std::abs(l->avgTime - t.avgTime) < deltaTime and
                    std::abs(r->avgTime - t.avgTime) < deltaTime and
                    AreAdjacent(Get<"SiPMID">(*l->cluster.front()), Get<"SiPMID">(*r->cluster.front()), Get<"SiPMID">(*t.cluster.front()))) {

                    double angleCondition1{std::fmod(l->avgAngle + r->avgAngle + std::numbers::pi, 4 * std::numbers::pi) / 2};
                    double angleCondition2{std::fmod(l->avgAngle + r->avgAngle - std::numbers::pi, 4 * std::numbers::pi) / 2};

                    if (std::abs(angleCondition1 - t.avgAngle) <= 0.1 or
                        std::abs(angleCondition2 - t.avgAngle) <= 0.1) {

                        auto newCluster{l->cluster};
                        newCluster.insert(newCluster.end(), r->cluster.begin(), r->cluster.end());
                        newCluster.insert(newCluster.end(), t.cluster.begin(), t.cluster.end());
                        data0.push_back(newCluster);

                        l->used = true;
                        r->used = true;
                        t.used = true;
                    }
                }
            }
        }
    }

    auto matchPairs{[&](std::vector<ClusterInfo>& a, std::vector<ClusterInfo>& b) {
        for (auto& cluster1 : a) {
            auto candidates{findCandidates(b, cluster1.avgTime)};
            for (auto cluster2 : candidates) {
                if (cluster1.used and cluster2->used) {
                    continue;
                }

                if (std::abs(cluster1.avgTime - cluster2->avgTime) < deltaTime and
                    AreAdjacent(Get<"SiPMID">(*cluster1.cluster.front()), Get<"SiPMID">(*cluster2->cluster.front()))) {

                    auto newCluster{cluster1.cluster};
                    newCluster.insert(newCluster.end(), cluster2->cluster.begin(), cluster2->cluster.end());
                    data0.push_back(newCluster);

                    cluster1.used = true;
                    cluster2->used = true;
                }
            }
        }
    }};

    matchPairs(lClusters, rClusters);
    matchPairs(lClusters, tClusters);
    matchPairs(rClusters, tClusters);

    // for (auto&& cluster : data0) {
    //     std::cout << "cluster" << std::endl;
    //     for (auto&& hit : cluster) {
    //         std::cout << Get<"SiPMID">(*hit) << " " << fiberMap.at(Get<"SiPMID">(*hit)).layerType << " " << fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle << std::endl;
    //     }
    // }

    return data0;
}

auto DivPoint(std::vector<std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>> data)
    -> std::vector<std::vector<std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<std::vector<std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>>> divData;
    for (auto&& pair1 : data) {
        auto [x1, cluster1] = pair1;
        const auto pairCluster{std::ranges::find_if(
            divData,
            [&](auto&& pairCluster) {
                // auto [x2, cluster2] = pairCluster.front();
                // return true;
                return std::ranges::any_of(pairCluster,
                                           [&](auto&& element) {
                                            auto [x2, cluster2]{element};
                                            double deltatheta = std::fabs(std::atan2(x1[1], x1[0]) -
                                                                          std::atan2(x2[1], x2[0]));
                                            if(deltatheta>std::numbers::pi){
                                                deltatheta = 2 * std::numbers::pi - deltatheta;
                                            }
                                            return std::fabs(x1[2] - x2[2]) < sciFiTracker.CentroidZThreshold() and
                                                   deltatheta < sciFiTracker.CentroidThetaThreshold(); });
            })};
        if (pairCluster != divData.end()) {
            pairCluster->emplace_back(pair1);
        } else {
            divData.emplace_back().emplace_back(pair1);
        }
    }
    return divData;
}

auto DirectionFit(std::vector<muc::array3d>& points)
    -> std::tuple<muc::array3d, muc::array3d, double> {
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    int number{};
    // calculate centroid
    Eigen::Vector3d centroid = Eigen::Vector3d::Zero();
    for (const auto& p : points) {
        // std::cout << Get<"x">(*p)[0] << " " << Get<"x">(*p)[1] << " " << Get<"x">(*p)[2] << std::endl;
        Eigen::Vector3d c;
        c.x() = p[0];
        c.y() = p[1];
        c.z() = p[2];
    }
    for (const auto& p : points) {
        // std::cout << Get<"x">(*p)[0] << " " << Get<"x">(*p)[1] << " " << Get<"x">(*p)[2] << std::endl;
        if (p[0] * centroid.x() + p[1] * centroid.y() + p[2] * centroid.z() >= 0) {
            // double normp = std::hypot(Get<"x">(*p)[0], Get<"x">(*p)[1], Get<"x">(*p)[2]);
            Eigen::Vector3d c;
            c.x() = p[0];
            c.y() = p[1];
            c.z() = p[2];
            centroid += c;
            number++;
        }
    }
    // std::cout << number << std::endl;
    if (number < 2)
        return std::tuple(
            muc::array3d{nan, nan, nan},
            muc::array3d{nan, nan, nan},
            nan);

    if (number == 2) {
        Eigen::Vector3d direction{0, 0, 0};
        centroid /= number;
        for (const auto& p : points) {
            if ((p[0] * centroid.x() + p[1] * centroid.y() + p[2] * centroid.z()) > 0) {
                // double normp = std::hypot(Get<"x">(*p)[0], Get<"x">(*p)[1], Get<"x">(*p)[2]);
                if (std::fabs(p[0]) - std::fabs(centroid.x()) > 0) {
                    direction.x() += p[0];
                    direction.y() += p[1];
                    direction.z() += p[2];
                } else {
                    direction.x() -= p[0];
                    direction.y() -= p[1];
                    direction.z() -= p[2];
                }
            }
        }
        muc::array3d c, v;
        c[0] = centroid.x();
        c[1] = centroid.y();
        c[2] = centroid.z();

        v[0] = nan;
        v[1] = nan;
        v[2] = nan;
        // std::cout << "c&v" << c[0] << " " << c[1] << " " << c[2] << " " << v[0] << " " << v[1] << " " << v[2] << std::endl;
        return std::tuple{c, v, nan};
        // return std::tuple{
        //     muc::array3d{nan, nan, nan},
        //     muc::array3d{nan, nan, nan}
        // };
    }

    centroid /= number;

    Eigen::MatrixXd A(number, 3);
    int rownum{};
    for (const auto& p : points) {
        if ((p[0] * centroid.x() + p[1] * centroid.y() + p[2] * centroid.z()) >= 0 and rownum < number) {
            Eigen::Vector3d c;
            c.x() = p[0];
            c.y() = p[1];
            c.z() = p[2];
            A.row(rownum++) = c - centroid;
        }
    }
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullV);
    Eigen::Vector3d direction = svd.matrixV().col(0);

    // std::cout << svd.matrixV() << std::endl;
    if (direction[0] * centroid.x() + direction[1] * centroid.y() + direction[2] * centroid.z() < 0) {
        direction = -direction;
    }
    double chi2 = 0.0;
    for (const auto& p : points) {
        if (Eigen::Vector3d(p[0], p[1], p[2]).dot(centroid) >= 0) { // 仅考虑拟合用点
            Eigen::Vector3d point(p[0], p[1], p[2]);
            Eigen::Vector3d vec = point - centroid;
            Eigen::Vector3d cross = vec.cross(direction); // 点到直线的距离向量
            chi2 += cross.squaredNorm();                  // 累加平方距离
        }
    }
    chi2 = chi2 / ((number * 3) - 6);
    // normalized
    muc::array3d c, v;
    c[0] = centroid.x();
    c[1] = centroid.y();
    c[2] = centroid.z();
    v[0] = direction.normalized().x();
    v[1] = direction.normalized().y();
    v[2] = direction.normalized().z();
    // std::cout << "c&v" << c[0] << " " << c[1] << " " << c[2] << " " << v[0] << " " << v[1] << " " << v[2] << " " << chi2 << std::endl;
    return std::tuple{c, v, chi2};
}

auto PositionTransform(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> data, const muc::array3d dir)
    -> std::vector<std::vector<std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    std::vector<std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>> divData;
    std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>> data0;
    // constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    for (auto&& cluster : data) {
        double avarageLAngle{}, avarageRAngle{}, avarageTAngle{};
        double lTime{}, rTime{}, tTime{};
        int lNOptPho{}, rNOptPho{}, tNOptPho{};
        double rLLayer{}, rRLayer{}, rTLayer{};
        double y0 = 0;
        for (auto&& hit : cluster) {
            if (fiberMap.at(Get<"SiPMID">(*hit)).layerType == "LHelical") {
                // if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                avarageLAngle += fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle * Get<"nOptPho">(*hit);
                lNOptPho += Get<"nOptPho">(*hit);
                lTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                rLLayer += fiberMap.at(Get<"SiPMID">(*hit)).radius * Get<"nOptPho">(*hit);
                //}
            } else if (fiberMap.at(Get<"SiPMID">(*hit)).layerType == "RHelical") {
                // if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                avarageRAngle += fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle * Get<"nOptPho">(*hit);
                rNOptPho += Get<"nOptPho">(*hit);
                rTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                rRLayer += fiberMap.at(Get<"SiPMID">(*hit)).radius * Get<"nOptPho">(*hit);
                //}
            } else {
                // if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                avarageTAngle += fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle * Get<"nOptPho">(*hit);
                tNOptPho += Get<"nOptPho">(*hit);
                tTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                rTLayer += fiberMap.at(Get<"SiPMID">(*hit)).radius * Get<"nOptPho">(*hit);
                //}
            }
        }

        // std::cout << avarageLAngle / lNOptPho << " " << avarageRAngle / rNOptPho << " " << avarageTAngle / tNOptPho << std::endl;

        if (lNOptPho != 0 and rNOptPho != 0 and tNOptPho != 0) {
            avarageLAngle = avarageLAngle / lNOptPho;
            avarageRAngle = avarageRAngle / rNOptPho;
            avarageTAngle = avarageTAngle / tNOptPho;
            double avarageLTime = lTime / lNOptPho;
            double avarageRTime = rTime / rNOptPho;
            double avarageTTime = tTime / tNOptPho;
            rLLayer /= lNOptPho;
            rRLayer /= rNOptPho;
            rTLayer /= tNOptPho;
            data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>());
            *Get<"t">(*data0.back()) = (avarageLTime * (lNOptPho) + avarageRTime * (rNOptPho) + avarageTTime * (tNOptPho)) / (lNOptPho + rNOptPho + tNOptPho);
            *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
            auto coordinates{FindCrossCoordinates(avarageLAngle, avarageRAngle, avarageTAngle, rLLayer, rRLayer, rTLayer, y0, dir)};
            for (auto&& coordinate : coordinates) {
                std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> div1;
                div1.first = (coordinate);
                div1.second = (cluster);
                divData.push_back(div1);
                // Get<"x">(*data0.back()) = muc::array3d{
                //     (coordinates[0][0] * (lNOptPho + tNOptPho) + coordinates[1][0] * (rNOptPho + tNOptPho) + coordinates[2][0] * (lNOptPho + rNOptPho)) / (2 * (lNOptPho + rNOptPho + tNOptPho)),
                //     (coordinates[0][1] * (lNOptPho + tNOptPho) + coordinates[1][1] * (rNOptPho + tNOptPho) + coordinates[2][1] * (lNOptPho + rNOptPho)) / (2 * (lNOptPho + rNOptPho + tNOptPho)),
                //     (coordinates[0][2] * (lNOptPho + tNOptPho) + coordinates[1][2] * (rNOptPho + tNOptPho) + coordinates[2][2] * (lNOptPho + rNOptPho)) / (2 * (lNOptPho + rNOptPho + tNOptPho))};
                // div1.first = (muc::array3d{Get<"x">(*data0.back())[0], Get<"x">(*data0.back())[1], Get<"x">(*data0.back())[2]});
                // div1.second = (cluster);
                // divData.push_back(div1);
            }
        } else if (rNOptPho != 0 and tNOptPho != 0) {
            avarageRAngle = avarageRAngle / rNOptPho;
            avarageTAngle = avarageTAngle / tNOptPho;
            double avarageRTime = rTime / rNOptPho;
            double avarageTTime = tTime / tNOptPho;
            rRLayer /= rNOptPho;
            rTLayer /= tNOptPho;

            data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>());
            *Get<"t">(*data0.back()) = (avarageRTime * (rNOptPho) + avarageTTime * (tNOptPho)) / (rNOptPho + tNOptPho);
            *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
            auto coordinates{FindCrossCoordinates(-1, avarageRAngle, avarageTAngle, -1, rRLayer, rTLayer, y0, dir)};
            for (auto&& coordinate : coordinates) {
                Get<"x">(*data0.back()) = coordinate;
                std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> div1;
                div1.first = (coordinate);
                div1.second = (cluster);
                divData.push_back(div1);
            }
        } else if (lNOptPho != 0 and tNOptPho != 0) {
            avarageLAngle = avarageLAngle / lNOptPho;
            avarageTAngle = avarageTAngle / tNOptPho;
            double avarageLTime = lTime / lNOptPho;
            double avarageTTime = tTime / tNOptPho;
            rLLayer /= lNOptPho;
            rTLayer /= tNOptPho;

            data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>());
            *Get<"t">(*data0.back()) = (avarageLTime * lNOptPho + avarageTTime * tNOptPho) / (lNOptPho + tNOptPho);
            *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
            auto coordinates{FindCrossCoordinates(avarageLAngle, -1, avarageTAngle, rLLayer, -1, rTLayer, y0, dir)};
            for (auto&& coordinate : coordinates) {
                Get<"x">(*data0.back()) = coordinate;
                std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> div1;
                div1.first = (coordinate);
                div1.second = (cluster);
                divData.push_back(div1);
            }
        } else if (lNOptPho != 0 and rNOptPho != 0) {
            avarageLAngle = avarageLAngle / lNOptPho;
            avarageRAngle = avarageRAngle / rNOptPho;
            double avarageLTime = lTime / lNOptPho;
            double avarageRTime = rTime / rNOptPho;
            rLLayer /= lNOptPho;
            rRLayer /= rNOptPho;
            // double rLLyaer = sciFiTracker.RLayer()->at(FindLayerID((int)avarageLID));
            // double rRLyaer = sciFiTracker.RLayer()->at(FindLayerID((int)avarageRID));
            auto coordinates{FindCrossCoordinates(avarageLAngle, avarageRAngle, -1, rLLayer, rRLayer, -1, y0, dir)};
            for (auto&& coordinate : coordinates) {
                data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>());
                *Get<"t">(*data0.back()) = (avarageLTime * lNOptPho + avarageRTime * rNOptPho) / (lNOptPho + rNOptPho);
                *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
                Get<"x">(*data0.back()) = coordinate;
                std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> div1;
                div1.first = (coordinate);
                div1.second = (cluster);
                divData.push_back(div1);
            }
        }
    }
    // for (auto&& hit : divData) {
    //     std::cout << hit.first.at(0) << " " << hit.first[1] << " " << hit.first[2] << std::endl;
    // }
    auto divData1 = DivPoint(divData);
    for (auto&& clusterlist : divData1) {
        // std::cout << "div" << std::endl;
        std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>> data1;
        for (auto cluster : clusterlist) {
            data1.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>());
            Get<"x">(*data1.back()) = cluster.first;
            // std::cout << Get<"x">(*data1.back())[0] << " " << Get<"x">(*data1.back())[1] << " " << Get<"x">(*data1.back())[2] << std::endl;
        }
        // auto [temp1, temp2] = DirectionFit(data1);
    }

    // for (auto&& hit : data0) {
    // Get<"p">(*hit) = {nan, nan, nan};
    // }
    // if (data0.size() != 0)
    //     std::cout << Get<"EvtID">(*data0.back()) << std::endl;
    return divData1;
}

auto TrackFit(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>& clusterList,
              muc::array3d initialS, muc::array3d initialP, double initialChi2)
    -> std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();
    const auto& ecal{MACE::Detector::Description::ECAL::Instance()};
    auto r{sciFiTracker.BracketInnerRadius() + sciFiTracker.BracketOuterRadius() / 2};
    std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>> data0;
    std::cout << initialS[0] << " " << initialS[1] << " " << initialS[2] << std::endl;
    std::cout << initialP[0] << " " << initialP[1] << " " << initialP[2] << std::endl;
    double initialSTheta{std::acos(initialS[2] / muc::hypot(initialS[0], initialS[1], initialS[2]))};
    double initialSPhi{std::atan2(initialS[1], initialS[0])};
    double initialPTheta{std::acos(initialP[2] / muc::hypot(initialP[0], initialP[1], initialP[2]))};
    double initialPPhi{std::atan2(initialP[1], initialP[0])};
    double initialT{(-initialS[2] + r * std::cos(initialPTheta)) / initialP[2]};
    double initialTheta{};
    double minDistance{};
    // std::cout << initialS[0] << " " << initialS[1] << " " << initialS[2] << " "
    //           << initialP[0] << " " << initialP[1] << " " << initialP[2] << std::endl;
    double maxDistance = 0;
    ROOT::Minuit2::Minuit2Minimizer minimizer;
    std::function targetFunction{
        [&](const double* xx) {
            auto r{muc::hypot(initialS[0], initialS[1], initialS[2])};
            muc::array3d p0{r * std::cos(xx[1]) * std::sin(xx[0]), r * std::sin(xx[1]) * std::sin(xx[0]), r * std::cos(xx[0])};
            const muc::array3d dir{std::cos(xx[3]) * std::sin(xx[2]), std::sin(xx[3]) * std::sin(xx[2]), std::cos(xx[2])};
            double t{};
            double theta{};
            double distance{};
            std::unordered_set<int> processedSiPMIDs;
            for (int i{}; i < std::ssize(clusterList); ++i) {
                auto&& cluster = clusterList[i];
                for (auto&& hit : cluster) {
                    auto rLayer{fiberMap.at(Get<"SiPMID">(*hit)).radius};
                    if (processedSiPMIDs.count(Get<"SiPMID">(*hit)) > 0) {
                        continue;
                    }
                    processedSiPMIDs.insert(Get<"SiPMID">(*hit));
                    double rotationAngle{fiberMap.at(Get<"SiPMID">(*hit)).rotationAngle};
                    // std::cout << rotationAngle / (2 * std::numbers::pi) << std::endl;
                    if (fiberMap.at(Get<"SiPMID">(*hit)).layerType == "LHelical") {
                        double b{sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        std::tie(minDistance, t, theta) = FindHLMinDistanceSqaure(rLayer, b, rotationAngle, p0, dir, initialT, initialTheta);
                        // std::cout
                        //  << Get<"SiPMID">(*hit) << " L " << minDistance << " "
                        //  << Helix(theta, rLayer, b, rotationAngle)[0] << " "
                        //  << Helix(theta, rLayer, b, rotationAngle)[1] << " "
                        //  << Helix(theta, rLayer, b, rotationAngle)[2] << " "
                        //  << muc::hypot2(Helix(theta, rLayer, b, rotationAngle)[0] - initialS[0],
                        //                 Helix(theta, rLayer, b, rotationAngle)[1] - initialS[1],
                        //                 Helix(theta, rLayer, b, rotationAngle)[2] - initialS[2])
                        //<< " "
                        //<< rLayer << " " << b << " " << rotationAngle << " "
                        //<< std::endl;
                        distance += minDistance;
                    } else if (fiberMap.at(Get<"SiPMID">(*hit)).layerType == "RHelical") {
                        double b{-sciFiTracker.FiberLength() / (2 * std::numbers::pi)};
                        std::tie(minDistance, t, theta) = FindHLMinDistanceSqaure(rLayer, b, rotationAngle, p0, dir, initialT, initialTheta);
                        // std::cout
                        //  << Get<"SiPMID">(*hit) << " R " << minDistance << " "
                        //  << Helix(theta, rLayer, b, rotationAngle)[0] << " "
                        //  << Helix(theta, rLayer, b, rotationAngle)[1] << " "
                        //  << Helix(theta, rLayer, b, rotationAngle)[2] << " "
                        //  << muc::hypot2(Helix(theta, rLayer, b, rotationAngle)[0] - initialS[0],
                        //                 Helix(theta, rLayer, b, rotationAngle)[1] - initialS[1],
                        //                 Helix(theta, rLayer, b, rotationAngle)[2] - initialS[2])
                        //  << " "
                        //<< rLayer << " " << b << " " << rotationAngle << " "
                        //<< std::endl;
                        std::cout << "R " << minDistance << std::endl;

                        distance += minDistance;
                    } else {
                        double x0{rLayer};
                        double y0{};
                        double x = x0 * std::cos(rotationAngle) - y0 * std::sin(rotationAngle);
                        double y = x0 * std::sin(rotationAngle) + y0 * std::cos(rotationAngle);

                        distance += FindLLMinDistanceSqaure(p0, dir, {x, y, 0}, {0, 0, 1});

                        // std::cout << Get<"SiPMID">(*hit) << " T " << FindLLMinDistanceSqaure(p0, dir, {x, y, 0}, {0, 0, 1}) << " "
                        //           << " " << LinePoint(0, {x, y, 0}, {0, 0, 1})[0] << " "
                        //           << LinePoint(0, {x, y, 0}, {0, 0, 1})[1] << " "
                        //           << "" << LinePoint(0, {x, y, 0}, {0, 0, 1})[2] << " "
                        //           << " "
                        //           << x << " " << y << " " << rotationAngle << " " << std::endl;
                    }

                    initialT = t;
                    initialTheta = theta;
                }
                if (distance > maxDistance) {
                    maxDistance = std::sqrt(distance);
                }
            }
            // std::cout << distance << std::endl;
            return distance;
        }};
    // while (repeatOptimization)
    // minimizer.SetTolerance(1e-4);
    // minimizer.SetPrintLevel(1);

    ROOT::Math::Functor f(targetFunction, 4);
    minimizer.SetFunction(f);
    minimizer.SetStrategy(2);
    minimizer.SetLimitedVariable(0, "theta", initialSTheta, 1e-2, initialSTheta - std::numbers::pi / 4, initialSTheta + std::numbers::pi / 4);
    minimizer.SetLimitedVariable(1, "phi", initialSPhi, 1e-2, initialSPhi - 2 * std::numbers::pi / 4, initialSPhi + 2 * std::numbers::pi / 4);
    minimizer.SetLimitedVariable(2, "thetap", initialPTheta, 1e-2, initialPTheta - std::numbers::pi / 4, initialPTheta + std::numbers::pi / 4);
    minimizer.SetLimitedVariable(3, "phip", initialPPhi, 1e-2, initialPPhi - 2 * std::numbers::pi / 4, initialPPhi + 2 * std::numbers::pi / 4);
    minimizer.Minimize();
    // std::cout << "max: " << maxDistance << std::endl;

    // muc::array3d s{initialT * std::cos(minimizer.State().Parameter(1).Value()) * std::sin(minimizer.State().Parameter(0).Value()),
    //                initialT * std::sin(minimizer.State().Parameter(1).Value()) * std::sin(minimizer.State().Parameter(0).Value()),
    //                initialT * std::cos(minimizer.State().Parameter(0).Value())};
    // muc::array3d p{std::cos(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
    //                std::sin(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
    //                std::cos(minimizer.State().Parameter(2).Value())};

    data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>());

    muc::array3d x{r * std::cos(minimizer.State().Parameter(1).Value()) * std::sin(minimizer.State().Parameter(0).Value()),
                   r * std::sin(minimizer.State().Parameter(1).Value()) * std::sin(minimizer.State().Parameter(0).Value()),
                   r * std::cos(minimizer.State().Parameter(0).Value())};

    // muc::array3d p0{std::cos(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
    //                 std::sin(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
    //                 std::cos(minimizer.State().Parameter(2).Value())};

    // std::cout << " " << LinePoint(initialT, x0, p0)[0] << " " << LinePoint(initialT, x0, p0)[1] << " " << LinePoint(initialT, x0, p0)[2] << std::endl;
    muc::array3d p{std::cos(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
                   std::sin(minimizer.State().Parameter(3).Value()) * std::sin(minimizer.State().Parameter(2).Value()),
                   std::cos(minimizer.State().Parameter(2).Value())};

    if (p[0] * initialS[0] + p[1] * initialS[1] + p[2] * initialS[2] < 0) {
        p[0] *= -1;
        p[1] *= -1;
        p[2] *= -1;
    }
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    if (not std::isnan(initialS[0]) and not std::isnan(initialP[0]) and minimizer.Status() == 0) {
        if (clusterList.size() > 2) {
            Get<"chi2">(*data0.back()) = minimizer.MinValue() / ((3 * clusterList.size()) - 6);
        } else {
            Get<"chi2">(*data0.back()) = 0;
        }
        std::cout << Get<"EvtID">(*clusterList.front().front()) << std::endl;
        std::cout << p[0] << " " << p[1] << " " << p[2] << " " << Get<"chi2">(*data0.back()) << " " << maxDistance << std::endl;
        Get<"x">(*data0.back()) = x;
        Get<"p">(*data0.back()) = p;
        Get<"EvtID">(*data0.back()) = Get<"EvtID">(*clusterList.front().front());
        // Get<"x">(*data0.back()) = initialS;
        // Get<"p">(*data0.back()) = initialP;
    } else {
        Get<"chi2">(*data0.back()) = initialChi2;
        // std::cout << Get<"EvtID">(*clusterList.front().front()) << std::endl;
        // std::cout << "i " << initialP[0] << " " << initialP[1] << " " << initialP[2] << " " << Get<"chi2">(*data0.back()) << std::endl;
        Get<"x">(*data0.back()) = initialS;
        Get<"p">(*data0.back()) = initialP;
        Get<"EvtID">(*data0.back()) = Get<"EvtID">(*clusterList.front().front());
        // Get<"x">(*data0.back()) = {nan, nan, nan};
        // Get<"p">(*data0.back()) = {nan, nan, nan};
    }
    return data0;
}

} // namespace MACE::PhaseI::ReconSciFi
