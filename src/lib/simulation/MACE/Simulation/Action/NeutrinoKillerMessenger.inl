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

namespace MACE::inline Simulation::inline Messenger {

template<typename AReceiver>
NeutrinoKillerMessenger<AReceiver>::NeutrinoKillerMessenger() :
    Mustard::Geant4X::SingletonMessenger<NeutrinoKillerMessenger<AReceiver>, AReceiver>{},
    fEnableNeutrinoKiller{} {
    // NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)
    fEnableNeutrinoKiller = std::make_unique<G4UIcmdWithABool>("/MACE/Action/EnableNeutrinoKiller", this);
    fEnableNeutrinoKiller->SetGuidance("Kill neutrino in stepping action if enabled (enable by default).");
    fEnableNeutrinoKiller->SetParameterName("bool", false);
    fEnableNeutrinoKiller->AvailableForStates(G4State_Idle);
    // NOLINTEND(cppcoreguidelines-prefer-member-initializer)
}

template<typename AReceiver>
auto NeutrinoKillerMessenger<AReceiver>::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fEnableNeutrinoKiller.get()) {
        this->template Deliver<AReceiver>([&](auto&& r) {
            r.EnableNeutrinoKiller(fEnableNeutrinoKiller->GetNewBoolValue(value));
        });
    }
}

} // namespace MACE::inline Simulation::inline Messenger
