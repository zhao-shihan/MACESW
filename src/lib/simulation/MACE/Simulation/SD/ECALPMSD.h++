#pragma once

#include "MACE/Simulation/Hit/ECALPMHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

namespace MACE::inline Simulation::inline SD {

class ECALPMSD : public G4VSensitiveDetector {
public:
    ECALPMSD(const G4String& sdName);

    virtual auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    virtual auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    virtual auto EndOfEvent(G4HCofThisEvent*) -> void override;

    auto NOpticalPhotonHit() const -> muc::flat_hash_map<int, int>;

protected:
    muc::flat_hash_map<int, muc::unique_ptrvec<ECALPMHit>> fHit;
    ECALPMHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
