#pragma once

#include "MACE/Simulation/Hit/ECALPMHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

namespace MACE::inline Simulation::inline SD {

class ECALPMSD : public G4VSensitiveDetector {
public:
    explicit ECALPMSD(const G4String& sdName);

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

    auto NOpticalPhotonHit() const -> muc::flat_hash_map<int, int>;

protected:
    muc::flat_hash_map<int, muc::unique_ptrvec<ECALPMHit>> fHit;
    ECALPMHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
