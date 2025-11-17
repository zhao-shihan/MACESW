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

#include "MACE/Data/SimVertex.h++"
#include "MACE/SimMMS/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimMMS/Action/TrackingAction.h++"
#include "MACE/SimMMS/Analysis.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4ProcessType.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"

#include <cassert>
#include <vector>

namespace MACE::SimMMS::inline Action {

TrackingAction::TrackingAction() :
    PassiveSingleton{this},
    G4UserTrackingAction{},
    fSaveDecayVertexData{true},
    fDecayVertexData{},
    fMessengerRegister{this} {}

auto TrackingAction::PostUserTrackingAction(const G4Track* track) -> void {
    if (fSaveDecayVertexData) {
        UpdateDecayVertexData(*track);
    }
}

auto TrackingAction::UpdateDecayVertexData(const G4Track& track) -> void {
    if (auto& eventManager{*G4EventManager::GetEventManager()};
        eventManager.GetTrackingManager()
            ->GetSteppingManager()
            ->GetfCurrentProcess()
            ->GetProcessType() == fDecay) {
        std::vector<int> secondaryPDGID;
        secondaryPDGID.reserve(track.GetStep()->GetSecondary()->size());
        for (auto&& sec : *track.GetStep()->GetSecondary()) {
            secondaryPDGID.emplace_back(sec->GetParticleDefinition()->GetPDGEncoding());
        }
        auto& vertex{fDecayVertexData.emplace_back(std::make_unique_for_overwrite<Mustard::Data::Tuple<Data::SimDecayVertex>>())};
        Get<"EvtID">(*vertex) = eventManager.GetConstCurrentEvent()->GetEventID();
        Get<"TrkID">(*vertex) = track.GetTrackID();
        Get<"PDGID">(*vertex) = track.GetParticleDefinition()->GetPDGEncoding();
        Get<"SecPDGID">(*vertex) = std::move(secondaryPDGID);
        Get<"t">(*vertex) = track.GetGlobalTime();
        Get<"x">(*vertex) = track.GetPosition();
        Get<"Ek">(*vertex) = track.GetKineticEnergy();
        Get<"p">(*vertex) = track.GetMomentum();
    }
}

} // namespace MACE::SimMMS::inline Action
