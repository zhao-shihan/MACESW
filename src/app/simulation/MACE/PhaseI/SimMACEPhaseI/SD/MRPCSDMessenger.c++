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

#include "MACE/PhaseI/SimMACEPhaseI/SD/MRPCSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/MRPCSDMessenger.h++"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

MRPCSDMessenger::MRPCSDMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fIonizingEnergyDepositionThreshold{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MACE/SD/MRPC/");
    fDirectory->SetGuidance("MRPC sensitive detector.");

    fIonizingEnergyDepositionThreshold = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/SD/MRPC/IonizingEnergyDepositionThreshold", this);
    fIonizingEnergyDepositionThreshold->SetGuidance("Ionizing energy deposition threshold for a response.");
    fIonizingEnergyDepositionThreshold->SetParameterName("E", false);
    fIonizingEnergyDepositionThreshold->SetUnitCategory("Energy");
    fIonizingEnergyDepositionThreshold->SetRange("E >= 0");
    fIonizingEnergyDepositionThreshold->AvailableForStates(G4State_Idle);
}

MRPCSDMessenger::~MRPCSDMessenger() = default;

auto MRPCSDMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fIonizingEnergyDepositionThreshold.get()) {
        Deliver<MRPCSD>([&](auto&& r) {
            r.IonizingEnergyDepositionThreshold(fIonizingEnergyDepositionThreshold->GetNewDoubleValue(value));
        });
    }
}

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
