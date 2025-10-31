namespace MACE::inline Simulation::inline Messenger {

template<typename AReceiver>
NeutrinoKillerMessenger<AReceiver>::NeutrinoKillerMessenger() :
    Mustard::Geant4X::SingletonMessenger<NeutrinoKillerMessenger<AReceiver>, AReceiver>{},
    fEnableNeutrinoKiller{} {
    // NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)
    fEnableNeutrinoKiller = std::make_unique<G4UIcmdWithABool>("/MACE/Action/EnableNeutrinoKiller", this);
    fEnableNeutrinoKiller->SetGuidance("Kill neutrino in stepping action if enabled (enable by default).");
    fEnableNeutrinoKiller->SetParameterName("bool", false);
    fEnableNeutrinoKiller->AvailableForStates(G4State_Idle);
    // NOLINTEND(cppcoreguidelines-prefer-member-initializer)
}

template<typename AReceiver>
auto NeutrinoKillerMessenger<AReceiver>::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fEnableNeutrinoKiller.get()) {
        this->template Deliver<AReceiver>([&](auto&& r) {
            r.EnableNeutrinoKiller(fEnableNeutrinoKiller->GetNewBoolValue(value));
        });
    }
}

} // namespace MACE::inline Simulation::inline Messenger
