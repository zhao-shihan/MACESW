#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIdirectory.hh"

#include <memory>

namespace MACE::inline Simulation::inline Messenger {

template<typename AReceiver>
class NumericMessenger final : public Mustard::Geant4X::SingletonMessenger<NumericMessenger<AReceiver>,
                                                                           AReceiver> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    NumericMessenger();
    ~NumericMessenger() = default;

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMinDriverStep;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fDeltaChord;
};

} // namespace MACE::inline Simulation::inline Messenger

#include "MACE/Simulation/Messenger/NumericMessenger.inl"
