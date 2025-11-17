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

#include "MACE/PhaseI/Simulation/SD/SciFiSiPMSD.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"

namespace MACE::PhaseI::inline Simulation::inline SD {

SciFiSiPMSD::SciFiSiPMSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fHit{},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");
}

auto SciFiSiPMSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHit.clear(); // clear at the begin of event allows TTCSD to get optical photon counts at the end of event

    fHitsCollection = new SciFiSiPMHitCollection(SensitiveDetectorName, collectionName[0]);
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto SciFiSiPMSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};

    if (&particle != G4OpticalPhoton::Definition()) {
        return false;
    }

    step.GetTrack()->SetTrackStatus(fStopAndKill);

    const auto postStepPoint{*step.GetPostStepPoint()};
    const auto siPMID{postStepPoint.GetTouchable()->GetReplicaNumber()};
    // new a hit
    auto hit{std::make_unique_for_overwrite<SciFiSiPMHit>()};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"SiPMID">(*hit) = siPMID;
    Get<"t">(*hit) = postStepPoint.GetGlobalTime();
    fHit[siPMID].emplace_back(std::move(hit));
    return true;
}

auto SciFiSiPMSD::EndOfEvent(G4HCofThisEvent*) -> void {
    for (int hitID{};
         auto&& [siPMID, hitOfDetector] : fHit) {
        for (auto&& hit : hitOfDetector) {
            Get<"HitID">(*hit) = hitID++;
            assert(Get<"SiPMID">(*hit) == siPMID);
            fHitsCollection->insert(hit.release());
        }
    }
}

auto SciFiSiPMSD::NOpticalPhotonHit() const -> muc::flat_hash_map<int, int> {
    muc::flat_hash_map<int, int> nHit;
    for (auto&& [siPMID, hit] : fHit) {
        if (not hit.empty()) {
            nHit[siPMID] = hit.size();
        }
    }
    return nHit;
}

} // namespace MACE::PhaseI::inline Simulation::inline SD
