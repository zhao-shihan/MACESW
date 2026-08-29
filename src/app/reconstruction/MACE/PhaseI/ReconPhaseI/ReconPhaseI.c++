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

#include "MACE/Data/Hit.h++"
#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SensorRawHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/ReconPhaseI/ReconPhaseI.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/GenFitFinder.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitDAFFitter.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitReferenceKalmanFitter.h++"

#include "Mustard/CLI/BasicCLI.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Data/SeqProcessor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "ROOT/RDataFrame.hxx"
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"

#include "muc/algorithm"
#include "muc/array"

#include "fmt/format.h"

#include <ConstField.h>
#include <EventDisplay.h>
#include <Exception.h>
#include <FieldManager.h>
#include <KalmanFitterRefTrack.h>
#include <MaterialEffects.h>
#include <PlanarMeasurement.h>
#include <RKTrackRep.h>
#include <SpacepointMeasurement.h>
#include <StateOnPlane.h>
#include <TEveManager.h>
#include <TGeoManager.h>
#include <TGeoMaterialInterface.h>
#include <TMath.h>
#include <TVector3.h>
#include <Track.h>
#include <TrackPoint.h>
#include <WireMeasurement.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numbers>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace MACE::PhaseI::ReconPhaseI {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::literals;

ReconPhaseI::ReconPhaseI() :
    Subprogram{"ReconPhaseI", "Phase-I event reconstruction."} {}

auto ReconPhaseI::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::BasicCLI<> cli{};
    cli->add_argument("input").help("Input file path(s).").nargs(argparse::nargs_pattern::at_least_one);
    cli->add_argument("-t", "--input-tree").help("Input tree name.").default_value("data"s).required().nargs(1);
    cli->add_argument("-o", "--output").help("Output file path. If not provided, auto-generated from input name.");
    cli->add_argument("-m", "--output-mode").help("Output file creation mode.").default_value("NEW"s).required().nargs(1);
    cli->add_argument("-c", "--description").help("Description YAML file path.").nargs(1);

    cli->parse_args(argc, argv);
    const Mustard::Env::MPIEnv env{argc, argv, {}};
    const auto inputFiles{cli->get<std::vector<std::string>>("input")};
    if (inputFiles.empty()) {
        Mustard::Throw<std::runtime_error>("No input file provided.");
    }
    const std::filesystem::path inputPath{inputFiles[0]};
    const auto fileName{inputPath.stem().string()};

    const std::filesystem::path defaultOutputBase{"SciFiOutput_" + fileName + ".root"};
    const auto outputPath{
        cli->present<std::string>("--output") ? std::filesystem::path{*cli->present<std::string>("--output")} : defaultOutputBase};
    const auto outputMode{cli->get<std::string>("--output-mode")};

    const auto finalPath{Mustard::Parallel::ProcessSpecificPath(outputPath)};
    TFile file{finalPath.generic_string().c_str(), outputMode.c_str()};
    Mustard::Data::Output<PhaseI::Data::Track> reconTrack{"G4Run0/ReconTrack"};

    MACE::PhaseI::Reconstruction::SciFiTracking::GenFitFinder<MACE::PhaseI::Data::SciFiHit, MACE::PhaseI::Data::Track> finder{};
    // MACE::PhaseI::Reconstruction::SciFiTracking::GenFitReferenceKalmanFitter<MACE::PhaseI::Data::SciFiHit, MACE::PhaseI::Data::Track> fitter{0.00289};
    MACE::PhaseI::Reconstruction::SciFiTracking::GenFitDAFFitter<MACE::PhaseI::Data::SciFiHit, MACE::PhaseI::Data::Track> fitter{0.00289};
    fitter.EnableEventDisplay(false);

    Mustard::Data::SeqProcessor processor{};

    const auto ecalData{
        ROOT::RDataFrame{"G4Run0/ECALSimHit", inputFiles[0]}
    };
    const auto sciFiData{
        ROOT::RDataFrame{"G4Run0/SciFiSimHit", inputFiles[0]}
    };
    const auto ttcData{
        ROOT::RDataFrame{"G4Run0/TTCSimHit", inputFiles[0]}
    };
    processor.Process<MACE::Data::ECALHit, PhaseI::Data::SciFiSimHit, MACE::Data::TTCHit>(
        {ecalData, sciFiData, ttcData}, int{}, "EvtID",
        [&](auto&& ecalEvent, auto&& sciFiEvent, auto&& ttcEvent) {
            muc::timsort(ecalEvent,
                         [](auto&& hit1, auto&& hit2) {
                             return Get<"Edep">(*hit1) > Get<"Edep">(*hit2);
                         });
            muc::timsort(sciFiEvent,
                         [](auto&& hit1, auto&& hit2) {
                             return std::tie(Get<"FiberID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"FiberID">(*hit2), Get<"t">(*hit2));
                         });
            muc::timsort(ttcEvent,
                         [](auto&& hit1, auto&& hit2) {
                             return Get<"t">(*hit1) > Get<"t">(*hit2);
                         });

            const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
            std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>> sciFiHitData{};
            for (auto&& hit : std::as_const(sciFiEvent)) {
                if (*Get<"Edep">(*hit) <= sciFiTracker.EnergyDepositionThreshold()) {
                    continue;
                }

                auto sciFiHit{std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>()};
                *Get<"EvtID">(*sciFiHit) = *Get<"EvtID">(*hit);
                *Get<"FiberID">(*sciFiHit) = *Get<"FiberID">(*hit);
                *Get<"Edep">(*sciFiHit) = *Get<"Edep">(*hit);
                *Get<"t">(*sciFiHit) = *Get<"t">(*hit);
                *Get<"TrkID">(*sciFiHit) = *Get<"TrkID">(*hit);
                sciFiHitData.emplace_back(std::move(sciFiHit));
            }

            auto nextTrackID{0};
            if (sciFiHitData.size() >= 4) {
                for (const auto& [trackID, good] : finder(sciFiHitData, nextTrackID).good) {
                    const auto track{fitter(good.hitData, good.seed).track};
                    if (track == nullptr) {
                        continue;
                    }

                    reconTrack.Fill(*track);
                    nextTrackID = trackID;
                }
            }
        });
    reconTrack.Write();
    // fitter.OpenEventDisplay();
    return EXIT_SUCCESS;
}

} // namespace MACE::PhaseI::ReconPhaseI
