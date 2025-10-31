#pragma once

#include "MACE/PhaseI/SimMACEPhaseI/SD/MRPCSDMessenger.h++"
#include "MACE/PhaseI/Simulation/Hit/MRPCHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

#include <algorithm>

class G4DataInterpolation;

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

class MRPCSD : public G4VSensitiveDetector {
public:
    explicit MRPCSD(const G4String& sdName);
    ~MRPCSD() override;

    auto IonizingEnergyDepositionThreshold(double e) -> void { fIonizingEnergyDepositionThreshold = std::max(0., e); }

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    double fIonizingEnergyDepositionThreshold;

    muc::flat_hash_map<int, muc::unique_ptrvec<MRPCHit>> fSplitHit;
    MRPCHitCollection* fHitsCollection;

    MRPCSDMessenger::Register<MRPCSD> fMessengerRegister;
};

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
