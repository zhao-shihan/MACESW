#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithABool;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;

namespace MACE::SimMACE {

inline namespace Action {
class DetectorConstruction;
} // namespace Action
class PhysicsList;

inline namespace Messenger {

class PhysicsMessenger final : public Mustard::Geant4X::SingletonMessenger<PhysicsMessenger,
                                                                           DetectorConstruction,
                                                                           PhysicsList> {
    friend class Mustard::Env::Memory::SingletonInstantiator;

private:
    PhysicsMessenger();
    ~PhysicsMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIcmdWithABool> fApplyProductionCutNearTarget;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMACEBiasPxySofteningFactor;
    std::unique_ptr<G4UIcmdWithADouble> fMACEBiasCosSofteningFactor;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMACEBiasEkLow;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMACEBiasEkSofteningFactor;
    std::unique_ptr<G4UIcmdWithABool> fApplyMACEBias;
};

} // namespace Messenger

} // namespace MACE::SimMACE
