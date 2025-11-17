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
#include "MACE/SimTTC/Messenger/PrimaryGeneratorActionMessenger.h++"

#include "G4UIcommand.hh"

namespace MACE::SimTTC::inline Messenger {

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger() :
    SingletonMessenger{},
    fSwitchToGPSX{},
    fSwitchToEcoMug{} {

    fSwitchToGPSX = std::make_unique<G4UIcommand>("/MACE/Generator/SwitchToGPSX", this);
    fSwitchToGPSX->SetGuidance("If set then the G4GeneralParticleSource will be used.");
    fSwitchToGPSX->AvailableForStates(G4State_Idle);

    fSwitchToEcoMug = std::make_unique<G4UIcommand>("/MACE/Generator/SwitchToEcoMug", this);
    fSwitchToEcoMug->SetGuidance("If set then the EcoMug generator will be used.");
    fSwitchToEcoMug->AvailableForStates(G4State_Idle);
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger() = default;

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String) {
    if (command == fSwitchToGPSX.get()) {
        Deliver<PrimaryGeneratorAction>([&](auto&& r) {
            r.SwitchToGPSX();
        });
    } else if (command == fSwitchToEcoMug.get()) {
        Deliver<PrimaryGeneratorAction>([&](auto&& r) {
            r.SwitchToEcoMug();
        });
    }
}

} // namespace MACE::SimTTC::inline Messenger
