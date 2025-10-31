#include "MACE/Simulation/SD/CDCSD.h++"

#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VProcess.hh"

#include "muc/algorithm"
#include "muc/numeric"
#include "muc/utility"

#include "gsl/gsl"

#include <cmath>
#include <ranges>
#include <string_view>
#include <tuple>
#include <utility>

namespace MACE::inline Simulation::inline SD {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::VectorArithmeticOperator;

CDCSD::CDCSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fIonizingEnergyDepositionThreshold{25_eV},
    fMeanDriftVelocity{},
    fCellMap{},
    fSplitHit{},
    fHitsCollection{},
    fMessengerRegister{this} {
    collectionName.emplace_back(sdName + "HC");

    const auto& cdc{Detector::Description::CDC::Instance()};
    fMeanDriftVelocity = cdc.MeanDriftVelocity();
    fCellMap = &cdc.CellMap();

    fSplitHit.reserve(fCellMap->size());
}

auto CDCSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHitsCollection = new CDCHitCollection{SensitiveDetectorName, collectionName[0]};
    const auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto CDCSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto eDep{step.GetTotalEnergyDeposit()};

    Expects(0 <= step.GetNonIonizingEnergyDeposit());
    Expects(step.GetNonIonizingEnergyDeposit() <= eDep);
    if (eDep - step.GetNonIonizingEnergyDeposit() < fIonizingEnergyDepositionThreshold) {
        return false;
    }

    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};
    const auto& preStepPoint{*step.GetPreStepPoint()};
    const auto& postStepPoint{*step.GetPostStepPoint()};
    const auto& touchable{*preStepPoint.GetTouchable()};
    const auto position{muc::midpoint(preStepPoint.GetPosition(), postStepPoint.GetPosition())};
    // retrieve wire position
    const auto cellID{touchable.GetReplicaNumber(1)};
    const auto& cellInfo{fCellMap->at(cellID)};
    Ensures(cellID == cellInfo.cellID);
    const auto xWire{Mustard::VectorCast<G4TwoVector>(cellInfo.position)};
    const auto tWire{Mustard::VectorCast<G4ThreeVector>(cellInfo.direction)};
    // calculate drift distance
    double driftDistance{};
    if (const auto pHat{muc::midpoint(preStepPoint.GetMomentumDirection(), postStepPoint.GetMomentumDirection())};
        not pHat.isParallel(tWire)) {
        const auto n{tWire.cross(pHat)};
        driftDistance = std::abs((position - xWire).dot(n)) / n.mag();
    } else {
        const auto delta{position - xWire};
        const auto n{tWire.dot(delta) * tWire - delta}; // == t x (t x delta)
        const auto n2{n.mag2()};
        driftDistance = std::isnormal(n2) ? std::abs(delta.dot(n)) / std::sqrt(n2) : 0;
    }
    const auto driftTime{driftDistance / fMeanDriftVelocity};
    const auto hitTime{muc::midpoint(preStepPoint.GetGlobalTime(), postStepPoint.GetGlobalTime())};
    const auto signalTime{hitTime + driftTime};
    // vertex Ek and p
    const auto vertexEk{track.GetVertexKineticEnergy()};
    const auto vertexMomentum{track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};
    // track creator process
    const auto creatorProcess{track.GetCreatorProcess()};
    // new a hit
    const auto& hit{fSplitHit[cellID].emplace_back(std::make_unique_for_overwrite<CDCHit>())};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"CellID">(*hit) = cellID;
    Get<"t">(*hit) = signalTime;
    Get<"Edep">(*hit) = eDep;
    Get<"d">(*hit) = driftDistance;
    Get<"Good">(*hit) = false; // to be determined
    Get<"tHit">(*hit) = hitTime;
    Get<"x">(*hit) = position;
    Get<"Ek">(*hit) = preStepPoint.GetKineticEnergy();
    Get<"p">(*hit) = preStepPoint.GetMomentum();
    Get<"TrkID">(*hit) = track.GetTrackID();
    Get<"PDGID">(*hit) = particle.GetPDGEncoding();
    Get<"t0">(*hit) = track.GetGlobalTime() - track.GetLocalTime();
    Get<"x0">(*hit) = track.GetVertexPosition();
    Get<"Ek0">(*hit) = vertexEk;
    Get<"p0">(*hit) = vertexMomentum;
    *Get<"CreatProc">(*hit) = creatorProcess ? std::string_view{creatorProcess->GetProcessName()} : "|0>";

    return true;
}

auto CDCSD::EndOfEvent(G4HCofThisEvent*) -> void {
    fHitsCollection->GetVector()->reserve(
        muc::ranges::accumulate(fSplitHit, 0,
                                [](auto&& count, auto&& cellHit) {
                                    return count + cellHit.second.size();
                                }));

    for (auto&& [cellID, splitHit] : fSplitHit) {
        switch (splitHit.size()) {
        case 0:
            muc::unreachable();
        case 1: {
            auto& hit{splitHit.front()};
            Ensures(Get<"CellID">(*hit) == cellID);
            fHitsCollection->insert(hit.release());
        } break;
        default: {
            const auto timeResolutionFWHM{Detector::Description::CDC::Instance().TimeResolutionFWHM()};
            Expects(timeResolutionFWHM >= 0);
            // sort hit by signal time
            muc::timsort(splitHit,
                         [](const auto& hit1, const auto& hit2) {
                             return Get<"t">(*hit1) < Get<"t">(*hit2);
                         });
            // loop over all hits on this cell and cluster to real hits by signal times
            std::ranges::subrange cluster{splitHit.begin(), splitHit.begin()};
            while (cluster.end() != splitHit.end()) {
                const auto tFirst{*Get<"t">(**cluster.end())};
                const auto windowClosingTime{tFirst + timeResolutionFWHM};
                if (tFirst == windowClosingTime and // Notice: bad numeric with huge Get<"t">(**clusterFirst)!
                    timeResolutionFWHM != 0) [[unlikely]] {
                    Mustard::PrintWarning(fmt::format("A huge time ({}) completely rounds off the time resolution ({})", tFirst, timeResolutionFWHM));
                }
                cluster = {cluster.end(), std::ranges::find_if_not(cluster.end(), splitHit.end(),
                                                                   [&windowClosingTime](const auto& hit) {
                                                                       return Get<"t">(*hit) <= windowClosingTime;
                                                                   })};
                // find top hit
                auto& topHit{*std::ranges::min_element(cluster,
                                                       [](const auto& hit1, const auto& hit2) {
                                                           return Get<"TrkID">(*hit1) < Get<"TrkID">(*hit2);
                                                       })};
                // construct real hit
                Ensures(Get<"CellID">(*topHit) == cellID);
                auto nTopHit{1};
                for (const auto& hit : cluster) {
                    if (hit == topHit) {
                        continue;
                    }
                    Get<"Edep">(*topHit) += Get<"Edep">(*hit); // sum
                    if (Get<"TrkID">(*hit) == Get<"TrkID">(*topHit)) {
                        ++nTopHit;
                        Get<"tHit">(*topHit) += Get<"tHit">(*hit); // mean
                        *Get<"x">(*topHit) += *Get<"x">(*hit);     // mean
                    }
                }
                Get<"tHit">(*topHit) /= nTopHit; // mean
                *Get<"x">(*topHit) /= nTopHit;   // mean
                fHitsCollection->insert(topHit.release());
            }
        } break;
        }
    }
    fSplitHit.clear();

    muc::timsort(*fHitsCollection->GetVector(),
                 [](const auto& hit1, const auto& hit2) {
                     return std::tie(Get<"TrkID">(*hit1), Get<"tHit">(*hit1)) <
                            std::tie(Get<"TrkID">(*hit2), Get<"tHit">(*hit2));
                 });

    for (int hitID{}; auto&& hit : *fHitsCollection->GetVector()) {
        Get<"HitID">(*hit) = hitID++;
    }
}

} // namespace MACE::inline Simulation::inline SD
