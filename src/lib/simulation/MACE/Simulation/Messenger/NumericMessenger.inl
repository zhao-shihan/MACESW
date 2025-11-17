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
NumericMessenger<AReceiver>::NumericMessenger() :
    Mustard::Geant4X::SingletonMessenger<NumericMessenger<AReceiver>, AReceiver>{},
    fDirectory{},
    fMinDriverStep{},
    fDeltaChord{} {
    // NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)
    fDirectory = std::make_unique<G4UIdirectory>("/MACE/Numeric/");
    fDirectory->SetGuidance("MACE numeric controller.");

    fMinDriverStep = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Numeric/MinDriverStep", this);
    fMinDriverStep->SetGuidance("Set h_min for particle stepper driver in field.");
    fMinDriverStep->SetParameterName("hMin", false);
    fMinDriverStep->SetUnitCategory("Length");
    fMinDriverStep->AvailableForStates(G4State_PreInit);

    fDeltaChord = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Numeric/DeltaChord", this);
    fDeltaChord->SetGuidance("Set delta chord.");
    fDeltaChord->SetParameterName("delta", false);
    fDeltaChord->SetUnitCategory("Length");
    fDeltaChord->AvailableForStates(G4State_PreInit);
    // NOLINTEND(cppcoreguidelines-prefer-member-initializer)
}

template<typename AReceiver>
auto NumericMessenger<AReceiver>::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fMinDriverStep.get()) {
        this->template Deliver<AReceiver>([&](auto&& r) {
            r.MinDriverStep(fMinDriverStep->GetNewDoubleValue(value));
        });
    } else if (command == fDeltaChord.get()) {
        this->template Deliver<AReceiver>([&](auto&& r) {
            r.DeltaChord(fDeltaChord->GetNewDoubleValue(value));
        });
    }
}

} // namespace MACE::inline Simulation::inline Messenger
