#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithoutParameter;

namespace MACE::inline Simulation::inline Physics {

class StandardPhysicsListBase;

class StandardPhysicsListMessenger final : public Mustard::Geant4X::SingletonMessenger<StandardPhysicsListMessenger,
                                                                                       StandardPhysicsListBase> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    StandardPhysicsListMessenger();
    ~StandardPhysicsListMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIcmdWithoutParameter> fUseRadioactiveDecayPhysics;
    std::unique_ptr<G4UIcmdWithoutParameter> fUseOpticalPhysics;
};

} // namespace MACE::inline Simulation::inline Physics
