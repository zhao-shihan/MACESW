#include "MACE/Detector/Description/TTC.h++"
#include "MACE/PhaseI/Detector/Description/TTC.h++"
#include "MACE/Simulation/SD/TTCSiPMSD.h++"

#include "Mustard/Utility/PhysicalConstant.h++"

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

namespace MACE::inline Simulation::inline SD {

using namespace Mustard::PhysicalConstant;

TTCSiPMSD::TTCSiPMSD(const G4String& sdName, const Type type) :
    G4VSensitiveDetector{sdName},
    type{type},
    fHit{},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");
}

auto TTCSiPMSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHit.clear(); // clear at the begin of event allows TTCSD to get optical photon counts at the end of event

    fHitsCollection = new TTCSiPMHitCollection(SensitiveDetectorName, collectionName[0]);
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto TTCSiPMSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};
    const auto& nSiPM{(type == TTCSiPMSD::Type::MACE) ? MACE::Detector::Description::TTC::Instance().NSiPM() : MACE::PhaseI::Detector::Description::TTC::Instance().NSiPM()};

    if (&particle != G4OpticalPhoton::Definition()) {
        return false;
    }

    step.GetTrack()->SetTrackStatus(fStopAndKill);

    const auto postStepPoint{*step.GetPostStepPoint()};
    const auto preStepPoint{*step.GetPreStepPoint()};
    const auto tileID{postStepPoint.GetTouchable()->GetReplicaNumber(2)};
    const auto siPMLocalID{postStepPoint.GetTouchable()->GetReplicaNumber(1)};
    const auto position{postStepPoint.GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(postStepPoint.GetPosition())};
    // new a hit
    auto hit{std::make_unique_for_overwrite<TTCSiPMHit>()};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"TileID">(*hit) = tileID;
    Get<"SiPMID">(*hit) = tileID * nSiPM - siPMLocalID;
    Get<"t">(*hit) = postStepPoint.GetGlobalTime();
    Get<"x">(*hit) = {position.x(), position.z()};
    Get<"k">(*hit) = preStepPoint.GetTouchable()->GetHistory()->GetTopTransform().TransformAxis(preStepPoint.GetMomentumDirection()) / hbar_Planck;
    fHit[tileID].emplace_back(std::move(hit));

    return true;
}

auto TTCSiPMSD::EndOfEvent(G4HCofThisEvent*) -> void {
    for (int hitID{};
         auto&& [tileID, hitOfDetector] : fHit) {
        for (auto&& hit : hitOfDetector) {
            Get<"HitID">(*hit) = hitID++;
            assert(Get<"TileID">(*hit) == tileID);
            fHitsCollection->insert(hit.release());
        }
    }
}

auto TTCSiPMSD::NOpticalPhotonHit() const -> muc::flat_hash_map<int, std::vector<int>> {
    muc::flat_hash_map<int, std::vector<int>> nHit;
    for (auto&& [tileID, hitofDetector] : fHit) {
        if (hitofDetector.size() > 0) {
            auto upSiPMNOpticalPhotonHit = std::ranges::count_if(hitofDetector, [](const auto& hit) { return Get<"SiPMID">(*hit) % 2 != 0; });
            auto downSiPMNOpticalPhotonHit = hitofDetector.size() - upSiPMNOpticalPhotonHit;
            nHit[tileID].emplace_back(upSiPMNOpticalPhotonHit);
            nHit[tileID].emplace_back(downSiPMNOpticalPhotonHit);
        }
    }
    return nHit;
}

} // namespace MACE::inline Simulation::inline SD
