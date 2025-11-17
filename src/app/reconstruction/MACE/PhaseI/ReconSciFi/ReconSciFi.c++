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
    Mustard::Data::Output<PhaseI::Data::Track> trackOutput{"G4Run0/Track"};
    Mustard::Data::Processor processor;

    processor.Process<PhaseI::Data::SciFiSimHit>(
        ROOT::RDataFrame{"G4Run0/SciFiSimHit", fileName}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return std::tie(Get<"FiberID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"FiberID">(*hit2), Get<"t">(*hit2));
                         });

            std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>> sciFiHitData;
            for (std::ranges::subrange sciFiHitRange{event.begin(), event.begin()};
                 sciFiHitRange.begin() != event.end();
                 sciFiHitRange = {sciFiHitRange.end(), sciFiHitRange.end()}) {
                sciFiHitRange = std::ranges::equal_range(sciFiHitRange.begin(), event.end(), *Get<"FiberID">(**sciFiHitRange.begin()), std::less{},
                                                         [](auto&& hit) { return Get<"FiberID">(*hit); });
                int count = 0;
                double initialTime = *Get<"t">(**sciFiHitRange.begin());
                double endTime = initialTime + sciFiTracker.ThresholdTime();
                for (int j{}; j < std::ssize(sciFiHitRange); ++j) {
                    if (*Get<"t">(*sciFiHitRange[j]) >= initialTime and *Get<"t">(*sciFiHitRange[j]) < endTime) {
                        initialTime = *Get<"t">(*sciFiHitRange[j]);
                        count++;
                        if (count == sciFiTracker.SiPMOpticalPhotonCountThreshold()) {
                            endTime = initialTime + sciFiTracker.TimeWindow();
                            auto sciFiHit{std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>()};
                            *Get<"t">(*sciFiHit) = *Get<"t">(*sciFiHitRange[j]);
                            *Get<"EvtID">(*sciFiHit) = *Get<"EvtID">(*sciFiHitRange[j]);
                            *Get<"FiberID">(*sciFiHit) = *Get<"FiberID">(*sciFiHitRange[j]);
                            sciFiHitData.emplace_back(std::move(sciFiHit));
                            while (j < std::ssize(sciFiHitRange) and *Get<"t">(*sciFiHitRange[j]) < endTime) {
                                count++;
                                j++;
                            }
                            *Get<"nOptPho">(*sciFiHitData.back()) = count;
                            count = 0;
                            if (j < std::ssize(sciFiHitRange)) {
                                initialTime = endTime + sciFiTracker.SiPMDeadTime();
                                endTime = initialTime + sciFiTracker.ThresholdTime();
                            }
                        }
                    } else if (j < std::ssize(sciFiHitRange)) {
                        while (j < std::ssize(sciFiHitRange) and *Get<"t">(*sciFiHitRange[j]) < endTime) {
                            j++;
                        }

                        if (initialTime < *Get<"t">(*sciFiHitRange[j])) {
                            initialTime = *Get<"t">(*sciFiHitRange[j]);
                        }

                        endTime = initialTime + sciFiTracker.ThresholdTime();
                        count = 0;
                    } else {
                        break;
                    }
                }
            }
        });
    trackOutput.Write();
    return EXIT_SUCCESS;
}
} // namespace MACE::PhaseI::ReconSciFi
