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

#include "MACE/Simulation/Physics/StandardPhysicsList.h++"
#include "MACE/Simulation/Physics/StandardPhysicsListMessenger.h++"

#include "G4UIcmdWithoutParameter.hh"
#include "G4UnitsTable.hh"

namespace MACE::inline Simulation::inline Physics {

StandardPhysicsListMessenger::StandardPhysicsListMessenger() :
    SingletonMessenger{},
    fUseRadioactiveDecayPhysics{},
    fUseOpticalPhysics{} {

    fUseRadioactiveDecayPhysics = std::make_unique<G4UIcmdWithoutParameter>("/MACE/Physics/UseRadioactiveDecayPhysics", this);
    fUseRadioactiveDecayPhysics->SetGuidance("If set, the G4RadioactiveDecayPhysics will be registered in the physics list.");
    fUseRadioactiveDecayPhysics->AvailableForStates(G4State_PreInit);

    fUseOpticalPhysics = std::make_unique<G4UIcmdWithoutParameter>("/MACE/Physics/UseOpticalPhysics", this);
    fUseOpticalPhysics->SetGuidance("If set, the G4OpticalPhysics will be registered in the physics list.");
    fUseOpticalPhysics->AvailableForStates(G4State_PreInit);
}

StandardPhysicsListMessenger::~StandardPhysicsListMessenger() = default;

auto StandardPhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String) -> void {
    if (command == fUseRadioactiveDecayPhysics.get()) {
        Deliver<StandardPhysicsListBase>([&](auto&& r) {
            r.UseRadioactiveDecayPhysics();
        });
    } else if (command == fUseOpticalPhysics.get()) {
        Deliver<StandardPhysicsListBase>([&](auto&& r) {
            r.UseOpticalPhysics();
        });
    }
}

} // namespace MACE::inline Simulation::inline Physics
