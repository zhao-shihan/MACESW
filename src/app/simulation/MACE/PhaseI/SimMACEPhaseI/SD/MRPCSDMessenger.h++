#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithADoubleAndUnit;
class G4UIdirectory;

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

class MRPCSD;

class MRPCSDMessenger final : public Mustard::Geant4X::SingletonMessenger<MRPCSDMessenger,
                                                                          MRPCSD> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MRPCSDMessenger();
    ~MRPCSDMessenger();

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fIonizingEnergyDepositionThreshold;
};

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
