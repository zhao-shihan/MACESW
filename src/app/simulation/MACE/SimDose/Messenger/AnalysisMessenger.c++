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

#include "MACE/SimDose/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimDose/Analysis.h++"
#include "MACE/SimDose/Messenger/AnalysisMessenger.h++"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"

#include <string_view>

namespace MACE::SimDose::inline Messenger {

AnalysisMessenger::AnalysisMessenger() :
    SingletonMessenger{},
    fDirectory{},
    fAddMap{},
    fMapNBinX{},
    fMapXMin{},
    fMapXMax{},
    fMapNBinY{},
    fMapYMin{},
    fMapYMax{},
    fMapNBinZ{},
    fMapZMin{},
    fMapZMax{} {

    fDirectory = std::make_unique<G4UIdirectory>("/MACE/Analysis/");
    fDirectory->SetGuidance("MACE::SimDose::Analysis controller.");

    fAddMap = std::make_unique<G4UIcmdWithAString>("/MACE/Analysis/AddMap", this);
    fAddMap->SetGuidance("Add a set of histograms named <name>EdepMap and <name>DoseMap");
    fAddMap->SetParameterName("name", true);
    fAddMap->SetDefaultValue("");
    fAddMap->AvailableForStates(G4State_Idle);

    fMapNBinX = std::make_unique<G4UIcmdWithAnInteger>("/MACE/Analysis/MapNBinX", this);
    fMapNBinX->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapNBinX->SetParameterName("n", false);
    fMapNBinX->SetRange("n > 0");
    fMapNBinX->AvailableForStates(G4State_Idle);

    fMapXMin = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Analysis/MapXMin", this);
    fMapXMin->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapXMin->SetParameterName("value", false);
    fMapXMin->SetUnitCategory("Length");
    fMapXMin->AvailableForStates(G4State_Idle);

    fMapXMax = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Analysis/MapXMax", this);
    fMapXMax->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapXMax->SetParameterName("value", false);
    fMapXMax->SetUnitCategory("Length");
    fMapXMax->AvailableForStates(G4State_Idle);

    fMapNBinY = std::make_unique<G4UIcmdWithAnInteger>("/MACE/Analysis/MapNBinY", this);
    fMapNBinY->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapNBinY->SetParameterName("n", false);
    fMapNBinY->SetRange("n > 0");
    fMapNBinY->AvailableForStates(G4State_Idle);

    fMapYMin = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Analysis/MapYMin", this);
    fMapYMin->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapYMin->SetParameterName("value", false);
    fMapYMin->SetUnitCategory("Length");
    fMapYMin->AvailableForStates(G4State_Idle);

    fMapYMax = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Analysis/MapYMax", this);
    fMapYMax->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapYMax->SetParameterName("value", false);
    fMapYMax->SetUnitCategory("Length");
    fMapYMax->AvailableForStates(G4State_Idle);

    fMapNBinZ = std::make_unique<G4UIcmdWithAnInteger>("/MACE/Analysis/MapNBinZ", this);
    fMapNBinZ->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapNBinZ->SetParameterName("n", false);
    fMapNBinZ->SetRange("n > 0");
    fMapNBinZ->AvailableForStates(G4State_Idle);

    fMapZMin = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Analysis/MapZMin", this);
    fMapZMin->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapZMin->SetParameterName("value", false);
    fMapZMin->SetUnitCategory("Length");
    fMapZMin->AvailableForStates(G4State_Idle);

    fMapZMax = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Analysis/MapZMax", this);
    fMapZMax->SetGuidance("Set bin parameter for the most recently added histogram.");
    fMapZMax->SetParameterName("value", false);
    fMapZMax->SetUnitCategory("Length");
    fMapZMax->AvailableForStates(G4State_Idle);
}

AnalysisMessenger::~AnalysisMessenger() = default;

auto AnalysisMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fAddMap.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.AddMap(value);
        });
    } else if (command == fMapNBinX.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapNBinX(fMapNBinX->GetNewIntValue(value));
        });
    } else if (command == fMapXMin.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapXMin(fMapXMin->GetNewDoubleValue(value));
        });
    } else if (command == fMapXMax.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapXMax(fMapXMax->GetNewDoubleValue(value));
        });
    } else if (command == fMapNBinY.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapNBinY(fMapNBinY->GetNewIntValue(value));
        });
    } else if (command == fMapYMin.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapYMin(fMapYMin->GetNewDoubleValue(value));
        });
    } else if (command == fMapYMax.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapYMax(fMapYMax->GetNewDoubleValue(value));
        });
    } else if (command == fMapNBinZ.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapNBinZ(fMapNBinZ->GetNewIntValue(value));
        });
    } else if (command == fMapZMin.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapZMin(fMapZMin->GetNewDoubleValue(value));
        });
    } else if (command == fMapZMax.get()) {
        Deliver<Analysis>([&](auto&& r) {
            r.MapZMax(fMapZMax->GetNewDoubleValue(value));
        });
    }
}

} // namespace MACE::SimDose::inline Messenger
