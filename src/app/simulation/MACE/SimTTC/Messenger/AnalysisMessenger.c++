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

#include "MACE/SimTTC/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimTTC/Action/TrackingAction.h++"
#include "MACE/SimTTC/Analysis.h++"
#include "MACE/SimTTC/Messenger/AnalysisMessenger.h++"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

#include <string_view>

namespace MACE::SimTTC::inline Messenger {

AnalysisMessenger::AnalysisMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fSavePrimaryVertexData{},
    fSaveDecayVertexData{},
    fSaveTTCHitData{},
    fSaveTTCSiPMHitData{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MACE/Analysis/");
    fDirectory->SetGuidance("MACE::SimTTC::Analysis controller.");

    fSavePrimaryVertexData = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/SavePrimaryVertexData", this);
    fSavePrimaryVertexData->SetGuidance("Save primary vertex data if enabled.");
    fSavePrimaryVertexData->SetParameterName("mode", false);
    fSavePrimaryVertexData->AvailableForStates(G4State_Idle);

    fSaveDecayVertexData = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/SaveDecayVertexData", this);
    fSaveDecayVertexData->SetGuidance("Save decay vertex data if enabled.");
    fSaveDecayVertexData->SetParameterName("mode", false);
    fSaveDecayVertexData->AvailableForStates(G4State_Idle);

    fSaveTTCHitData = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/SaveTTCHitData", this);
    fSaveTTCHitData->SetGuidance("Save good TTC hit data if enabled.");
    fSaveTTCHitData->SetParameterName("mode", false);
    fSaveTTCHitData->AvailableForStates(G4State_Idle);

    fSaveTTCSiPMHitData = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/SaveTTCSiPMHitData", this);
    fSaveTTCSiPMHitData->SetGuidance("Save TTCSiPM hit data if enabled.");
    fSaveTTCSiPMHitData->SetParameterName("mode", false);
    fSaveTTCSiPMHitData->AvailableForStates(G4State_Idle);
}

AnalysisMessenger::~AnalysisMessenger() = default;

auto AnalysisMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fSavePrimaryVertexData.get()) {
        Deliver<PrimaryGeneratorAction>([&](auto&& r) {
            r.SavePrimaryVertexData(fSavePrimaryVertexData->GetNewBoolValue(value));
        });
    } else if (command == fSaveDecayVertexData.get()) {
        Deliver<TrackingAction>([&](auto&& r) {
            r.SaveDecayVertexData(fSaveDecayVertexData->GetNewBoolValue(value));
        });
    } else if (command == fSaveTTCHitData.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.SaveTTCHitData(fSaveTTCHitData->GetNewBoolValue(value));
        });
    } else if (command == fSaveTTCSiPMHitData.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.SaveTTCSiPMHitData(fSaveTTCSiPMHitData->GetNewBoolValue(value));
        });
    }
}

} // namespace MACE::SimTTC::inline Messenger
