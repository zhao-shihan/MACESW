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
#include "MACE/PhaseI/SimMACEPhaseI/Messenger/AnalysisMessenger.h++"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

#include <string_view>

namespace MACE::PhaseI::SimMACEPhaseI::inline Messenger {

AnalysisMessenger::AnalysisMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fFilePath{},
    fFileMode{},
    fSavePrimaryVertexData{},
    fSaveDecayVertexData{},
    fCoincidenceWithMRPC{},
    fCoincidenceWithECAL{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MACE/Analysis/");
    fDirectory->SetGuidance("MACE::SimMACEPhaseI::Analysis controller.");

    fFilePath = std::make_unique<G4UIcmdWithAString>("/MACE/Analysis/FilePath", this);
    fFilePath->SetGuidance("Set file path.");
    fFilePath->SetParameterName("path", false);
    fFilePath->AvailableForStates(G4State_Idle);

    fFileMode = std::make_unique<G4UIcmdWithAString>("/MACE/Analysis/FileMode", this);
    fFileMode->SetGuidance("Set mode (NEW, RECREATE, or UPDATE) for opening ROOT file(s).");
    fFileMode->SetParameterName("mode", false);
    fFileMode->AvailableForStates(G4State_Idle);

    fSavePrimaryVertexData = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/SavePrimaryVertexData", this);
    fSavePrimaryVertexData->SetGuidance("Save primary vertex data if enabled.");
    fSavePrimaryVertexData->SetParameterName("mode", false);
    fSavePrimaryVertexData->AvailableForStates(G4State_Idle);

    fSaveDecayVertexData = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/SaveDecayVertexData", this);
    fSaveDecayVertexData->SetGuidance("Save decay vertex data if enabled.");
    fSaveDecayVertexData->SetParameterName("mode", false);
    fSaveDecayVertexData->AvailableForStates(G4State_Idle);

    fCoincidenceWithMRPC = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/CoincidenceWithMRPC", this);
    fCoincidenceWithMRPC->SetGuidance("Enable MRPC for coincident detection.");
    fCoincidenceWithMRPC->SetParameterName("mode", false);
    fCoincidenceWithMRPC->AvailableForStates(G4State_Idle);

    fCoincidenceWithECAL = std::make_unique<G4UIcmdWithABool>("/MACE/Analysis/CoincidenceWithECAL", this);
    fCoincidenceWithECAL->SetGuidance("Enable ECAL for coincident detection.");
    fCoincidenceWithECAL->SetParameterName("mode", false);
    fCoincidenceWithECAL->AvailableForStates(G4State_Idle);
}

AnalysisMessenger::~AnalysisMessenger() = default;

auto AnalysisMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fFilePath.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.FilePath(std::string_view(value));
        });
    } else if (command == fFileMode.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.FileMode(value);
        });
    } else if (command == fSavePrimaryVertexData.get()) {
        Deliver<PrimaryGeneratorAction>([&](auto&& r) {
            r.SavePrimaryVertexData(fSavePrimaryVertexData->GetNewBoolValue(value));
        });
    } else if (command == fSaveDecayVertexData.get()) {
        Deliver<TrackingAction>([&](auto&& r) {
            r.SaveDecayVertexData(fSaveDecayVertexData->GetNewBoolValue(value));
        });
    } else if (command == fCoincidenceWithMRPC.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.CoincidenceWithMRPC(fCoincidenceWithMRPC->GetNewBoolValue(value));
        });
    } else if (command == fCoincidenceWithECAL.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.CoincidenceWithECAL(fCoincidenceWithECAL->GetNewBoolValue(value));
        });
    }
}

} // namespace MACE::PhaseI::SimMACEPhaseI::inline Messenger
