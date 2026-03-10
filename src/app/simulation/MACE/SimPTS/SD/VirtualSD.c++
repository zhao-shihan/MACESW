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

#include "MACE/SimPTS/Analysis.h++"
#include "MACE/SimPTS/Hit/VirtualHit.h++"
#include "MACE/SimPTS/SD/VirtualSD.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VTouchable.hh"

#include <string_view>

namespace MACE::SimPTS::inline SD {

VirtualSD::VirtualSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");
}

auto VirtualSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHitsCollection = new VirtualHitCollection{SensitiveDetectorName, collectionName[0]};
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto VirtualSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto& track{*step.GetTrack()};
    if (not(step.IsFirstStepInVolume() and track.GetCurrentStepNumber() > 1)) {
        return false;
    }

    const auto& particle{*track.GetDefinition()};
    const auto& preStepPoint{*step.GetPreStepPoint()};
    const auto& touchable{*preStepPoint.GetTouchable()};
    // transform hit position to local coordinate
    const G4TwoVector hitPosition{*touchable.GetRotation() * (preStepPoint.GetPosition() - touchable.GetTranslation())};
    // calculate (E0, p0)
    const auto vertexEk{track.GetVertexKineticEnergy()};
    const auto vertexMomentum{track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};
    // track creator process
    const auto creatorProcess{track.GetCreatorProcess()};
    // new a hit
    auto hit{new VirtualHit};
    Get<"DetID">(*hit) = touchable.GetCopyNumber();
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"TrkID">(*hit) = track.GetTrackID();
    Get<"PDGID">(*hit) = particle.GetPDGEncoding();
    Get<"t">(*hit) = preStepPoint.GetGlobalTime();
    Get<"t0">(*hit) = track.GetGlobalTime() - track.GetLocalTime();
    Get<"x">(*hit) = hitPosition;
    Get<"x0">(*hit) = track.GetVertexPosition();
    Get<"Ek">(*hit) = preStepPoint.GetKineticEnergy();
    Get<"Ek0">(*hit) = vertexEk;
    Get<"p">(*hit) = preStepPoint.GetMomentum();
    Get<"p0">(*hit) = vertexMomentum;
    *Get<"CreatProc">(*hit) = creatorProcess ? std::string_view{creatorProcess->GetProcessName()} : "|0>";
    fHitsCollection->insert(hit);

    return true;
}

auto VirtualSD::EndOfEvent(G4HCofThisEvent*) -> void {
    Analysis::Instance().SubmitVirtualHC(*fHitsCollection->GetVector());
}

} // namespace MACE::SimPTS::inline SD
