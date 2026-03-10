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

#include "MACE/SimDose/Action/ActionInitialization.h++"
#include "MACE/SimDose/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimDose/Action/RunAction.h++"
#include "MACE/SimDose/Action/SteppingAction.h++"

namespace MACE::SimDose {

ActionInitialization::ActionInitialization() :
    PassiveSingleton{this},
    G4VUserActionInitialization{} {}

auto ActionInitialization::Build() const -> void {
    SetUserAction(new RunAction);
    SetUserAction(new PrimaryGeneratorAction);
    SetUserAction(new SteppingAction);
}

} // namespace MACE::SimDose
