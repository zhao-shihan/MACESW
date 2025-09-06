#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithABool;
class G4UIdirectory;

namespace MACE::SimTarget {

inline namespace Action {

class SteppingAction;

} // namespace Action

inline namespace Messenger {

class ActionMessenger final : public Mustard::Geant4X::SingletonMessenger<ActionMessenger,
                                                                          SteppingAction> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    ActionMessenger();
    ~ActionMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithABool> fKillIrrelevance;
};

} // namespace Messenger

} // namespace MACE::SimTarget
