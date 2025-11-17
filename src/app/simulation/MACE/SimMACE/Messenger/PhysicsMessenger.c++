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

#include "MACE/SimMACE/Action/DetectorConstruction.h++"
#include "MACE/SimMACE/Messenger/PhysicsMessenger.h++"

#include "G4UIcmdWithABool.hh"

namespace MACE::SimMACE::inline Messenger {

PhysicsMessenger::PhysicsMessenger() :
    SingletonMessenger{},
    fApplyProductionCutNearTarget{} {
    fApplyProductionCutNearTarget = std::make_unique<G4UIcmdWithABool>("/MACE/Physics/ApplyProductionCutNearTarget", this);
    fApplyProductionCutNearTarget->SetGuidance("If yes, apply default production cut to the material-to-vacuum region near the target");
    fApplyProductionCutNearTarget->SetParameterName("apply", false);
    fApplyProductionCutNearTarget->AvailableForStates(G4State_Idle);
}

PhysicsMessenger::~PhysicsMessenger() = default;

auto PhysicsMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fApplyProductionCutNearTarget.get()) {
        Deliver<DetectorConstruction>([&](auto&& r) {
            r.ApplyProductionCutNearTarget(fApplyProductionCutNearTarget->GetNewBoolValue(value));
        });
    }
}

} // namespace MACE::SimMACE::inline Messenger
