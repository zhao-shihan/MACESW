#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/Simulation/SD/SciFiSD.h++"
#include "MACE/PhaseI/Simulation/SD/SciFiSiPMSD.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessType.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SteppingManager.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4TwoVector.hh"
#include "G4VProcess.hh"
#include "G4VTouchable.hh"

#include "muc/algorithm"
#include "muc/numeric"
#include "muc/utility"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iterator>
#include <numeric>
#include <string_view>
#include <utility>
#include <vector>

namespace MACE::PhaseI::inline Simulation::inline SD {

using namespace Mustard::LiteralUnit;

SciFiSD::SciFiSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fSplitHit{},
    fHitsCollection{} {
    collectionName.insert(sdName + "HC");
}

auto SciFiSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fSplitHit.clear(); // clear at the begin of event allows SciFiSD to get optical photon counts at the end of event

    fHitsCollection = new SciFiHitCollection(SensitiveDetectorName, collectionName[0]);
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto SciFiSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};

    if (&particle == G4OpticalPhoton::Definition()) {
        return false;
    }
    if (particle.GetPDGCharge() == 0) {
        return false;
    }
    // std::cout << track.GetParentID() << std::endl;

    const auto eDep{step.GetTotalEnergyDeposit()};
    const auto fEnergyDepositionThreshold = 1.24_keV;
    if (eDep < fEnergyDepositionThreshold) {
        return false;
    }
    const auto preStepPoint{*step.GetPreStepPoint()};

    const auto x{preStepPoint.GetPosition()};
    const auto fiberID{preStepPoint.GetTouchable()->GetReplicaNumber(1)};
    const auto creatorProcess{track.GetCreatorProcess()};
    const auto vertexEk{track.GetVertexKineticEnergy()};
    const auto vertexMomentum{track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};

    // new a hit
    const auto& hit{fSplitHit[fiberID].emplace_back(std::make_unique_for_overwrite<SciFiSimHit>())};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"TrkID">(*hit) = track.GetTrackID();
    Get<"FiberID">(*hit) = fiberID;
    Get<"x">(*hit) = x;
    Get<"t">(*hit) = preStepPoint.GetGlobalTime();
    Get<"MotherID">(*hit) = track.GetParentID();
    Get<"Edep">(*hit) = eDep;
    Get<"Ek">(*hit) = preStepPoint.GetKineticEnergy();
    Get<"p">(*hit) = preStepPoint.GetMomentum();
    Get<"PDGID">(*hit) = particle.GetPDGEncoding();
    Get<"Ek0">(*hit) = vertexEk;
    Get<"p0">(*hit) = vertexMomentum;
    *Get<"CreatProc">(*hit) = creatorProcess ? std::string_view{creatorProcess->GetProcessName()} : "|0>";
    Get<"x0">(*hit) = track.GetVertexPosition();
    return true;
}

auto SciFiSD::EndOfEvent(G4HCofThisEvent*) -> void {
    fHitsCollection->GetVector()->reserve(
        muc::ranges::accumulate(fSplitHit, 0,
                                [](auto&& count, auto&& cellHit) {
                                    return count + cellHit.second.size();
                                }));
    constexpr auto ByTrackID{
        [](const auto& hit1, const auto& hit2) {
            return Get<"EvtID">(*hit1) < Get<"EvtID">(*hit2);
        }};
    for (int hitID{};
         auto&& [trackID, splitHit] : fSplitHit) {
        switch (splitHit.size()) {
        case 0:
            muc::unreachable();
        case 1: {
            auto& hit{splitHit.front()};
            Get<"HitID">(*hit) = hitID++;
            fHitsCollection->insert(hit.release());
        } break;
        default: {
            const auto scintillationTimeConstant1{3_ns};
            assert(scintillationTimeConstant1 >= 0);
            // sort hit by time
            muc::timsort(splitHit,
                         [](const auto& hit1, const auto& hit2) {
                             return Get<"t">(*hit1) < Get<"t">(*hit2);
                         });
            // loop over all hits on this tile and cluster to real hits by times
            std::ranges::subrange cluster{splitHit.begin(), splitHit.begin()};
            while (cluster.end() != splitHit.end()) {
                const auto tFirst{*Get<"t">(**cluster.end())};
                const auto windowClosingTime{tFirst + scintillationTimeConstant1};
                if (tFirst == windowClosingTime and // Notice: bad numeric with huge Get<"t">(**clusterFirst)!
                    scintillationTimeConstant1 != 0) [[unlikely]] {
                }
                cluster = {cluster.end(), std::ranges::find_if_not(cluster.end(), splitHit.end(),
                                                                   [&windowClosingTime](const auto& hit) {
                                                                       return Get<"t">(*hit) <= windowClosingTime;
                                                                   })};
                // find top hit
                auto& topHit{*std::ranges::min_element(cluster, ByTrackID)};

                // construct real hit
                Get<"HitID">(*topHit) = hitID++;
                for (const auto& hit : cluster) {
                    if (hit == topHit) {
                        continue;
                    }
                    Get<"Edep">(*topHit) += Get<"Edep">(*hit);
                }
                fHitsCollection->insert(topHit.release());
            }

        } break;
        }
    }
    fSplitHit.clear();
}

} // namespace MACE::PhaseI::inline Simulation::inline SD
