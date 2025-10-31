namespace MACE::inline Simulation::inline Messenger {

template<typename AReceiver>
NumericMessenger<AReceiver>::NumericMessenger() :
    Mustard::Geant4X::SingletonMessenger<NumericMessenger<AReceiver>, AReceiver>{},
    fDirectory{},
    fMinDriverStep{},
    fDeltaChord{} {
    // NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)
    fDirectory = std::make_unique<G4UIdirectory>("/MACE/Numeric/");
    fDirectory->SetGuidance("MACE numeric controller.");

    fMinDriverStep = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Numeric/MinDriverStep", this);
    fMinDriverStep->SetGuidance("Set h_min for particle stepper driver in field.");
    fMinDriverStep->SetParameterName("hMin", false);
    fMinDriverStep->SetUnitCategory("Length");
    fMinDriverStep->AvailableForStates(G4State_PreInit);

    fDeltaChord = std::make_unique<G4UIcmdWithADoubleAndUnit>("/MACE/Numeric/DeltaChord", this);
    fDeltaChord->SetGuidance("Set delta chord.");
    fDeltaChord->SetParameterName("delta", false);
    fDeltaChord->SetUnitCategory("Length");
    fDeltaChord->AvailableForStates(G4State_PreInit);
    // NOLINTEND(cppcoreguidelines-prefer-member-initializer)
}

template<typename AReceiver>
auto NumericMessenger<AReceiver>::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fMinDriverStep.get()) {
        this->template Deliver<AReceiver>([&](auto&& r) {
            r.MinDriverStep(fMinDriverStep->GetNewDoubleValue(value));
        });
    } else if (command == fDeltaChord.get()) {
        this->template Deliver<AReceiver>([&](auto&& r) {
            r.DeltaChord(fDeltaChord->GetNewDoubleValue(value));
        });
    }
}

} // namespace MACE::inline Simulation::inline Messenger
