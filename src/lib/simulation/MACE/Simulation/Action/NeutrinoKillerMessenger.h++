#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include "G4UIcmdWithABool.hh"

#include <memory>

namespace MACE::inline Simulation::inline Messenger {

template<typename AReceiver>
class NeutrinoKillerMessenger final : public Mustard::Geant4X::SingletonMessenger<NeutrinoKillerMessenger<AReceiver>,
                                                                                  AReceiver> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    NeutrinoKillerMessenger();
    ~NeutrinoKillerMessenger() override = default;

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIcmdWithABool> fEnableNeutrinoKiller;
};

} // namespace MACE::inline Simulation::inline Messenger

#include "MACE/Simulation/Action/NeutrinoKillerMessenger.inl"
