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

#include "MACE/Simulation/Physics/StandardPhysicsListMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "QBBC.hh"

namespace MACE::inline Simulation::inline Physics {

class StandardPhysicsListBase : public QBBC {
public:
    StandardPhysicsListBase();
    ~StandardPhysicsListBase() override = default;

    auto UseRadioactiveDecayPhysics() -> void;
    auto UseOpticalPhysics() -> void;

private:
    StandardPhysicsListMessenger::Register<StandardPhysicsListBase> fMessengerRegister;
};

class StandardPhysicsList final : public Mustard::Env::Memory::PassiveSingleton<StandardPhysicsList>,
                                  public StandardPhysicsListBase {
public:
    StandardPhysicsList();
};

} // namespace MACE::inline Simulation::inline Physics
