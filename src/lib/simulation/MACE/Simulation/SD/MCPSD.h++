#pragma once

#include "MACE/Simulation/Hit/MCPHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/ptrvec"

#include <algorithm>

class G4DataInterpolation;

namespace MACE::inline Simulation::inline SD {

class MCPSD : public G4VSensitiveDetector {
public:
    explicit MCPSD(const G4String& sdName);
    ~MCPSD() override;

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    std::unique_ptr<G4DataInterpolation> fEfficiency;

    muc::unique_ptrvec<MCPHit> fSplitHit;
    MCPHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
