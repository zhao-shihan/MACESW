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

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithoutParameter;

namespace MACE::inline Simulation::inline Physics {

class StandardPhysicsListBase;

class StandardPhysicsListMessenger final : public Mustard::Geant4X::SingletonMessenger<StandardPhysicsListMessenger,
                                                                                       StandardPhysicsListBase> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    StandardPhysicsListMessenger();
    ~StandardPhysicsListMessenger() override;

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIcmdWithoutParameter> fUseRadioactiveDecayPhysics;
    std::unique_ptr<G4UIcmdWithoutParameter> fUseOpticalPhysics;
};

} // namespace MACE::inline Simulation::inline Physics
