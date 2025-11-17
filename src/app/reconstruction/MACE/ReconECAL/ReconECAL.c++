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
#include "MACE/ReconECAL/ReconECAL.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/Tuple.h++"
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

#include "muc/algorithm"

#include "fmt/format.h"

#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

namespace MACE::ReconECAL {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;

auto Smear(float e) -> float {
    e *= 1000;
    constexpr auto a = -7.47073293;
    constexpr auto b = 2.76377561;
    auto fwhm = a + b * std::sqrt(e);
    auto smearedEnergy = gRandom->Gaus(e, fwhm / 2.35482);
    return smearedEnergy / 1000;
}

ReconECAL::ReconECAL() :
    Subprogram{"ReconECAL", "Electromagnetic calorimeter (ECAL) event reconstruction."} {}

auto ReconECAL::Main(int argc, char* argv[]) const -> int {
    Mustard::Env::MPIEnv env{argc, argv, {}};

    std::vector<std::string> files;
    for (auto i{1}; i < argc; ++i) {
        files.emplace_back(argv[i]);
    }

    const auto& ecal{Detector::Description::ECAL::Instance()};
    const auto& faceList{ecal.Mesh().faceList};

    std::map<int, CLHEP::Hep3Vector> centroidMap;

    for (int i{}; auto&& [centroid, _1, _2, _3, _4] : std::as_const(faceList)) {
        centroidMap[i] = centroid;
        i++;
    }

    TFile outputFile{Mustard::Parallel::ProcessSpecificPath("dual_coin.root").generic_string().c_str(), "RECREATE"};
    using ECALEnergy = Mustard::Data::TupleModel<Mustard::Data::Value<float, "Edep", "Energy deposition">,
                                                 Mustard::Data::Value<float, "Edep1", "Energy deposition 1">,
                                                 Mustard::Data::Value<float, "Edep2", "Energy deposition 2">,
                                                 Mustard::Data::Value<float, "dE", "Delta energy">,
                                                 Mustard::Data::Value<double, "theta", "angel">,
                                                 Mustard::Data::Value<double, "dt0", "Delta time">>;
    Mustard::Data::Output<ECALEnergy> reconEnergy{"G4Run0/ReconECAL"};

    Mustard::Data::Processor processor;
    processor.Process<Data::ECALSimHit>(
        ROOT::RDataFrame{"G4Run0/ECALSimHit", files}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return Get<"Edep">(*hit1) > Get<"Edep">(*hit2);
                         });
            std::unordered_map<short, std::shared_ptr<Mustard::Data::Tuple<Data::ECALSimHit>>> hitDict;
            std::vector<short> potentialSeedModule;

            for (auto&& hit : event) {
                hitDict.try_emplace(Get<"ModID">(*hit), hit);
                if (Get<"Edep">(*hit) < 50_keV) {
                    continue;
                }
                potentialSeedModule.emplace_back(Get<"ModID">(*hit));
            }

            if (std::ssize(potentialSeedModule) < 2) {
                return;
            }

            std::unordered_set<short> firstCluster;
            std::unordered_set<short> secondCluster;

            auto firstSeedModule = potentialSeedModule.begin();
            auto secondSeedModule = std::ranges::next(potentialSeedModule.begin());

            const auto clustering = [&](std::unordered_set<short>& set, std::vector<short>::iterator it) {
                set.insert(*it); // add seed module
                for (auto&& m : faceList[*it].neighborModuleID) {
                    set.insert(m); // add 1st layer
                    for (auto&& n : faceList[m].neighborModuleID) {
                        set.insert(n);                                                                        // add 2nd layer
                        set.insert(faceList[n].neighborModuleID.begin(), faceList[n].neighborModuleID.end()); // add 3rd layer
                    }
                }

                float energy{};
                for (auto&& m : set) {
                    if (not hitDict.contains(m) or Get<"Edep">(*hitDict.at(m)) < 50_keV) {
                        continue;
                    }
                    energy += Smear(Get<"Edep">(*hitDict.at(m)));
                }
                return energy;
            };

            auto firstClusterEnergy = clustering(firstCluster, firstSeedModule);
            auto secondClusterEnergy = clustering(secondCluster, secondSeedModule);

            if (firstClusterEnergy > 590_keV or secondClusterEnergy > 590_keV) {
                return;
            }

            Mustard::Data::Tuple<ECALEnergy> energyTuple;
            Get<"Edep">(energyTuple) = firstClusterEnergy + secondClusterEnergy;
            Get<"Edep1">(energyTuple) = firstClusterEnergy;
            Get<"Edep2">(energyTuple) = secondClusterEnergy;
            Get<"dE">(energyTuple) = std::abs(firstClusterEnergy - secondClusterEnergy);
            Get<"theta">(energyTuple) = centroidMap.at(*firstSeedModule).angle(centroidMap.at(*secondSeedModule));
            Get<"dt0">(energyTuple) = std::abs(*Get<"t0">(*hitDict.at(*firstSeedModule)) - *Get<"t0">(*hitDict.at(*secondSeedModule)));
            reconEnergy.Fill(std::move(energyTuple));
        });

    reconEnergy.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::ReconECAL
