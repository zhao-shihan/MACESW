#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithADoubleAndUnit;
class G4UIdirectory;

namespace MACE::inline Simulation::inline SD {

class CDCSD;

class CDCSDMessenger final : public Mustard::Geant4X::SingletonMessenger<CDCSDMessenger,
                                                                         CDCSD> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    CDCSDMessenger();
    ~CDCSDMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fIonizingEnergyDepositionThreshold;
};

} // namespace MACE::inline Simulation::inline SD
