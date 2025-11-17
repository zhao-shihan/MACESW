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

#include "MACE/SimTarget/Messenger/ActionMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4ParticleDefinition.hh"
#include "G4UserSteppingAction.hh"

namespace MACE::SimTarget::inline Action {

class SteppingAction final : public Mustard::Env::Memory::PassiveSingleton<SteppingAction>,
                             public G4UserSteppingAction {
public:
    SteppingAction();

    auto KillIrrelevance(G4bool val) -> void { fKillIrrelevance = val; }

    auto UserSteppingAction(const G4Step* step) -> void override;

private:
    const G4ParticleDefinition* const fMuonPlus;
    const G4ParticleDefinition* const fMuonium;
    const G4ParticleDefinition* const fAntimuonium;
    G4bool fKillIrrelevance;

    ActionMessenger::Register<SteppingAction> fMessengerRegister;
};

} // namespace MACE::SimTarget::inline Action
