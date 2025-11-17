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

#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Simulation/SD/ECALPMSD.h++"
#include "MACE/Simulation/SD/ECALSD.h++"

#include "Mustard/IO/PrettyLog.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"

#include "muc/algorithm"
#include "muc/numeric"

#include "gsl/gsl"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace MACE::inline Simulation::inline SD {

ECALSD::ECALSD(const G4String& sdName, const ECALPMSD* ecalPMSD) :
    G4VSensitiveDetector{sdName},
    fECALPMSD{ecalPMSD},
    fEnergyDepositionThreshold{},
    fSplitHit{},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");

    const auto& ecal{Detector::Description::ECAL::Instance()};
    Expects(ecal.ScintillationEnergyBin().size() == ecal.ScintillationComponent1().size());
    std::vector<double> dE(ecal.ScintillationEnergyBin().size());
    muc::ranges::adjacent_difference(ecal.ScintillationEnergyBin(), dE.begin());
    std::vector<double> spectrum(ecal.ScintillationComponent1().size());
    muc::ranges::adjacent_difference(ecal.ScintillationEnergyBin(), spectrum.begin(), muc::midpoint<double>);
    const auto integral{std::inner_product(next(spectrum.cbegin()), spectrum.cend(), next(dE.cbegin()), 0.)};
    std::vector<double> meanE(ecal.ScintillationEnergyBin().size());
    muc::ranges::adjacent_difference(ecal.ScintillationEnergyBin(), meanE.begin(), muc::midpoint<double>);
    std::ranges::transform(spectrum, meanE, spectrum.begin(), std::multiplies{});
    fEnergyDepositionThreshold = std::inner_product(next(spectrum.cbegin()), spectrum.cend(), next(dE.cbegin()), 0.) / integral;

    fSplitHit.reserve(ecal.NUnit());
}

auto ECALSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHitsCollection = new ECALHitCollection{SensitiveDetectorName, collectionName[0]};
    const auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto ECALSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};

    if (&particle == G4OpticalPhoton::Definition()) {
        return false;
    }

    const auto eDep{step.GetTotalEnergyDeposit()};
    if (eDep < fEnergyDepositionThreshold) {
        return false;
    }
    Expects(eDep > 0);

    const auto& preStepPoint{*step.GetPreStepPoint()};
    const auto& touchable{*preStepPoint.GetTouchable()};
    const auto modID{touchable.GetReplicaNumber()};
    // calculate (Ek0, p0)
    const auto vertexEk{track.GetVertexKineticEnergy()};
    const auto vertexMomentum{track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};
    // track creator process
    const auto creatorProcess{track.GetCreatorProcess()};
    // new a hit
    const auto& hit{fSplitHit[modID].emplace_back(std::make_unique_for_overwrite<ECALHit>())};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"ModID">(*hit) = modID;
    Get<"t">(*hit) = preStepPoint.GetGlobalTime();
    Get<"Edep">(*hit) = eDep;
    Get<"nOptPho">(*hit) = -1; // to be determined
    Get<"x">(*hit) = preStepPoint.GetPosition() - touchable.GetTranslation();
    Get<"Ek">(*hit) = preStepPoint.GetKineticEnergy();
    Get<"p">(*hit) = preStepPoint.GetMomentum();
    Get<"TrkID">(*hit) = track.GetTrackID();
    Get<"PDGID">(*hit) = particle.GetPDGEncoding();
    Get<"t0">(*hit) = track.GetGlobalTime() - track.GetLocalTime();
    Get<"x0">(*hit) = track.GetVertexPosition();
    Get<"Ek0">(*hit) = vertexEk;
    Get<"p0">(*hit) = vertexMomentum;
    *Get<"CreatProc">(*hit) = creatorProcess ? std::string_view{creatorProcess->GetProcessName()} : "|0>";

    return true;
}

auto ECALSD::EndOfEvent(G4HCofThisEvent*) -> void {
    fHitsCollection->GetVector()->reserve(
        muc::ranges::accumulate(fSplitHit, 0,
                                [](auto&& count, auto&& cellHit) {
                                    return count + cellHit.second.size();
                                }));

    for (auto&& [modID, splitHit] : fSplitHit) {
        switch (splitHit.size()) {
        case 0:
            muc::unreachable();
        case 1: {
            auto& hit{splitHit.front()};
            Ensures(Get<"ModID">(*hit) == modID);
            fHitsCollection->insert(hit.release());
        } break;
        default: {
            const auto scintillationTimeConstant1{Detector::Description::ECAL::Instance().ScintillationTimeConstant1()};
            Expects(scintillationTimeConstant1 >= 0);
            // sort hit by time
            muc::timsort(splitHit,
                         [](const auto& hit1, const auto& hit2) {
                             return Get<"t">(*hit1) < Get<"t">(*hit2);
                         });
            // loop over all hits on this crystal and cluster to real hits by times
            std::ranges::subrange cluster{splitHit.begin(), splitHit.begin()};
            while (cluster.end() != splitHit.end()) {
                const auto tFirst{*Get<"t">(**cluster.end())};
                const auto windowClosingTime{tFirst + scintillationTimeConstant1};
                if (tFirst == windowClosingTime and // Notice: bad numeric with huge Get<"t">(**clusterFirst)!
                    scintillationTimeConstant1 != 0) [[unlikely]] {
                    Mustard::PrintWarning(fmt::format("A huge time ({}) completely rounds off the time resolution ({})", tFirst, scintillationTimeConstant1));
                }
                cluster = {cluster.end(), std::ranges::find_if_not(cluster.end(), splitHit.end(),
                                                                   [&windowClosingTime](const auto& hit) {
                                                                       return Get<"t">(*hit) <= windowClosingTime;
                                                                   })};
                // find top hit
                auto& topHit{*std::ranges::min_element(cluster,
                                                       [](const auto& hit1, const auto& hit2) {
                                                           return Get<"TrkID">(*hit1) < Get<"TrkID">(*hit2);
                                                       })};
                // construct real hit
                Ensures(Get<"ModID">(*topHit) == modID);
                for (const auto& hit : cluster) {
                    if (hit == topHit) {
                        continue;
                    }
                    Get<"Edep">(*topHit) += Get<"Edep">(*hit);
                }
                fHitsCollection->insert(topHit.release());
            }
        } break;
        }
    }
    fSplitHit.clear();

    muc::timsort(*fHitsCollection->GetVector(),
                 [](const auto& hit1, const auto& hit2) {
                     return std::tie(Get<"TrkID">(*hit1), Get<"t">(*hit1)) <
                            std::tie(Get<"TrkID">(*hit2), Get<"t">(*hit2));
                 });

    for (int hitID{}; auto&& hit : *fHitsCollection->GetVector()) {
        Get<"HitID">(*hit) = hitID++;
    }

    if (fECALPMSD) {
        auto nHit{fECALPMSD->NOpticalPhotonHit()};
        for (auto&& hit : std::as_const(*fHitsCollection->GetVector())) {
            Get<"nOptPho">(*hit) = nHit[Get<"ModID">(*hit)];
        }
    }
}

} // namespace MACE::inline Simulation::inline SD
