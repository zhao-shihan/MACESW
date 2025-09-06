#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SensorRawHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"

#include "Mustard/CLI/BasicCLI.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "Algorithm.h++"
#include "ReconSciFi.h++"

#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TH1.h"
#include "TH3.h"
#include "TRandom.h"
#include "TTree.h"

#include "muc/algorithm"
#include "muc/array"

#include "fmt/format.h"

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
#include <vector>

namespace MACE::PhaseI::ReconSciFi {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::literals;

ReconSciFi::ReconSciFi() :
    Subprogram{"ReconSciFi", "Scintilating Fiber Tracker (SciFi Tracker) event reconstruction."} {}

auto ReconSciFi::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::BasicCLI<> cli;
    cli->add_argument("input").help("Input file path(s).").nargs(argparse::nargs_pattern::at_least_one);
    cli->add_argument("-t", "--input-tree").help("Input tree name.").default_value("data"s).required().nargs(1);
    cli->add_argument("-o", "--output").help("Output file path.").required().nargs(1);
    cli->add_argument("-m", "--output-mode").help("Output file creation mode.").default_value("NEW"s).required().nargs(1);
    cli->add_argument("-c", "--description").help("Description YAML file path.").nargs(1);
    Mustard::Env::MPIEnv env{argc, argv, {}};
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::string fileName{argv[1]};
    TFile file{Mustard::Parallel::ProcessSpecificPath("output.root").generic_string().c_str(), "RECREATE"};
    Mustard::Data::Output<PhaseI::Data::ReconTrack> reconTrack{"G4Run0/ReconTrack"};

    Mustard::Data::Processor processor;
    std::unordered_map<int, std::vector<int>> trueIDs;
    int testID{15999};
    double theta{}, z{}, phi{};
    int count1{};
    std::unordered_map<int, muc::array3d> truedir;
    processor.Process<PhaseI::Data::SciFiSimHit>(
        ROOT::RDataFrame{"G4Run0/SciFiHit", fileName}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) { return; }
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return std::tie(Get<"EvtID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"EvtID">(*hit2), Get<"t">(*hit2));
                         });
            for (auto&& hit : event) {
                int minid{100};
                // if (Get<"EvtID">(*hit) != testID) { continue; }
                if (Get<"TrkID">(*hit) == 1) {
                    if (sciFiTracker.TypeOfLayer()->at(FindLayerID(Get<"FiberID">(*hit))) == "RHelical") {
                        auto layerID1{FindLayerID(Get<"FiberID">(*hit))};
                        auto firstID1{sciFiTracker.FirstIDOfLayer()->at(layerID1)};
                        auto lastID1{sciFiTracker.LastIDOfLayer()->at(layerID1)};
                        trueIDs[Get<"EvtID">(*hit)].push_back(Get<"FiberID">(*hit) - (lastID1 - firstID1 + 1.) / 2);
                        trueIDs[Get<"EvtID">(*hit)].push_back(Get<"FiberID">(*hit) + (lastID1 - firstID1 + 1.) / 2);
                    } else if (Get<"Edep">(*hit) < 0.03) {
                        trueIDs[Get<"EvtID">(*hit)].push_back(Get<"FiberID">(*hit));
                        // eDepcount++;
                    }
                }
                double p0 = sqrt(Get<"p0">(*hit)[0] * Get<"p0">(*hit)[0] + Get<"p0">(*hit)[1] * Get<"p0">(*hit)[1] + Get<"p0">(*hit)[2] * Get<"p0">(*hit)[2]);
                phi = std::atan2(Get<"p0">(*hit)[1], Get<"p0">(*hit)[0]);
                z = Get<"p0">(*hit)[2] / p0;
                theta = std::acos(Get<"p0">(*hit)[2] / p0);
                if (Get<"TrkID">(*hit) < minid) {
                    truedir[Get<"EvtID">(*hit)] = {Get<"p0">(*hit)[0] / p0, Get<"p0">(*hit)[1] / p0, Get<"p0">(*hit)[2] / p0};
                    minid = Get<"TrkID">(*hit);
                }
                // std::cout << truedir[testID][0] << " " << truedir[testID][1] << " " << truedir[testID][2] << std::endl;
                //  truedir = {Get<"p0">(*hit)[0] / p0, Get<"p0">(*hit)[1] / p0, Get<"p0">(*hit)[2] / p0};
                //  std::cout << Get<"p0">(*hit)[1] / Get<"p0">(*hit)[0] << " " << theta << " " << z << std::endl;
                // double p = sqrt(Get<"p">(*hit)[0] * Get<"p">(*hit)[0] + Get<"p">(*hit)[1] * Get<"p">(*hit)[1] + Get<"p">(*hit)[2] * Get<"p">(*hit)[2]);
                // std::cout << Get<"EvtID">(*hit) << " FiberID:" << Get<"FiberID">(*hit) << " TrkID:" << Get<"TrkID">(*hit) << " " << Get<"p0">(*hit)[0] / p0 << " " << Get<"p0">(*hit)[1] / p0 << " " << Get<"p0">(*hit)[2] / p0 << std::endl;
                // std::cout << Get<"EvtID">(*hit) << " FiberID:" << Get<"FiberID">(*hit) << " TrkID:" << Get<"TrkID">(*hit) << " " << Get<"p">(*hit)[0] / p << " " << Get<"p">(*hit)[1] / p << " " << Get<"p">(*hit)[2] / p << " " << Get<"Edep">(*hit) << std::endl;
                // std::cout << Get<"EvtID">(*hit) << " FiberID:" << Get<"FiberID">(*hit) << " TrkID:" << Get<"TrkID">(*hit) << " " << Get<"x">(*hit)[0] << " " << Get<"x">(*hit)[1] << " " << Get<"x">(*hit)[2] << std::endl;
            }
            // for (auto&& id : trueIDs[15999]) {
            //     std::cout << id << std::endl;
            // }
        });

    processor.Process<PhaseI::Data::SciFiSiPMRawHit>(
        ROOT::RDataFrame{"G4Run0/SciFiSiPMHit", fileName}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return std::tie(Get<"SiPMID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"SiPMID">(*hit2), Get<"t">(*hit2));
                         });

            std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>> siPMHitData;
            for (std::ranges::subrange siPMHitRange{event.begin(), event.begin()};
                 siPMHitRange.begin() != event.end();
                 siPMHitRange = {siPMHitRange.end(), siPMHitRange.end()}) {
                siPMHitRange = std::ranges::equal_range(siPMHitRange.begin(), event.end(), *Get<"SiPMID">(**siPMHitRange.begin()), std::less{},
                                                        [](auto&& hit) { return Get<"SiPMID">(*hit); });
                int count = 0;
                double initialTime = *Get<"t">(**siPMHitRange.begin());
                double endTime = initialTime + sciFiTracker.ThresholdTime();
                for (int j{}; j < std::ssize(siPMHitRange); ++j) {
                    if (*Get<"t">(*siPMHitRange[j]) >= initialTime && *Get<"t">(*siPMHitRange[j]) < endTime) {
                        initialTime = *Get<"t">(*siPMHitRange[j]);
                        count++;
                        if (count == sciFiTracker.Threshold()) {
                            endTime = initialTime + sciFiTracker.TimeWindow();
                            // if (*Get<"EvtID">(**siPMHitRange.begin()) != testID) { continue; }
                            //  if (std::find(trueIDs[*Get<"EvtID">(**siPMHitRange.begin())].begin(),
                            //                trueIDs[*Get<"EvtID">(**siPMHitRange.begin())].end(),
                            //                *Get<"SiPMID">(*siPMHitRange[j])) == trueIDs[*Get<"EvtID">(**siPMHitRange.begin())].end()) {
                            //      continue;
                            //  }
                            siPMHitData.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>());
                            *Get<"t">(*siPMHitData.back()) = *Get<"t">(*siPMHitRange[j]);
                            *Get<"EvtID">(*siPMHitData.back()) = *Get<"EvtID">(*siPMHitRange[j]);
                            *Get<"SiPMID">(*siPMHitData.back()) = *Get<"SiPMID">(*siPMHitRange[j]);

                            while ([&] {
                                if (j >= std::ssize(siPMHitRange)) {
                                    return false;
                                } else {
                                    return (j < std::ssize(siPMHitRange) && *Get<"t">(*siPMHitRange[j]) < endTime);
                                }
                            }()) {
                                count++;
                                j++;
                            }
                            *Get<"nOptPho">(*siPMHitData.back()) = count;
                            count = 0;
                            if (j < std::ssize(siPMHitRange)) {
                                initialTime = endTime + sciFiTracker.DeadTime();
                                endTime = initialTime + sciFiTracker.ThresholdTime();
                            }
                        }
                    } else if (j < std::ssize(siPMHitRange)) {
                        while ([&] {
                            if ((j) >= std::ssize(siPMHitRange)) {
                                return false;
                            } else {
                                return ((j) < std::ssize(siPMHitRange) && *Get<"t">(*siPMHitRange[j]) < endTime);
                            }
                        }()) {
                            if (j < std::ssize(siPMHitRange)) {
                                j++;
                            }
                        }

                        if (j < std::ssize(siPMHitRange)) {
                            if (initialTime < *Get<"t">(*siPMHitRange[j]))
                                initialTime = *Get<"t">(*siPMHitRange[j]);
                        }
                        endTime = initialTime + sciFiTracker.ThresholdTime();
                        count = 0;
                    } else {
                        break;
                    }
                }
            }
            auto cluster{HitNumber(siPMHitData, sciFiTracker.ThresholdTime())};
            auto divHit{DividedHit(cluster, sciFiTracker.ThresholdTime())};
            auto initialPosition{PositionTransform(divHit, muc::array3d{})};
            if (std::ssize(siPMHitData) > 0) {
                // std::cout << *Get<"EvtID">(*siPMHitData.front()) << " " << 1 << std::endl;
            } else if (std::ssize(siPMHitData) > 0 && std::ssize(siPMHitData) < 4) {
                // std::cout << *Get<"EvtID">(*siPMHitData.front()) << " " << 0 << std::endl;
            }
            for (auto&& initialclusterList : initialPosition) {
                std::vector<muc::array3d> point;
                std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> hit;
                for (auto&& initialcluster : initialclusterList) {
                    point.push_back(initialcluster.first);
                    hit.push_back(initialcluster.second);
                }

                int count{};
                muc::array3d t{};
                while (count < 50) {
                    muc::array3d initialDir;
                    std::tie(std::ignore, initialDir, std::ignore) = DirectionFit(point);
                    // std::cout << initialDir[0] << " " << initialDir[1] << " " << initialDir[2] << std::endl;
                    point.clear();
                    // hit.clear();
                    auto position{PositionTransform(hit, initialDir)};
                    for (auto&& clusterList : position) {
                        for (auto&& cluster : clusterList) {
                            point.push_back(cluster.first);
                            // hit.push_back(cluster.second);
                        }
                    }
                    if (muc::ranges::inner_product(t, initialDir.cbegin(), 0.) > 0.99999) {
                        break;
                    }
                    t = initialDir;
                    count++;
                }
                auto [initialS, initialP, initialChi2] = DirectionFit(point);
                if (std::ssize(hit) >= 4) {
                    auto momentum = TrackFit(hit, initialS, initialP, initialChi2);
                    for (auto&& track : momentum) {
                        Get<"nfiber">(*track) = std::ssize(siPMHitData);
                    }
                    reconTrack.Fill(std::move(momentum));
                }
            }
        });
    reconTrack.Write();
    return EXIT_SUCCESS;
}
} // namespace MACE::PhaseI::ReconSciFi
