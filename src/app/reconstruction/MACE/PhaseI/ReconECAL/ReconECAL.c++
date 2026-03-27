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

#include "MACE/Data/SimHit.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/PhaseI/Detector/Description/UsePhaseIDefault.h++"
#include "MACE/PhaseI/ReconECAL/ReconECAL.h++"
#include "MACE/Reconstruction/ECALClustering/Reconstructing.h++"

#include "Mustard/CLI/BasicCLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Detector/Description/DescriptionIO.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"

#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TTree.h"

#include <algorithm>
#include <optional>
#include <unordered_map>

namespace MACE::PhaseI::ReconECAL {

ReconECAL::ReconECAL() :
    Subprogram{"ReconECAL", "Electromagnetic calorimeter (ECAL) event reconstruction in PhaseI."} {}

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;
using namespace std::literals;

auto ReconECAL::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::BasicCLI<Mustard::CLI::DetectorDescriptionModule<MACE::Detector::Description::ECAL>> cli;
    auto& modeCLI{cli->add_mutually_exclusive_group()};
    modeCLI.add_argument("--single").help("Reconstruction of single-cluster event.").flag();
    modeCLI.add_argument("--double").help("Reconstruction of double-cluster events.").flag();
    modeCLI.add_argument("--triple").help("Reconstruction of triple-cluster events.").flag();
    cli->add_argument("input").help("Input file path(s).").nargs(argparse::nargs_pattern::at_least_one);
    cli->add_argument("-h", "--input-ecal-hit-tree").help("Input ECAL hit tree name.").default_value("G4Run0/ECALSimHit"s).required().nargs(1);
    cli->add_argument("-o", "--output").help("Output file path.").required().nargs(1);
    cli->add_argument("-r", "--output-tree").help("Output tree name.").default_value("G4Run0/ReconECAL"s).required().nargs(1);
    cli->add_argument("-m", "--output-mode").help("Output file creation mode.").default_value("NEW"s).required().nargs(1);
    cli->add_argument("-e", "--energy-threshold").help("Energy threshold for clustering.").default_value(50_keV).required().nargs(1);

    Mustard::Env::MPIEnv env{argc, argv, cli};
    Detector::Description::UsePhaseIDefault();
    cli.DetectorDescriptionIOIfFlagged();

    const auto& ecal{MACE::Detector::Description::ECAL::Instance()};
    const auto& moduleList{ecal.Array().moduleList};

    using ECALEnergy = Mustard::Data::TupleModel<
        Mustard::Data::Value<double, "Edep1", "Energy deposition of the 1st cluster">,
        Mustard::Data::Value<double, "Edep2", "Energy deposition of the 2nd cluster">,
        Mustard::Data::Value<double, "Edep3", "Energy deposition of the 3rd cluster">,
        // Mustard::Data::Value<double, "t", "Time of the track">,
        Mustard::Data::Value<muc::array3f, "Position1", "Position of the 1st cluster">,
        Mustard::Data::Value<muc::array3f, "Position2", "Position of the 2nd cluster">,
        Mustard::Data::Value<muc::array3f, "Position3", "Position of the 3rd cluster">,
        Mustard::Data::Value<double, "TotalEdep", "Energy deposition in total">,
        Mustard::Data::Value<double, "dE", "Energy difference of the tracks">,
        Mustard::Data::Value<double, "dt", "Time difference of the tracks">,
        Mustard::Data::Value<double, "cosTheta", "Cosine of angle between the tracks">,
        Mustard::Data::Value<double, "theta", "Angle between the tracks">>;

    const auto createEnergyTuple1{[&](const std::vector<int>& potentialSeedModule,
                                      const std::unordered_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>>& hitDict) -> std::optional<Mustard::Data::Tuple<ECALEnergy>> {
        Mustard::Data::Tuple<ECALEnergy> energyTuple;

        if (potentialSeedModule.empty()) {
            return std::nullopt;
        }

        const auto energyThreshold{cli->get<double>("--energy-threshold")};
        const auto seedModule{potentialSeedModule.begin()};
        const auto cluster{ECALClustering::Reconstructing(*seedModule, moduleList, hitDict, energyThreshold)};

        Get<"Edep1">(energyTuple) = cluster.energy;
        Get<"Position1">(energyTuple) = cluster.position;
        Get<"TotalEdep">(energyTuple) = cluster.energy;
        Get<"theta">(energyTuple) = cluster.position.theta(CLHEP::Hep3Vector{0, 0, 1});

        return energyTuple;
    }};

    const auto createEnergyTuple2{[&](const std::vector<int>& potentialSeedModule,
                                      const std::unordered_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>>& hitDict) -> std::optional<Mustard::Data::Tuple<ECALEnergy>> {
        Mustard::Data::Tuple<ECALEnergy> energyTuple;

        if (std::ssize(potentialSeedModule) < 2) {
            return std::nullopt;
        }

        const auto energyThreshold{cli->get<double>("--energy-threshold")};
        const auto firstSeedModule{potentialSeedModule.begin()};
        const auto secondSeedModule{std::ranges::find_if(
            potentialSeedModule,
            [&](int moduleID) {
                const auto& c1{moduleList.at(*firstSeedModule).centroid};
                const auto& c2{moduleList.at(moduleID).centroid};
                return c1.angle(c2) > 0.8 * pi;
            })};
        if (secondSeedModule == potentialSeedModule.end()) {
            return std::nullopt;
        }
        const auto firstCluster{ECALClustering::Reconstructing(*firstSeedModule, moduleList, hitDict, energyThreshold)};
        const auto secondCluster{ECALClustering::Reconstructing(*secondSeedModule, moduleList, hitDict, energyThreshold)};

        Get<"Edep1">(energyTuple) = firstCluster.energy;
        Get<"Edep2">(energyTuple) = secondCluster.energy;
        Get<"Position1">(energyTuple) = firstCluster.position;
        Get<"Position2">(energyTuple) = secondCluster.position;
        Get<"TotalEdep">(energyTuple) = firstCluster.energy + secondCluster.energy;
        Get<"dE">(energyTuple) = std::abs(firstCluster.energy - secondCluster.energy);
        Get<"dt">(energyTuple) = std::abs(*Get<"t">(*hitDict.at(*firstSeedModule)) - *Get<"t">(*hitDict.at(*secondSeedModule)));
        Get<"cosTheta">(energyTuple) = firstCluster.position.cosTheta(secondCluster.position);

        return energyTuple;
    }};

    TFile outputFile{Mustard::Parallel::ProcessSpecificPath(cli->get("--output").c_str()).generic_string().c_str(), cli->get("--output-mode").c_str()};
    Mustard::Data::Output<ECALEnergy> reconEnergy{cli->get("--output-tree")};
    Mustard::Data::Processor processor;

    processor.Process<Data::ECALSimHit>(
        ROOT::RDataFrame{cli->get("--input-ecal-hit-tree"), cli->get<std::vector<std::string>>("input")}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return Get<"Edep">(*hit1) > Get<"Edep">(*hit2);
                         });

            std::unordered_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>> hitDict;
            std::vector<int> potentialSeedModule;

            for (auto&& hit : event) {
                hitDict.try_emplace(Get<"ModID">(*hit), hit);
                potentialSeedModule.emplace_back(Get<"ModID">(*hit));
            }

            auto energyTuple{[&]() -> std::optional<Mustard::Data::Tuple<ECALEnergy>> {
                if (cli->get("--single") == true) {
                    return createEnergyTuple1(potentialSeedModule, hitDict);
                }
                if (cli->get("--double") == true) {
                    return createEnergyTuple2(potentialSeedModule, hitDict);
                }
                if (cli->get("--triple") == true) {
                    // To be implemented
                    return std::nullopt;
                }
                return std::nullopt;
            }()};

            if (not energyTuple) {
                return;
            }

            reconEnergy.Fill(std::move(*energyTuple));
        });
    reconEnergy.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::PhaseI::ReconECAL
