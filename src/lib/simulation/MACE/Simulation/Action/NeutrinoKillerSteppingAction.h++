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

#include "MACE/Simulation/Action/NeutrinoKillerMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessTable.hh"
#include "G4ProcessType.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4UserSteppingAction.hh"

#include "muc/math"

#include "gsl/gsl"

namespace MACE::inline Simulation::inline Action {

template<typename ADerived = void>
class NeutrinoKillerSteppingAction : public Mustard::Env::Memory::PassiveSingleton<ADerived>,
                                     public G4UserSteppingAction {
public:
    explicit NeutrinoKillerSteppingAction(ADerived* derived);

    auto EnableNeutrinoKiller(bool val) -> void;

    auto UserSteppingAction(const G4Step* step) -> void final;

private:
    virtual auto SteppingActionForNeutrino(const G4Step&) -> void {}
    virtual auto SteppingActionWithoutNeutrino(const G4Step& step) -> void = 0;

private:
    bool fEnableNeutrinoKiller;

    NeutrinoKillerMessenger<ADerived>::template Register<ADerived> fMessengerRegister;
};

template<>
class NeutrinoKillerSteppingAction<void> final : public NeutrinoKillerSteppingAction<NeutrinoKillerSteppingAction<void>> {
public:
    NeutrinoKillerSteppingAction();

private:
    auto SteppingActionWithoutNeutrino(const G4Step&) -> void override {}
};

} // namespace MACE::inline Simulation::inline Action

#include "MACE/Simulation/Action/NeutrinoKillerSteppingAction.inl"
