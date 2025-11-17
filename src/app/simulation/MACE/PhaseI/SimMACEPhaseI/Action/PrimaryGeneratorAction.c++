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

#include "MACE/PhaseI/SimMACEPhaseI/Action/PrimaryGeneratorAction.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Action/TrackingAction.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Analysis.h++"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

namespace MACE::PhaseI::SimMACEPhaseI::inline Action {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
    PassiveSingleton{this},
    G4VUserPrimaryGeneratorAction{},
    fAvailableGenerator{{}, {Mustard::Geant4X::EcoMugCosmicRayMuon::Coordinate::Beam}},
    fGenerator{&fAvailableGenerator.gpsx},
    fSavePrimaryVertexData{true},
    fPrimaryVertexData{},
    fAnalysisMessengerRegister{this},
    fPrimaryGeneratorActionMessengerRegister{this} {}

auto PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) -> void {
    fGenerator->GeneratePrimaryVertex(event);
    if (fSavePrimaryVertexData) {
        UpdatePrimaryVertexData(*event);
    }
}

auto PrimaryGeneratorAction::UpdatePrimaryVertexData(const G4Event& event) -> void {
    fPrimaryVertexData.clear();
    fPrimaryVertexData.reserve(event.GetNumberOfPrimaryVertex());
    for (const auto* pv{event.GetPrimaryVertex()}; pv; pv = pv->GetNext()) {
        for (const auto* pp{pv->GetPrimary()}; pp; pp = pp->GetNext()) {
            const auto& v{fPrimaryVertexData.emplace_back(std::make_unique_for_overwrite<Mustard::Data::Tuple<MACE::Data::SimPrimaryVertex>>())};
            Get<"EvtID">(*v) = event.GetEventID();
            Get<"PDGID">(*v) = pp->GetPDGcode();
            Get<"t0">(*v) = pv->GetT0();
            Get<"x0">(*v) = pv->GetPosition();
            Get<"Ek0">(*v) = pp->GetKineticEnergy();
            Get<"p0">(*v) = pp->GetMomentum();
        }
    }
    Analysis::Instance().SubmitPrimaryVertexData(fPrimaryVertexData);
}

} // namespace MACE::PhaseI::SimMACEPhaseI::inline Action
