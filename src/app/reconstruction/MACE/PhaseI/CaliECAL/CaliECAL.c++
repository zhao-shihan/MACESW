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
#include "MACE/Data/SimVertex.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/PhaseI/CaliECAL/CaliECAL.h++"
#include "MACE/PhaseI/Detector/Description/UsePhaseIDefault.h++"
#include "MACE/Reconstruction/ECALClustering/Reconstructing.h++"

#include "Mustard/CLI/BasicCLI.h++"
#include "Mustard/CLI/Module/DetectorDescriptionModule.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Data/SeqProcessor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Detector/Description/DescriptionIO.h++"
#include "Mustard/Env/BasicEnv.h++"
#include "Mustard/IO/File.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"

#include "CLHEP/Vector/ThreeVector.h"

#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TTree.h"

#include "fmt/format.h"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace MACE::PhaseI::CaliECAL {

CaliECAL::CaliECAL() :
    Subprogram{"CaliECAL", "Electromagnetic calorimeter (ECAL) event reconstruction for calibration in PhaseI."} {}

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;
using namespace std::literals;

auto CaliECAL::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::BasicCLI<Mustard::CLI::DetectorDescriptionModule<MACE::Detector::Description::ECAL>> cli;
    cli->add_argument("input").help("Input file path(s).").nargs(argparse::nargs_pattern::at_least_one);
    cli->add_argument("-p", "--input-primary-vertex-tree").help("Input primary vertex tree name.").default_value("G4Run0/SimPrimaryVertex"s).required().nargs(1);
    cli->add_argument("-t", "--input-ecal-hit-tree").help("Input ECAL hit tree name.").default_value("G4Run0/ECALSimHit"s).required().nargs(1);
    cli->add_argument("-o", "--output").help("Output file path.").required().nargs(1);
    cli->add_argument("-r", "--output-tree").help("Output tree name.").default_value("G4Run0/CaliECAL"s).required().nargs(1);
    cli->add_argument("-m", "--output-mode").help("Output file creation mode.").default_value("NEW"s).required().nargs(1);
    cli->add_argument("-e", "--energy-threshold").help("Energy threshold for clustering.").default_value(50_keV).required().nargs(1);
    cli->add_argument("-c", "--pe-count-threshold").help("Photoelectron count threshold for clustering.").default_value(3).required().nargs(1);
    cli->add_argument("--optics").help("Use optical response.").flag();

    Mustard::Env::BasicEnv env{argc, argv, cli};
    Detector::Description::UsePhaseIDefault();
    cli.DetectorDescriptionIOIfFlagged();

    const auto& ecal{MACE::Detector::Description::ECAL::Instance()};
    const auto& moduleList{ecal.Array().moduleList};

    using ECALEnergy = Mustard::Data::TupleModel<
        Mustard::Data::Value<double, "Edep", "Energy deposition of the cluster">,
        Mustard::Data::Value<int, "PE", "Photoelectron counts of the cluster">,
        // Mustard::Data::Value<double, "t", "Time of the track">,
        Mustard::Data::Value<muc::array3f, "Position", "Position of the cluster">,
        Mustard::Data::Value<double, "cosTheta", "Cosine of angle between the tracks">,
        Mustard::Data::Value<double, "theta", "Angle between the tracks">>;

    auto createEnergyTuple = [&](const std::vector<int>& potentialSeedModule,
                                 const std::unordered_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>>& hitDict,
                                 const CLHEP::Hep3Vector& truthHitMomentum) -> std::optional<Mustard::Data::Tuple<ECALEnergy>> {
        Mustard::Data::Tuple<ECALEnergy> energyTuple;

        if (potentialSeedModule.empty()) {
            return std::nullopt;
        }

        const auto energyThreshold{cli->get<double>("--energy-threshold")};
        const auto peCountThreshold{cli->get<int>("--pe-count-threshold")};
        const auto seedModule{potentialSeedModule.begin()};

        const auto cluster{ECALClustering::Reconstructing(*seedModule, moduleList, hitDict, energyThreshold, cli["--optics"] == true, peCountThreshold)};

        Get<"Edep">(energyTuple) = cluster.energy;
        Get<"PE">(energyTuple) = cluster.peCount;
        Get<"Position">(energyTuple) = cluster.position;
        Get<"cosTheta">(energyTuple) = cluster.position.cosTheta(truthHitMomentum);
        Get<"theta">(energyTuple) = cluster.position.theta(truthHitMomentum);

        return energyTuple;
    };

    ROOT::RDataFrame primaryData{cli->get("--input-primary-vertex-tree"), cli->get<std::vector<std::string>>("input")};
    ROOT::RDataFrame inputData{cli->get("--input-ecal-hit-tree"), cli->get<std::vector<std::string>>("input")};
    Mustard::ProcessSpecificFile<TFile> outputFile{cli->get("--output"), cli->get("--output-mode")};
    Mustard::Data::Output<ECALEnergy> reconEnergy{cli->get("--output-tree")};
    Mustard::Data::SeqProcessor processor;

    processor.Process<Data::SimPrimaryVertex, Data::ECALSimHit>(
        {primaryData, inputData}, int{}, "EvtID",
        [&](auto&& primary, auto&& event) {
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return Get<"Edep">(*hit1) > Get<"Edep">(*hit2);
                         });

            std::unordered_map<int, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>> hitDict;
            std::vector<int> potentialSeedModule;
            muc::array3f primaryMomentum{};
            CLHEP::Hep3Vector truthHitMomentum{};

            for (auto&& hit : event) {
                hitDict.try_emplace(Get<"ModID">(*hit), hit);
                potentialSeedModule.emplace_back(Get<"ModID">(*hit));
            }

            if (primary.size() != 1) {
                Mustard::PrintError(fmt::format("Unexpected number ({}) of primary vertices", primary.size()));
                return;
            }
            primaryMomentum = Get<"p0">(*primary.front());
            truthHitMomentum.set(primaryMomentum.at(0),
                                 primaryMomentum.at(1),
                                 primaryMomentum.at(2));

            auto energyTuple{createEnergyTuple(potentialSeedModule, hitDict, truthHitMomentum)};
            if (not energyTuple) {
                return;
            }

            reconEnergy.Fill(std::move(*energyTuple));
        });
    reconEnergy.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::PhaseI::CaliECAL
