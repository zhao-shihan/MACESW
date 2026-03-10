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

#include "MACE/PhaseI/Detector/Description/MRPC.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Analysis.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/MRPCSD.h++"

#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ParticleDefinition.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TwoVector.hh"
#include "G4VProcess.hh"

#include "muc/algorithm"

#include "gsl/gsl"

#include <cmath>
#include <ranges>
#include <string_view>
#include <tuple>

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

using namespace Mustard::LiteralUnit::Energy;

MRPCSD::MRPCSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fIonizingEnergyDepositionThreshold{20_eV},
    fSplitHit{},
    fHitsCollection{},
    fMessengerRegister{this} {
    collectionName.insert(sdName + "HC");
}

MRPCSD::~MRPCSD() = default;

auto MRPCSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHitsCollection = new MRPCHitCollection{SensitiveDetectorName, collectionName[0]};
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto MRPCSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto eDep{step.GetTotalEnergyDeposit()};

    Expects(0 <= step.GetNonIonizingEnergyDeposit());
    Expects(step.GetNonIonizingEnergyDeposit() <= eDep);
    if (eDep - step.GetNonIonizingEnergyDeposit() < fIonizingEnergyDepositionThreshold) {
        return false;
    }

    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};
    const auto& preStepPoint{*step.GetPreStepPoint()};
    const auto& touchable{*preStepPoint.GetTouchable()};
    const auto& modID{touchable.GetReplicaNumber(1)};

    // transform hit position to local coordinate
    const G4TwoVector hitPosition{*touchable.GetRotation() * (preStepPoint.GetPosition() - touchable.GetTranslation())};
    // calculate (E0, p0)
    const auto vertexEk{track.GetVertexKineticEnergy()};
    const auto vertexMomentum{track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};
    // track creator process
    const auto creatorProcess{track.GetCreatorProcess()};

    if (particle.GetPDGCharge() == 0) {
        return false;
    }

    // new a hit
    const auto& hit{fSplitHit[modID].emplace_back(std::make_unique_for_overwrite<MRPCHit>())};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"ModID">(*hit) = modID;
    Get<"t">(*hit) = preStepPoint.GetGlobalTime();
    // Get<"x">(*hit) = hitPosition;
    Get<"x">(*hit) = preStepPoint.GetPosition();
    Get<"Trig">(*hit) = false; // to be determined
    Get<"Edep">(*hit) = eDep;
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

auto MRPCSD::EndOfEvent(G4HCofThisEvent*) -> void {
    fHitsCollection->GetVector()->reserve(fSplitHit.size());

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
            const auto timeResolutionFWHM{PhaseI::Detector::Description::MRPC::Instance().TimeResolutionFWHM()};
            Expects(timeResolutionFWHM >= 0);
            // sort hit by time
            muc::timsort(splitHit,
                         [](const auto& hit1, const auto& hit2) {
                             return Get<"t">(*hit1) < Get<"t">(*hit2);
                         });
            // loop over all hits and cluster to real hits by times
            std::ranges::subrange cluster{splitHit.begin(), splitHit.begin()};
            while (cluster.end() != splitHit.end()) {
                const auto tFirst{*Get<"t">(**cluster.end())};
                const auto windowClosingTime{tFirst + timeResolutionFWHM};
                if (tFirst == windowClosingTime and // Notice: bad numeric with huge Get<"t">(**clusterFirst)!
                    timeResolutionFWHM != 0) [[unlikely]] {
                    Mustard::PrintWarning(fmt::format("A huge time ({}) completely rounds off the time resolution ({})", tFirst, timeResolutionFWHM));
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
        Get<"Trig">(*hit) = true;
    }

    Analysis::Instance().SubmitMRPCHC(*fHitsCollection->GetVector());
}

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
