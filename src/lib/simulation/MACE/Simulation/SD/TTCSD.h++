#pragma once

#include "MACE/Simulation/Hit/TTCHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

namespace MACE::inline Simulation::inline SD {

class TTCSiPMSD;

class TTCSD : public G4VSensitiveDetector {
public:
    TTCSD(const G4String& sdName, const TTCSiPMSD* ttcSiPMSD = {});

    virtual auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    virtual auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    virtual auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    const TTCSiPMSD* const fTTCSiPMSD;

    double fEnergyDepositionThreshold;

    muc::flat_hash_map<int, muc::unique_ptrvec<TTCHit>> fSplitHit;
    TTCHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
