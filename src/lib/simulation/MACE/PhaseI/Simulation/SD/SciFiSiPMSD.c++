#include "MACE/PhaseI/Simulation/SD/SciFiSiPMSD.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"

#include <cassert>

namespace MACE::PhaseI::inline Simulation::inline SD {

SciFiSiPMSD::SciFiSiPMSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fHit{},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");
}

auto SciFiSiPMSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHit.clear(); // clear at the begin of event allows TTCSD to get optical photon counts at the end of event

    fHitsCollection = new SciFiSiPMHitCollection(SensitiveDetectorName, collectionName[0]);
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto SciFiSiPMSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};

    if (&particle != G4OpticalPhoton::Definition()) {
        return false;
    }

    step.GetTrack()->SetTrackStatus(fStopAndKill);

    const auto postStepPoint{*step.GetPostStepPoint()};
    const auto siPMID{postStepPoint.GetTouchable()->GetReplicaNumber()};
    const auto epoxyID{postStepPoint.GetTouchable()->GetReplicaNumber(1)};
    const auto trueSiPMID{siPMID + 64 * epoxyID};
    // new a hit
    auto hit{std::make_unique_for_overwrite<SciFiSiPMHit>()};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"SiPMID">(*hit) = trueSiPMID;
    Get<"t">(*hit) = postStepPoint.GetGlobalTime();
    fHit[trueSiPMID].emplace_back(std::move(hit));
    return true;
}

auto SciFiSiPMSD::EndOfEvent(G4HCofThisEvent*) -> void {
    for (int hitID{};
         auto&& [siPMID, hitOfDetector] : fHit) {
        for (auto&& hit : hitOfDetector) {
            Get<"HitID">(*hit) = hitID++;
            assert(Get<"SiPMID">(*hit) == siPMID);
            fHitsCollection->insert(hit.release());
        }
    }
}

auto SciFiSiPMSD::NOpticalPhotonHit() const -> muc::flat_hash_map<int, int> {
    muc::flat_hash_map<int, int> nHit;
    for (auto&& [siPMID, hit] : fHit) {
        if (hit.size() > 0) {
            nHit[siPMID] = hit.size();
        }
    }
    return nHit;
}

} // namespace MACE::PhaseI::inline Simulation::inline SD
