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

#include "MACE/Detector/Description/TTC.h++"
#include "MACE/PhaseI/Detector/Description/TTC.h++"
#include "MACE/Simulation/SD/TTCSD.h++"
#include "MACE/Simulation/SD/TTCSiPMSD.h++"

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
#include "G4VTouchable.hh"

#include "muc/algorithm"
#include "muc/numeric"
#include "muc/utility"

#include "gsl/gsl"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iterator>
#include <numeric>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace MACE::inline Simulation::inline SD {

TTCSD::TTCSD(const G4String& sdName, const Type type, const TTCSiPMSD* ttcSiPMSD) :
    G4VSensitiveDetector{sdName},
    fTTCSiPMSD{ttcSiPMSD},
    fType{type},
    fEnergyDepositionThreshold{},
    fSplitHit{},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");

    const auto& energyThreshold{
        [](auto& ttc) {
            Expects(ttc.ScintillationComponent1EnergyBin().size() == ttc.ScintillationComponent1().size());
            std::vector<double> dE(ttc.ScintillationComponent1EnergyBin().size());
            muc::ranges::adjacent_difference(ttc.ScintillationComponent1EnergyBin(), dE.begin());
            std::vector<double> spectrum(ttc.ScintillationComponent1().size());
            muc::ranges::adjacent_difference(ttc.ScintillationComponent1EnergyBin(), spectrum.begin(), muc::midpoint<double>);
            const auto integral{std::inner_product(next(spectrum.cbegin()), spectrum.cend(), next(dE.cbegin()), 0.)};
            std::vector<double> meanE(ttc.ScintillationComponent1EnergyBin().size());
            muc::ranges::adjacent_difference(ttc.ScintillationComponent1EnergyBin(), meanE.begin(), muc::midpoint<double>);
            std::ranges::transform(spectrum, meanE, spectrum.begin(), std::multiplies{});
            return std::inner_product(next(spectrum.cbegin()), spectrum.cend(), next(dE.cbegin()), 0.) / integral;
        }};

    if (type == TTCSD::Type::MACE) {
        const auto& ttc{Detector::Description::TTC::Instance()};
        fEnergyDepositionThreshold = energyThreshold(ttc);
        fSplitHit.reserve(ttc.NAlongPhi() * ttc.Width().size());
    } else {
        const auto& ttc{PhaseI::Detector::Description::TTC::Instance()};
        fEnergyDepositionThreshold = energyThreshold(ttc);
        fSplitHit.reserve(muc::ranges::reduce(ttc.NAlongPhi()));
    }
}

auto TTCSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHitsCollection = new TTCHitCollection{SensitiveDetectorName, collectionName[0]};
    const auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto TTCSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
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
    const auto tileID{preStepPoint.GetTouchable()->GetReplicaNumber(1)};
    // calculate (Ek0, p0)
    const auto vertexEk{track.GetVertexKineticEnergy()};
    const auto vertexMomentum{track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};
    // track creator process
    const auto creatorProcess{track.GetCreatorProcess()};
    // new a hit
    const auto& hit{fSplitHit[tileID].emplace_back(std::make_unique_for_overwrite<TTCHit>())};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"TileID">(*hit) = tileID;
    Get<"t">(*hit) = preStepPoint.GetGlobalTime();
    Get<"Edep">(*hit) = eDep;
    Get<"Good">(*hit) = false; // to be determined
    Get<"ADC">(*hit) = {};     // to be determined
    Get<"nOptPho">(*hit) = {}; // to be determined
    Get<"x">(*hit) = preStepPoint.GetPosition();
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

auto TTCSD::EndOfEvent(G4HCofThisEvent*) -> void {
    fHitsCollection->GetVector()->reserve(
        muc::ranges::accumulate(fSplitHit, 0,
                                [](auto&& count, auto&& cellHit) {
                                    return count + cellHit.second.size();
                                }));

    for (auto&& [tileID, splitHit] : fSplitHit) {
        switch (splitHit.size()) {
        case 0:
            muc::unreachable();
        case 1: {
            auto& hit{splitHit.front()};
            Expects(Get<"TileID">(*hit) == tileID);
            fHitsCollection->insert(hit.release());
        } break;
        default: {
            const auto scintillationRiseTimeConstant1{Detector::Description::TTC::Instance().ScintillationRiseTimeConstant1()};
            const auto scintillationDecayTimeConstant1{Detector::Description::TTC::Instance().ScintillationDecayTimeConstant1()};
            Expects(scintillationRiseTimeConstant1 >= 0 and scintillationDecayTimeConstant1 >= 0);
            const auto triggerTimeWindow{scintillationRiseTimeConstant1 + scintillationDecayTimeConstant1};
            // sort hit by time
            muc::timsort(splitHit,
                         [](const auto& hit1, const auto& hit2) {
                             return Get<"t">(*hit1) < Get<"t">(*hit2);
                         });
            // loop over all hits on this tile and cluster to real hits by times
            std::ranges::subrange cluster{splitHit.begin(), splitHit.begin()};
            while (cluster.end() != splitHit.end()) {
                const auto tFirst{*Get<"t">(**cluster.end())};
                const auto windowClosingTime{tFirst + triggerTimeWindow};
                if (tFirst == windowClosingTime and // Notice: bad numeric with huge Get<"t">(**clusterFirst)!
                    triggerTimeWindow != 0) [[unlikely]] {
                    Mustard::PrintWarning(fmt::format("A huge time ({}) completely rounds off the time resolution ({})", tFirst, triggerTimeWindow));
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
                Ensures(Get<"TileID">(*topHit) == tileID);
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

    if (fTTCSiPMSD) {
        auto nHit{fTTCSiPMSD->NOpticalPhotonHit()};
        for (auto&& hit : std::as_const(*fHitsCollection->GetVector())) {
            Get<"nOptPho">(*hit) = nHit[Get<"TileID">(*hit)];
            Get<"ADC">(*hit) = {nHit[Get<"TileID">(*hit)].begin(), nHit[Get<"TileID">(*hit)].end()};
        }
    }
}

} // namespace MACE::inline Simulation::inline SD
