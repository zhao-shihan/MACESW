#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Simulation/SD/ECALPMSD.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"

#include "muc/algorithm"

#include "gsl/gsl"

#include <cassert>

namespace MACE::inline Simulation::inline SD {

ECALPMSD::ECALPMSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fHit{},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");
}

auto ECALPMSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHit.clear(); // clear at the begin of event allows ECALSD to get optical photon counts at the end of event

    fHitsCollection = new ECALPMHitCollection(SensitiveDetectorName, collectionName[0]);
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto ECALPMSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};

    if (&particle != G4OpticalPhoton::Definition()) {
        return false;
    }

    step.GetTrack()->SetTrackStatus(fStopAndKill);

    const auto postStepPoint{*step.GetPostStepPoint()};
    const auto modID{postStepPoint.GetTouchable()->GetReplicaNumber(1)};
    // new a hit
    const auto& hit{fHit[modID].emplace_back(std::make_unique_for_overwrite<ECALPMHit>())};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"ModID">(*hit) = modID;
    Get<"t">(*hit) = postStepPoint.GetGlobalTime();

    return true;
}

auto ECALPMSD::EndOfEvent(G4HCofThisEvent*) -> void {
    const auto integralTime{Detector::Description::ECAL::Instance().WaveformIntegralTime()};
    Expects(integralTime >= 0);

    for (int hitID{};
         auto&& [modID, hitOfUnit] : fHit) {
        double initialTime{};
        muc::timsort(hitOfUnit,
                     [](auto&& hit1, auto&& hit2) {
                         return Get<"t">(*hit1) < Get<"t">(*hit2);
                     });
        for (auto&& hit : hitOfUnit) {
            if (hitID == 0) {
                initialTime = Get<"t">(*hit);
            } else if (Get<"t">(*hit) - initialTime > integralTime) {
                break;
            }
            Get<"HitID">(*hit) = hitID++;
            Ensures(Get<"ModID">(*hit) == modID);
            fHitsCollection->insert(hit.release());
        }
    }
}

auto ECALPMSD::NOpticalPhotonHit() const -> muc::flat_hash_map<int, int> {
    muc::flat_hash_map<int, int> nHit;
    for (auto&& [modID, hit] : fHit) {
        if (not hit.empty()) {
            nHit[modID] = hit.size();
        }
    }
    return nHit;
}

} // namespace MACE::inline Simulation::inline SD
