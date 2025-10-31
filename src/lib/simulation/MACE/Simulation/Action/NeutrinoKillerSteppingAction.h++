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
