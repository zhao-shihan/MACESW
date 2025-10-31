#pragma once

#include "MACE/SimPTS/Hit/VirtualHit.h++"

#include "G4VSensitiveDetector.hh"

namespace MACE::SimPTS::inline SD {

class VirtualSD : public G4VSensitiveDetector {
public:
    explicit VirtualSD(const G4String& sdName);

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    VirtualHitCollection* fHitsCollection;
};

} // namespace MACE::SimPTS::inline SD
