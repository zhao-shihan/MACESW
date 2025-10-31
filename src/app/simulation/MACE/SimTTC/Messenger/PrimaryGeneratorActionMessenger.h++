#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcommand;

namespace MACE::SimTTC {

inline namespace Action {
class PrimaryGeneratorAction;
} // namespace Action

inline namespace Messenger {

class PrimaryGeneratorActionMessenger final : public Mustard::Geant4X::SingletonMessenger<PrimaryGeneratorActionMessenger,
                                                                                          PrimaryGeneratorAction> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    PrimaryGeneratorActionMessenger();
    ~PrimaryGeneratorActionMessenger() override;

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIcommand> fSwitchToGPSX;
    std::unique_ptr<G4UIcommand> fSwitchToEcoMug;
};

} // namespace Messenger

} // namespace MACE::SimTTC
