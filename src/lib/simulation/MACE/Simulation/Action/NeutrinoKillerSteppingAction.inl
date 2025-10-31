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
