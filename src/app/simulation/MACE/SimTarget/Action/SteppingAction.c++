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

#include "MACE/SimTarget/Action/SteppingAction.h++"

#include "Mustard/Geant4X/Particle/Antimuonium.h++"
#include "Mustard/Geant4X/Particle/Muonium.h++"

#include "G4MuonPlus.hh"
#include "G4Step.hh"

#include "gsl/gsl"

namespace MACE::SimTarget::inline Action {

SteppingAction::SteppingAction() :
    PassiveSingleton{this},
    G4UserSteppingAction{},
    fMuonPlus{gsl::not_null{G4MuonPlus::Definition()}},
    fMuonium{gsl::not_null{Mustard::Geant4X::Muonium::Definition()}},
    fAntimuonium{gsl::not_null{Mustard::Geant4X::Antimuonium::Definition()}},
    fKillIrrelevance{false},
    fMessengerRegister{this} {}

auto SteppingAction::UserSteppingAction(const G4Step* step) -> void {
    if (fKillIrrelevance) {
        const auto particle = step->GetTrack()->GetParticleDefinition();
        if (particle != fMuonPlus and particle != fMuonium and particle != fAntimuonium) {
            step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
        }
    }
}

} // namespace MACE::SimTarget::inline Action
