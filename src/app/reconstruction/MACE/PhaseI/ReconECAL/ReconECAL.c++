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
#include "MACE/Reconstruction/ECALClustering/Clusterer.h++"

#include "Mustard/CLI/BasicCLI.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Detector/Description/DescriptionIO.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "CLHEP/Vector/ThreeVector.h"

#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TH1.h"
#include "TH3.h"
#include "TRandom.h"
#include "TTree.h"
#include "TVector3.h"

#include "muc/algorithm"

#include "fmt/format.h"

#include <algorithm>
#include <functional>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

namespace MACE::PhaseI::ReconECAL {

ReconECAL::ReconECAL() :
    Subprogram{"ReconECAL", "Electromagnetic calorimeter (ECAL) event reconstruction in PhaseI."} {}

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::literals;

auto ReconECAL::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::BasicCLI<> cli;
    cli->add_argument("input").help("Input file path(s).").nargs(argparse::nargs_pattern::at_least_one);
    cli->add_argument("-t", "--input-tree").help("Input tree name.").default_value("G4Run0/ECALSimHit"s).required().nargs(1);
    cli->add_argument("-o", "--output").help("Output file path.").required().nargs(1);
    cli->add_argument("-m", "--output-mode").help("Output file creation mode.").default_value("RECREATE"s).required().nargs(1);
    cli->add_argument("-d", "--description").help("Description YAML file path.").nargs(1);
    cli->add_argument("-single", "--track-single").help("Reconstruction of single event.").flag();
    cli->add_argument("-double", "--track-double").help("Reconstruction of double events.").flag();
    cli->add_argument("-triple", "--track-triple").help("Reconstruction of triple events.").flag();
    cli->add_argument("-optics", "--optics").help("Use optical response.").flag();
    cli->add_argument("-cali", "--calibration").help("Use calibration configurations.").flag();
    Mustard::Env::MPIEnv env{argc, argv, cli};

    const auto doSingle{cli["--track-single"] == true};
    const auto doDouble{cli["--track-double"] == true};
    const auto doTriple{cli["--track-triple"] == true};
    const auto useOptics{cli["--optics"] == true};
    const auto useCalibration{cli["--calibration"] == true};
    std::vector<bool> reconstructionConfig{doSingle, doDouble, doTriple};

    if (std::ranges::count(reconstructionConfig, true) != 1) {
        Mustard::PrintError("One and only one reconstruction mode must be enabled.");
    }
    if (const auto descriptionPath{cli->present("--description")}) {
        Mustard::Detector::Description::DescriptionIO::Import<MACE::Detector::Description::ECAL>(*descriptionPath);
    } else {
        Detector::Description::UsePhaseIDefault();
    }

    const auto& ecal{MACE::Detector::Description::ECAL::Instance()};
    const auto& moduleList{ecal.Array().moduleList};

    TFile outputFile{Mustard::Parallel::ProcessSpecificPath(cli->get("--output").c_str()).generic_string().c_str(), cli->get("--output-mode").c_str()};

    using ECALEnergy = Mustard::Data::TupleModel<
        Mustard::Data::Value<double, "Edep1", "Energy deposition of the 1st cluster">,
        Mustard::Data::Value<double, "Edep2", "Energy deposition of the 2nd cluster">,
        Mustard::Data::Value<double, "Edep3", "Energy deposition of the 3rd cluster">,
        Mustard::Data::Value<double, "TotalEdep", "Energy deposition in total">,
        Mustard::Data::Value<int, "PE1", "Photoelectron counts of the 1st cluster">,
        Mustard::Data::Value<int, "PE2", "Photoelectron counts of the 2nd cluster">,
        Mustard::Data::Value<int, "PE3", "Photoelectron counts of the 3rd cluster">,
        Mustard::Data::Value<int, "TotalPE", "Photoelectron counts in total">,
        // Mustard::Data::Value<double, "t", "Time of the track">,
        Mustard::Data::Value<double, "dE", "Energy difference of the tracks">,
        Mustard::Data::Value<double, "dt", "Time difference of the tracks">,
        Mustard::Data::Value<muc::array3f, "Position1", "Position of the 1st cluster">,
        Mustard::Data::Value<muc::array3f, "Position2", "Position of the 2nd cluster">,
        Mustard::Data::Value<muc::array3f, "Position3", "Position of the 3rd cluster">,
        Mustard::Data::Value<double, "cosTheta", "Cosine of angle between the tracks">,
        Mustard::Data::Value<double, "theta", "Angle between the tracks">>;

    Mustard::Data::Output<ECALEnergy> reconEnergy{"G4Run0/ReconECAL"};
    Mustard::Data::Processor processor;
    processor.Process<Data::ECALSimHit>(
        ROOT::RDataFrame{cli->get("--input-tree"), cli->get<std::vector<std::string>>("input")}, int{}, "EvtID",
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
            muc::array3f truthHitMomentum{};

            for (auto&& hit : event) {
                hitDict.try_emplace(Get<"ModID">(*hit), hit);
                potentialSeedModule.emplace_back(Get<"ModID">(*hit));
                if (Get<"TrkID">(*hit) == 1 and Get<"HitID">(*hit) == 0) {
                    truthHitMomentum = Get<"p0">(*hit);
                }
            }
            CLHEP::Hep3Vector truthHitVector{
                truthHitMomentum.at(0),
                truthHitMomentum.at(1),
                truthHitMomentum.at(2)};

            double energy1{};
            double energy2{};
            // double energy3{};
            int pe1{};
            // int pe2{};
            // int pe3{};
            CLHEP::Hep3Vector weightedPosition1{};
            CLHEP::Hep3Vector weightedPosition2{};
            // CLHEP::Hep3Vector weightedPosition3{};
            CLHEP::Hep3Vector clusterPosition1{};
            CLHEP::Hep3Vector clusterPosition2{};
            // CLHEP::Hep3Vector clusterPosition3{};
            Mustard::Data::Tuple<ECALEnergy> energyTuple;

            if (doSingle) {
                auto seedModule = potentialSeedModule.begin();
                auto cluster = ECALClustering::Clusterer(*seedModule, moduleList);
                for (const auto& module : cluster) {
                    auto hitIt = hitDict.find(module);
                    if (hitIt == hitDict.end() or Get<"Edep">(*hitIt->second) < 50_keV) {
                        continue;
                    }
                    auto energy = Get<"Edep">(*hitIt->second);
                    weightedPosition1 += energy * moduleList.at(module).centroid;

                    if (useOptics) {
                        auto pe = Get<"nOptPho">(*hitIt->second);
                        if (pe > 3) {
                            energy1 += energy;
                            pe1 += pe;
                        }
                    } else {
                        energy1 += energy;
                    }
                    if (energy1 != 0) {
                        clusterPosition1 = weightedPosition1 / energy1;
                    }

                    Get<"Edep1">(energyTuple) = energy1;
                    Get<"PE1">(energyTuple) = pe1;
                    Get<"Position1">(energyTuple) = clusterPosition1;
                    if (useCalibration) {
                        Get<"cosTheta">(energyTuple) = clusterPosition1.cosTheta(truthHitVector);
                    } else {
                        Get<"theta">(energyTuple) = clusterPosition1.theta(CLHEP::Hep3Vector{0, 0, 1});
                    }
                }
            } else if (doDouble) {
                if (std::ssize(potentialSeedModule) < 2) {
                    return;
                }
                auto firstSeedModule = potentialSeedModule.begin();
                auto secondSeedModule = std::ranges::find_if(
                    potentialSeedModule,
                    [&](int moduleID) {
                        const auto& c1{moduleList.at(*firstSeedModule).centroid};
                        const auto& c2{moduleList.at(moduleID).centroid};
                        return c1.angle(c2) > 0.8 * pi;
                    });
                if (secondSeedModule == potentialSeedModule.end()) {
                    return;
                }
                auto firstCluster = ECALClustering::Clusterer(*firstSeedModule, moduleList);
                auto secondCluster = ECALClustering::Clusterer(*secondSeedModule, moduleList);
                for (const auto& module : firstCluster) {
                    auto hitIt = hitDict.find(module);
                    if (hitIt == hitDict.end() or Get<"Edep">(*hitIt->second) < 50_keV) {
                        continue;
                    }
                    auto energy = Get<"Edep">(*hitIt->second);
                    weightedPosition1 += energy * moduleList.at(module).centroid;
                    energy1 += energy;
                    if (energy1 != 0) {
                        clusterPosition1 = weightedPosition1 / energy1;
                    }
                }
                for (const auto& module : secondCluster) {
                    auto hitIt = hitDict.find(module);
                    if (hitIt == hitDict.end() or Get<"Edep">(*hitIt->second) < 50_keV) {
                        continue;
                    }
                    auto energy = Get<"Edep">(*hitIt->second);
                    weightedPosition2 += energy * moduleList.at(module).centroid;
                    energy2 += energy;
                    if (energy2 != 0) {
                        clusterPosition2 = weightedPosition2 / energy2;
                    }
                }

                Get<"Edep1">(energyTuple) = energy1;
                Get<"Position1">(energyTuple) = clusterPosition1;
                Get<"Edep2">(energyTuple) = energy2;
                Get<"Position2">(energyTuple) = clusterPosition2;
                Get<"TotalEdep">(energyTuple) = energy1 + energy2;
                Get<"dE">(energyTuple) = std::abs(energy1 - energy2);
                Get<"dt">(energyTuple) = std::abs(*Get<"t">(*hitDict.at(*firstSeedModule)) - *Get<"t">(*hitDict.at(*secondSeedModule)));
                Get<"cosTheta">(energyTuple) = clusterPosition1.cosTheta(clusterPosition2);
            } else if (doTriple) {
                // To be implemented
            }

            reconEnergy.Fill(std::move(energyTuple));
        });
    reconEnergy.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::PhaseI::ReconECAL
