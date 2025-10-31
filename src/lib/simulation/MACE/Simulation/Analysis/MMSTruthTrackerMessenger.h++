#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithAnInteger;

namespace MACE::inline Simulation::Analysis {

class MMSTruthTracker;

class MMSTruthTrackerMessenger final : public Mustard::Geant4X::SingletonMessenger<MMSTruthTrackerMessenger,
                                                                                   MMSTruthTracker> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MMSTruthTrackerMessenger();
    ~MMSTruthTrackerMessenger() override;

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIcmdWithAnInteger> fMinNCDCHitForQualifiedTrack;
    std::unique_ptr<G4UIcmdWithAnInteger> fMinNTTCHitForQualifiedTrack;
};

} // namespace MACE::inline Simulation::Analysis
