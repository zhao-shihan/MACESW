#pragma once

#include "MACE/Simulation/Hit/TTCHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

namespace MACE::inline Simulation::inline SD {

class TTCSiPMSD;

class TTCSD : public G4VSensitiveDetector {
public:
    enum struct Type {
        MACE,
        MACEPhaseI
    };

public:
    TTCSD(const G4String& sdName, Type type, const TTCSiPMSD* ttcSiPMSD = {});

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    const TTCSiPMSD* fTTCSiPMSD;

    Type fType;

    double fEnergyDepositionThreshold;

    muc::flat_hash_map<int, muc::unique_ptrvec<TTCHit>> fSplitHit;
    TTCHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
