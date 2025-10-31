#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/ReconSciFi/Algorithm.h++"

#include "muc/algorithm"

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numbers>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace MACE::PhaseI::ReconSciFi {

auto FindCrossCoordinates(double lID, double rID, double tID, int lNumber, int rNumber, int tNumber, double x0, double y0)
    -> std::vector<muc::array3d> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<muc::array3d> coordinates;
    if (lID >= 0 and rID >= 0 and tID >= 0) {
        double x1 = (x0 * std::cos(((tID) / (tNumber)) * 2 * std::numbers::pi) - y0 * std::sin((tID / (tNumber)) * 2 * std::numbers::pi));
        double y1 = (x0 * std::sin(((tID) / (tNumber)) * 2 * std::numbers::pi) + y0 * std::cos((tID / (tNumber)) * 2 * std::numbers::pi));
        double z1 =
            ((tID / tNumber) > (lID / lNumber)) ?
                (sciFiTracker.FiberLength() / 2) - ((lID + lNumber) / (lNumber) - (tID) / (tNumber)) * sciFiTracker.FiberLength() :
                (sciFiTracker.FiberLength() / 2) - ((lID / lNumber) - (tID) / (tNumber)) * sciFiTracker.FiberLength();

        double x2 = (x0 * std::cos((tID) / (tNumber) * 2 * std::numbers::pi) - y0 * std::sin((tID) / (tNumber) * 2 * std::numbers::pi));
        double y2 = (x0 * std::sin((tID) / (tNumber) * 2 * std::numbers::pi) + y0 * std::cos((tID) / (tNumber) * 2 * std::numbers::pi));
        double z2 =
            ((tID / tNumber) > ((rID + rNumber / 2.) / (rNumber))) ?
                -(sciFiTracker.FiberLength() / 2) + fmod(((rID + rNumber / 2. + rNumber)) / (rNumber) - (tID) / (tNumber), 1) * sciFiTracker.FiberLength() :
                -(sciFiTracker.FiberLength() / 2) + fmod(((rID + rNumber / 2.)) / (rNumber) - (tID) / (tNumber), 1) * sciFiTracker.FiberLength();
        double trueTID{
            [&] {
                if (std::abs(std::fmod((lID / lNumber) + (rID + rNumber / 2.) / rNumber, 2) / 2 - (tID / tNumber)) <
                    std::abs(std::fmod((lID / lNumber) + (rID - rNumber / 2.) / rNumber, 2) / 2 - (tID / tNumber))) {
                    return std::fmod((lID / lNumber) + (rID + rNumber / 2.) / rNumber, 2) / 2 * tNumber;
                }
                return std::fmod((lID / lNumber) + (rID - rNumber / 2.) / rNumber, 2) / 2 * tNumber;
            }()};
        double x3 = ((x0 * std::cos((trueTID) / (tNumber) * 2 * std::numbers::pi) - y0 * std::sin((trueTID) / (tNumber) * 2 * std::numbers::pi)));
        double y3 = ((x0 * std::sin((trueTID) / (tNumber) * 2 * std::numbers::pi) + y0 * std::cos((trueTID) / (tNumber) * 2 * std::numbers::pi)));
        double z3 = ((trueTID / tNumber) > (lID / lNumber)) ?
                        (sciFiTracker.FiberLength() / 2) - ((lID + lNumber) / (lNumber) - (trueTID) / (tNumber)) * sciFiTracker.FiberLength() :
                        (sciFiTracker.FiberLength() / 2) - ((lID / lNumber) - (trueTID) / (tNumber)) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x1, y1, z1});
        coordinates.push_back(muc::array3d{x2, y2, z2});
        coordinates.push_back(muc::array3d{x3, y3, z3});
    } else if (rID == -1) {
        double x = (x0 * std::cos((tID / tNumber) * 2 * std::numbers::pi) - y0 * std::sin((tID / tNumber) * 2 * std::numbers::pi));
        double y = (x0 * std::sin((tID / tNumber) * 2 * std::numbers::pi) + y0 * std::cos((tID / tNumber) * 2 * std::numbers::pi));
        double z =
            ((tID / tNumber) > (lID / lNumber)) ?
                (sciFiTracker.FiberLength() / 2) - ((lID + lNumber) / (lNumber) - (tID) / (tNumber)) * sciFiTracker.FiberLength() :
                (sciFiTracker.FiberLength() / 2) - ((lID / lNumber) - (tID) / (tNumber)) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x, y, z});
    } else if (lID == -1) {
        double x = (x0 * std::cos((tID / tNumber) * 2 * std::numbers::pi) - y0 * std::sin((tID / tNumber) * 2 * std::numbers::pi));
        double y = (x0 * std::sin((tID / tNumber) * 2 * std::numbers::pi) + y0 * std::cos((tID / tNumber) * 2 * std::numbers::pi));
        double z =
            ((tID / tNumber) > ((rID + rNumber / 2.) / rNumber)) ?
                -(sciFiTracker.FiberLength() / 2) + fmod(((rID + rNumber / 2. + rNumber)) / (rNumber) - (tID) / (tNumber), 1) * sciFiTracker.FiberLength() :
                -(sciFiTracker.FiberLength() / 2) + fmod(((rID + rNumber / 2.)) / (rNumber) - (tID) / (tNumber), 1) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x, y, z});

    } else if (tID == -1) {
        double trueTID1 = std::fmod((lID / lNumber) + (rID - rNumber / 2.) / rNumber, 2) / 2 * tNumber;
        double trueTID2 = std::fmod((lID / lNumber) + (rID + rNumber / 2.) / rNumber, 2) / 2 * tNumber;
        double x1 = ((x0 * std::cos((trueTID1) / (tNumber) * 2 * std::numbers::pi) - y0 * std::sin((trueTID1) / (tNumber) * 2 * std::numbers::pi)));
        double y1 = ((x0 * std::sin((trueTID1) / (tNumber) * 2 * std::numbers::pi) + y0 * std::cos((trueTID1) / (tNumber) * 2 * std::numbers::pi)));
        double z1 = ((trueTID1 / tNumber) > (lID / lNumber)) ?
                        (sciFiTracker.FiberLength() / 2) - ((lID + lNumber) / (lNumber) - (trueTID1) / (tNumber)) * sciFiTracker.FiberLength() :
                        (sciFiTracker.FiberLength() / 2) - ((lID / lNumber) - (trueTID1) / (tNumber)) * sciFiTracker.FiberLength();

        double x2 = ((x0 * std::cos((trueTID2) / (tNumber) * 2 * std::numbers::pi) - y0 * std::sin((trueTID2) / (tNumber) * 2 * std::numbers::pi)));
        double y2 = ((x0 * std::sin((trueTID2) / (tNumber) * 2 * std::numbers::pi) + y0 * std::cos((trueTID2) / (tNumber) * 2 * std::numbers::pi)));
        double z2 = ((trueTID2 / tNumber) > (lID / lNumber)) ?
                        (sciFiTracker.FiberLength() / 2) - ((lID + lNumber) / (lNumber) - (trueTID2) / (tNumber)) * sciFiTracker.FiberLength() :
                        (sciFiTracker.FiberLength() / 2) - ((lID / lNumber) - (trueTID2) / (tNumber)) * sciFiTracker.FiberLength();
        coordinates.push_back(muc::array3d{x1, y1, z1});
        coordinates.push_back(muc::array3d{x2, y2, z2});

    } else {
        Mustard::Throw<std::runtime_error>(fmt::format("Too much ID are negitive!"));
    }
    return coordinates;
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
auto InSameSubarray(Args... args) -> bool {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::unordered_set<int> targetIds = {args...};
    if (targetIds.empty()) {
        return false;
    }

    for (const auto& sub : *sciFiTracker.CombinationOfLayer()) {
        std::unordered_set<int> subSet(sub.begin(), sub.end());
        bool allFound = true;
        for (int id : targetIds) {
            if (not subSet.contains(id)) {
                allFound = false;
                break;
            }
        }
        if (allFound) {
            return true;
        }
    }
    return false;
}

auto HitNumber(std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> clusterList;
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
                           InSameSubarray(FindLayerID(Get<"SiPMID">(*hit)),
                                          FindLayerID(Get<"SiPMID">(*element))) and
                           sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) == sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"SiPMID">(*element))) and
                           std::abs((Get<"SiPMID">(*hit) - (sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))))) -
                                    (Get<"SiPMID">(*element) - (sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*element)))))) <= sciFiTracker.ClusterLength();
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

auto DividedHit(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> data0;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> lData;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> rData;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> tData;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> usedLdata;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> usedRdata;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> usedTdata;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> newLData;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> newRData;
    std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> newTData;
    for (auto&& cluster : data) {
        if (sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"SiPMID">(*cluster.front()))) == "LHelical") {
            lData.emplace_back(cluster);
        } else if (
            sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"SiPMID">(*cluster.front()))) == "RHelical") {
            rData.emplace_back(cluster);
        } else {
            tData.emplace_back(cluster);
        }
    }

    if (std::ssize(lData) != 0 and std::ssize(rData) != 0 and std::ssize(tData) != 0) {
        for (auto it1{lData.begin()}; it1 != lData.end();) {
            for (auto it2{rData.begin()}; it2 != rData.end();) {
                for (auto it3{tData.begin()}; it3 != tData.end();) {
                    double avarageLNumber{};
                    double avarageRNumber{};
                    double avarageTNumber{};
                    int lNOptPho{};
                    int rNOptPho{};
                    int tNOptPho{};
                    double lTime{};
                    double rTime{};
                    double tTime{};
                    if (not InSameSubarray(FindLayerID(Get<"SiPMID">(*it1->front())),
                                           FindLayerID(Get<"SiPMID">(*it3->front())),
                                           FindLayerID(Get<"SiPMID">(*it3->front())))) {
                        it3++;
                        continue;
                    }
                    for (auto&& hit : *it1) {
                        if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
                            avarageLNumber += Get<"nOptPho">(*hit) * 0.5;
                        }
                        if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                            avarageLNumber += Get<"nOptPho">(*hit) * (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))));
                            lNOptPho += Get<"nOptPho">(*hit);
                            lTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                        }
                    }
                    for (auto&& hit : *it2) {
                        if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
                            avarageRNumber += Get<"nOptPho">(*hit) * 0.5;
                        }
                        if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                            avarageRNumber += Get<"nOptPho">(*hit) * (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))));
                            rNOptPho += Get<"nOptPho">(*hit);
                            rTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                        }
                    }

                    for (auto&& hit : *it3) {
                        if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
                            avarageTNumber += Get<"nOptPho">(*hit) * 0.5;
                        }
                        if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                            avarageTNumber += Get<"nOptPho">(*hit) * (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))));
                            tNOptPho += Get<"nOptPho">(*hit);
                            tTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                        }
                    }
                    avarageLNumber = avarageLNumber / lNOptPho;
                    avarageRNumber = avarageRNumber / rNOptPho;
                    avarageTNumber = avarageTNumber / tNOptPho;
                    double avarageLTime = lTime / lNOptPho;
                    double avarageRTime = rTime / rNOptPho;
                    double avarageTTime = tTime / tNOptPho;
                    int lNumber = sciFiTracker.LastIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*it1->front()))) -
                                  sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*it1->front())));
                    int rNumber = sciFiTracker.LastIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*it2->front()))) -
                                  sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*it2->front())));
                    int tNumber = sciFiTracker.LastIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*it3->front()))) -
                                  sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*it3->front())));
                    if (std::abs(avarageTTime - avarageLTime) < deltaTime and
                        std::abs(avarageTTime - avarageRTime) < deltaTime and
                        (((std::fmod((avarageLNumber / lNumber) + (avarageRNumber - rNumber / 2.) / rNumber, 2) / 2 * tNumber) - avarageTNumber <= 5) or
                         ((std::fmod((avarageLNumber / lNumber) + (avarageRNumber + rNumber / 2.) / rNumber, 2) / 2 * tNumber) - avarageTNumber <= 5))) {

                        usedLdata.push_back(*it1);
                        usedRdata.push_back(*it2);
                        usedTdata.push_back(*it3);
                        data0.push_back(*it1);
                        data0.back().insert(data0.back().end(), it2->begin(), it2->end());
                        data0.back().insert(data0.back().end(), it3->begin(), it3->end());
                    }
                    ++it3;
                }
                ++it2;
            }
            ++it1;
        }
    }

    std::ranges::set_difference(lData, usedLdata, std::back_inserter(newLData));
    std::ranges::set_difference(rData, usedRdata, std::back_inserter(newRData));
    std::ranges::set_difference(tData, usedTdata, std::back_inserter(newTData));
    if (std::ssize(newLData) != 0 and std::ssize(newRData) != 0) {
        for (auto&& cluster1 : newLData) {
            for (auto&& cluster2 : newRData) {
                if (not InSameSubarray(FindLayerID(Get<"SiPMID">(*cluster1.front())),
                                       FindLayerID(Get<"SiPMID">(*cluster2.front())))) {
                    continue;
                }
                if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime) {
                    data0.push_back(cluster1);
                    data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
                }
            }
        }
    }
    if (std::ssize(newLData) != 0 and std::ssize(newTData) != 0) {

        for (auto&& cluster1 : newLData) {
            for (auto&& cluster2 : newTData) {
                if (not InSameSubarray(FindLayerID(Get<"SiPMID">(*cluster1.front())),
                                       FindLayerID(Get<"SiPMID">(*cluster2.front())))) {
                    continue;
                }
                if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime) {
                    data0.push_back(cluster1);
                    data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
                }
            }
        }
    }

    if (std::ssize(newRData) != 0 and std::ssize(newTData) != 0) {

        for (auto&& cluster1 : newRData) {
            for (auto&& cluster2 : newTData) {
                if (not InSameSubarray(FindLayerID(Get<"SiPMID">(*cluster1.front())),
                                       FindLayerID(Get<"SiPMID">(*cluster2.front())))) {
                    continue;
                }
                if (std::abs(Get<"t">(*cluster1.front()) - Get<"t">(*cluster2.front())) < deltaTime) {
                    data0.push_back(cluster1);
                    data0.back().insert(data0.back().end(), cluster2.begin(), cluster2.end());
                }
            }
        }
    }
    return data0;
}

auto PositionTransform(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>>& data)
    -> std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>> data0;
    for (auto&& cluster : data) {
        double lID{};
        double rID{};
        double tID{};
        double lTime{};
        double rTime{};
        double tTime{};
        int lNOptPho{};
        int rNOptPho{};
        int tNOptPho{};
        int nLLayer{};
        int nRLayer{};
        int nTLayer{};
        double x0 = [&]() {
            double avaRadius{};
            for (size_t i{}; i < sciFiTracker.CombinationOfLayer()->size(); ++i) {
                const auto& sub = sciFiTracker.CombinationOfLayer()->at(i);
                if (std::ranges::find(sub, FindLayerID(Get<"SiPMID">(*cluster.front()))) != sub.end()) {
                    for (auto&& id : sciFiTracker.CombinationOfLayer()->at(i)) {
                        avaRadius += sciFiTracker.RLayer()->at(id);
                    }
                    return avaRadius / sciFiTracker.CombinationOfLayer()->at(i).size();
                }
            }
            Mustard::Throw<std::logic_error>("nLayer Out of range");
        }();
        double y0 = 0;
        for (auto&& hit : cluster) {
            if (sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) == "LHelical") {
                if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
                    lID += Get<"nOptPho">(*hit) * 0.5;
                }
                if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                    lID += Get<"nOptPho">(*hit) * (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))));
                    lTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                    lNOptPho += Get<"nOptPho">(*hit);
                }
                nLLayer = sciFiTracker.LastIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) -
                          sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit)));

            } else if (sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) == "RHelical") {
                if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
                    rID += Get<"nOptPho">(*hit) * 0.5;
                }
                if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                    rID += Get<"nOptPho">(*hit) * (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))));
                    rTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                    rNOptPho += Get<"nOptPho">(*hit);
                }
                nRLayer = sciFiTracker.LastIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) -
                          sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit)));

            } else {
                if (sciFiTracker.IsSecond()->at(FindLayerID(Get<"SiPMID">(*hit))) == 1) {
                    tID += Get<"nOptPho">(*hit) * 0.5;
                }
                if (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) != 0) {
                    tID += Get<"nOptPho">(*hit) * (Get<"SiPMID">(*hit) - sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))));
                    tTime += Get<"t">(*hit) * Get<"nOptPho">(*hit);
                    tNOptPho += Get<"nOptPho">(*hit);
                }
                nTLayer = sciFiTracker.LastIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit))) -
                          sciFiTracker.FirstIDOfLayer()->at(FindLayerID(Get<"SiPMID">(*hit)));
            }
        }

        if (lNOptPho != 0 and rNOptPho != 0 and tNOptPho != 0) {
            double avarageLID = lID / lNOptPho;
            double avarageRID = rID / rNOptPho;
            double avarageTID = tID / tNOptPho;
            double avarageLTime = lTime / lNOptPho;
            double avarageRTime = rTime / rNOptPho;
            double avarageTTime = tTime / tNOptPho;
            data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>());
            *Get<"t">(*data0.back()) = (avarageLTime * (lNOptPho) + avarageRTime * (rNOptPho) + avarageTTime * (tNOptPho)) / (lNOptPho + rNOptPho + tNOptPho);
            *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
            auto coordinates{FindCrossCoordinates(avarageLID, avarageRID, avarageTID, nLLayer, nRLayer, nTLayer, x0, y0)};
            Get<"x">(*data0.back()) = muc::array3d{
                (coordinates[0][0] * (lNOptPho + tNOptPho) + coordinates[1][0] * (rNOptPho + tNOptPho) + coordinates[2][0] * (lNOptPho + rNOptPho)) / (2 * (lNOptPho + rNOptPho + tNOptPho)),
                (coordinates[0][1] * (lNOptPho + tNOptPho) + coordinates[1][1] * (rNOptPho + tNOptPho) + coordinates[2][1] * (lNOptPho + rNOptPho)) / (2 * (lNOptPho + rNOptPho + tNOptPho)),
                (coordinates[0][2] * (lNOptPho + tNOptPho) + coordinates[1][2] * (rNOptPho + tNOptPho) + coordinates[2][2] * (lNOptPho + rNOptPho)) / (2 * (lNOptPho + rNOptPho + tNOptPho))};
            // std::cout << Get<"x">(*data0.back())[0] << " " << Get<"x">(*data0.back())[1] << " " << Get<"x">(*data0.back())[2] << std::endl;
        } else if (rNOptPho != 0 and tNOptPho != 0) {
            // std::cout << "case2" << std::endl;
            double avarageRID = rID / rNOptPho;
            double avarageTID = tID / tNOptPho;
            double avarageRTime = rTime / rNOptPho;
            double avarageTTime = tTime / tNOptPho;
            data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>());
            *Get<"t">(*data0.back()) = (avarageRTime * (rNOptPho) + avarageTTime * (tNOptPho)) / (rNOptPho + tNOptPho);
            *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
            auto coordinates{FindCrossCoordinates(-1, avarageRID, avarageTID, nLLayer, nRLayer, nTLayer, x0, y0)};
            for (auto&& coordinate : coordinates) {
                Get<"x">(*data0.back()) = coordinate;
            }
        } else if (lNOptPho != 0 and tNOptPho != 0) {
            double avarageLID = lID / lNOptPho;
            double avarageTID = tID / tNOptPho;
            double avarageLTime = lTime / lNOptPho;
            double avarageTTime = tTime / tNOptPho;
            data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>());
            *Get<"t">(*data0.back()) = (avarageLTime * (lNOptPho) + avarageTTime * (tNOptPho)) / (lNOptPho + tNOptPho);
            *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
            auto coordinates{FindCrossCoordinates(avarageLID, -1, avarageTID, nLLayer, nRLayer, nTLayer, x0, y0)};
            for (auto&& coordinate : coordinates) {
                Get<"x">(*data0.back()) = coordinate;
            }
        } else if (lNOptPho != 0 and rNOptPho != 0) {
            double avarageLID = lID / lNOptPho;
            double avarageRID = rID / rNOptPho;
            double avarageLTime = lTime / lNOptPho;
            double avarageRTime = rTime / rNOptPho;

            auto coordinates{FindCrossCoordinates(avarageLID, avarageRID, -1, nLLayer, nRLayer, nTLayer, x0, y0)};
            for (auto&& coordinate : coordinates) {
                data0.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>());
                *Get<"t">(*data0.back()) = (avarageLTime * (lNOptPho) + avarageRTime * (rNOptPho)) / (lNOptPho + rNOptPho);
                *Get<"EvtID">(*data0.back()) = *Get<"EvtID">(*cluster.front());
                Get<"x">(*data0.back()) = coordinate;
            }
        }
    }
    return data0;
}

} // namespace MACE::PhaseI::ReconSciFi
