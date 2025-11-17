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
#include "MACE/SimTarget/Messenger/ActionMessenger.h++"

#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"

namespace MACE::SimTarget::inline Messenger {

ActionMessenger::ActionMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fKillIrrelevance{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MACE/Action/");

    fKillIrrelevance = std::make_unique<G4UIcmdWithABool>("/MACE/Action/KillIrrelevance", this);
    fKillIrrelevance->SetGuidance("Kill decay products of muon and muonium for better performance.");
    fKillIrrelevance->SetParameterName("b", false);
    fKillIrrelevance->AvailableForStates(G4State_Idle);
}

ActionMessenger::~ActionMessenger() = default;

auto ActionMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fKillIrrelevance.get()) {
        Deliver<SteppingAction>([&](auto&& r) {
            r.KillIrrelevance(fKillIrrelevance->GetNewBoolValue(value));
        });
    }
}

} // namespace MACE::SimTarget::inline Messenger
