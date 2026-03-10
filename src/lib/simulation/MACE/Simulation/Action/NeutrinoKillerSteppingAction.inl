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

namespace MACE::inline Simulation::inline Action {

template<typename ADerived>
NeutrinoKillerSteppingAction<ADerived>::NeutrinoKillerSteppingAction(ADerived* derived) :
    Mustard::Env::Memory::PassiveSingleton<ADerived>{derived},
    G4UserSteppingAction{},
    fEnableNeutrinoKiller{true},
    fMessengerRegister{static_cast<ADerived*>(this)} {}

template<typename ADerived>
auto NeutrinoKillerSteppingAction<ADerived>::EnableNeutrinoKiller(bool val) -> void {
    fEnableNeutrinoKiller = val;
    auto setPhysicalProcessActivation{
        [&, processTable = G4ProcessTable::GetProcessTable()](auto&& particle) {
            processTable->SetProcessActivation(fElectromagnetic, particle, not val);
            processTable->SetProcessActivation(fOptical, particle, not val);
            processTable->SetProcessActivation(fHadronic, particle, not val);
            processTable->SetProcessActivation(fPhotolepton_hadron, particle, not val);
            processTable->SetProcessActivation(fDecay, particle, not val);
            processTable->SetProcessActivation(fGeneral, particle, not val);
            processTable->SetProcessActivation(fParameterisation, particle, not val);
            processTable->SetProcessActivation(fPhonon, particle, not val);
            processTable->SetProcessActivation(fUCN, particle, not val);
        }};
    setPhysicalProcessActivation(G4NeutrinoE::Definition());
    setPhysicalProcessActivation(G4AntiNeutrinoE::Definition());
    setPhysicalProcessActivation(G4NeutrinoMu::Definition());
    setPhysicalProcessActivation(G4AntiNeutrinoMu::Definition());
    setPhysicalProcessActivation(G4NeutrinoTau::Definition());
    setPhysicalProcessActivation(G4AntiNeutrinoTau::Definition());
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

template<typename ADerived>
auto NeutrinoKillerSteppingAction<ADerived>::UserSteppingAction(const G4Step* step) -> void {
    switch (auto& track{*step->GetTrack()};
            muc::abs(track.GetParticleDefinition()->GetPDGEncoding())) {
    case 12:
    case 14:
    case 16:
        if (fEnableNeutrinoKiller) {
            track.SetTrackStatus(fKillTrackAndSecondaries);
        }
        SteppingActionForNeutrino(*step);
        break;
    default:
        SteppingActionWithoutNeutrino(*step);
        break;
    }
}

} // namespace MACE::inline Simulation::inline Action
