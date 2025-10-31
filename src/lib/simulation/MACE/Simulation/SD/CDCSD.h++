#pragma once

#include "MACE/Data/MMSTrack.h++"
#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Simulation/Hit/CDCHit.h++"
#include "MACE/Simulation/SD/CDCSDMessenger.h++"

#include "Mustard/Data/Tuple.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

#include <algorithm>
#include <vector>

namespace MACE::inline Simulation::inline SD {

class CDCSD : public G4VSensitiveDetector {
public:
    explicit CDCSD(const G4String& sdName);

    auto IonizingEnergyDepositionThreshold(double e) -> void { fIonizingEnergyDepositionThreshold = std::max(0., e); }

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    double fIonizingEnergyDepositionThreshold;

    double fMeanDriftVelocity;
    const std::vector<Detector::Description::CDC::CellInformation>* fCellMap;

    muc::flat_hash_map<int, muc::unique_ptrvec<CDCHit>> fSplitHit;
    CDCHitCollection* fHitsCollection;

    CDCSDMessenger::Register<CDCSD> fMessengerRegister;
};

} // namespace MACE::inline Simulation::inline SD
