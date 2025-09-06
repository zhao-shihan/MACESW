#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithADoubleAndUnit;
class G4UIdirectory;

namespace MACE::inline Simulation::inline SD {

class MCPSD;

class MCPSDMessenger final : public Mustard::Geant4X::SingletonMessenger<MCPSDMessenger,
                                                                         MCPSD> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MCPSDMessenger();
    ~MCPSDMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fIonizingEnergyDepositionThreshold;
};

} // namespace MACE::inline Simulation::inline SD
