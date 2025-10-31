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
                    if (*Get<"t">(*siPMHitRange[j]) >= initialTime and *Get<"t">(*siPMHitRange[j]) < endTime) {
                        initialTime = *Get<"t">(*siPMHitRange[j]);
                        count++;
                        if (count == sciFiTracker.Threshold()) {
                            endTime = initialTime + sciFiTracker.TimeWindow();

                            siPMHitData.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>());
                            *Get<"t">(*siPMHitData.back()) = *Get<"t">(*siPMHitRange[j]);
                            *Get<"EvtID">(*siPMHitData.back()) = *Get<"EvtID">(*siPMHitRange[j]);
                            *Get<"SiPMID">(*siPMHitData.back()) = *Get<"SiPMID">(*siPMHitRange[j]);

                            while ([&] {
                                if ((j) >= std::ssize(siPMHitRange)) {
                                    return false;
                                }
                                return ((j) < std::ssize(siPMHitRange) and *Get<"t">(*siPMHitRange[j]) < endTime);
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
                            }
                            return ((j) < std::ssize(siPMHitRange) and *Get<"t">(*siPMHitRange[j]) < endTime);
                        }()) {
                            if (j < std::ssize(siPMHitRange)) {
                                j++;
                            }
                        }

                        if (j < std::ssize(siPMHitRange)) {
                            if (initialTime < *Get<"t">(*siPMHitRange[j])) {
                                initialTime = *Get<"t">(*siPMHitRange[j]);
                            }
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
            auto positionData{PositionTransform(divHit)};
            reconTrack.Fill(std::move(positionData));
        });
    reconTrack.Write();
    return EXIT_SUCCESS;
}
} // namespace MACE::PhaseI::ReconSciFi
