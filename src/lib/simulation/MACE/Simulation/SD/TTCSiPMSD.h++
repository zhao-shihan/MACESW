#pragma once

#include "MACE/Simulation/Hit/TTCSiPMHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

#include <vector>

namespace MACE::inline Simulation::inline SD {

class TTCSiPMSD : public G4VSensitiveDetector {
public:
    enum struct Type {
        MACE,
        MACEPhaseI
    };

public:
    TTCSiPMSD(const G4String& sdName, Type type);

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

    auto NOpticalPhotonHit() const -> muc::flat_hash_map<int, std::vector<int>>;

protected:
    Type fType;
    muc::flat_hash_map<int, muc::unique_ptrvec<TTCSiPMHit>> fHit;
    TTCSiPMHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
