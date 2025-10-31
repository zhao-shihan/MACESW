#pragma once

#include "MACE/PhaseI/Simulation/Hit/SciFiHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

namespace MACE::PhaseI::inline Simulation::inline SD {

class SciFiSD : public G4VSensitiveDetector {
public:
    explicit SciFiSD(const G4String& sdName);

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    muc::flat_hash_map<int, muc::unique_ptrvec<SciFiHit>> fSplitHit;
    SciFiHitCollection* fHitsCollection;
};

} // namespace MACE::PhaseI::inline Simulation::inline SD
