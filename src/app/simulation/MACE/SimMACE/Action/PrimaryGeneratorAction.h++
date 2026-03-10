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

#pragma once

#include "MACE/Data/SimVertex.h++"
#include "MACE/SimMACE/Messenger/AnalysisMessenger.h++"
#include "MACE/SimMACE/Messenger/PrimaryGeneratorActionMessenger.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"
#include "Mustard/Geant4X/Generator/DataReaderPrimaryGenerator.h++"
#include "Mustard/Geant4X/Generator/GeneralParticleSourceX.h++"

#include "G4VUserPrimaryGeneratorAction.hh"

#include "muc/ptrvec"

namespace MACE::SimMACE::inline Action {

class PrimaryGeneratorAction final : public Mustard::Env::Memory::PassiveSingleton<PrimaryGeneratorAction>,
                                     public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();

    auto SwitchToGPSX() -> void { fGenerator = &fAvailableGenerator.gpsx; }
    auto SwitchToFromDataPrimaryGenerator() -> void { fGenerator = &fAvailableGenerator.dataReaderPrimaryGenerator; }

    auto SavePrimaryVertexData() const -> auto { return fSavePrimaryVertexData; }
    auto SavePrimaryVertexData(bool val) -> void { fSavePrimaryVertexData = val; }

    auto GeneratePrimaries(G4Event* event) -> void override;

private:
    auto UpdatePrimaryVertexData(const G4Event& event) -> void;

private:
    struct {
        Mustard::Geant4X::GeneralParticleSourceX gpsx;
        Mustard::Geant4X::DataReaderPrimaryGenerator dataReaderPrimaryGenerator;
    } fAvailableGenerator;
    G4VPrimaryGenerator* fGenerator;

    bool fSavePrimaryVertexData;
    muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>> fPrimaryVertexData;

    AnalysisMessenger::Register<PrimaryGeneratorAction> fAnalysisMessengerRegister;
    PrimaryGeneratorActionMessenger::Register<PrimaryGeneratorAction> fPrimaryGeneratorActionMessengerRegister;
};

} // namespace MACE::SimMACE::inline Action
