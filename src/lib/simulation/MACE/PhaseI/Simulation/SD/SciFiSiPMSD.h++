#pragma once

#include "MACE/PhaseI/Simulation/Hit/SciFiSiPMRawHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

namespace MACE::PhaseI::inline Simulation::inline SD {

class SciFiSiPMSD : public G4VSensitiveDetector {
public:
    SciFiSiPMSD(const G4String& sdName);

    virtual auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    virtual auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    virtual auto EndOfEvent(G4HCofThisEvent*) -> void override;

    auto NOpticalPhotonHit() const -> muc::flat_hash_map<int, int>;

protected:
    muc::flat_hash_map<int, muc::unique_ptrvec<SciFiSiPMRawHit>> fHit;
    SciFiSiPMRawHitCollection* fHitsCollection;
};

} // namespace MACE::PhaseI::inline Simulation::inline SD
