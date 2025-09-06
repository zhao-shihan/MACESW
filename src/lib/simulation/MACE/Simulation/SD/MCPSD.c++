#include "MACE/Detector/Description/MCP.h++"
#include "MACE/Simulation/SD/MCPSD.h++"

#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4DataInterpolation.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ParticleDefinition.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VProcess.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"

#include "muc/algorithm"

#include <cassert>
#include <cmath>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <tuple>

namespace MACE::inline Simulation::inline SD {

using namespace Mustard::LiteralUnit::Energy;

MCPSD::MCPSD(const G4String& sdName) :
    G4VSensitiveDetector{sdName},
    fIonizingEnergyDepositionThreshold{20_eV},
    fEfficiency{},
    fSplitHit{},
    fHitsCollection{},
    fMessengerRegister{this} {
    collectionName.insert(sdName + "HC");

    const auto& mcp{Detector::Description::MCP::Instance()};
    if (mcp.EfficiencyEnergy().size() != mcp.EfficiencyValue().size()) {
        Mustard::Throw<std::runtime_error>("mcp.EfficiencyEnergy().size() != mcp.EfficiencyValue().size()");
    }
    const auto n{mcp.EfficiencyEnergy().size()};
    fEfficiency = std::make_unique<G4DataInterpolation>(const_cast<double*>(mcp.EfficiencyEnergy().data()), // stupid interface accepts non-const ptr only
                                                        const_cast<double*>(mcp.EfficiencyValue().data()),  // stupid interface accepts non-const ptr only
                                                        n,
                                                        (mcp.EfficiencyValue()[1] - mcp.EfficiencyValue()[0]) / (mcp.EfficiencyEnergy()[1] - mcp.EfficiencyEnergy()[0]),
                                                        (mcp.EfficiencyValue()[n - 1] - mcp.EfficiencyValue()[n - 2]) / (mcp.EfficiencyEnergy()[n - 1] - mcp.EfficiencyEnergy()[n - 2]));
}

MCPSD::~MCPSD() = default;

auto MCPSD::Initialize(G4HCofThisEvent* hitsCollectionOfThisEvent) -> void {
    fHitsCollection = new MCPHitCollection{SensitiveDetectorName, collectionName[0]};
    auto hitsCollectionID{G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection)};
    hitsCollectionOfThisEvent->AddHitsCollection(hitsCollectionID, fHitsCollection);
}

auto MCPSD::ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool {
    const auto& step{*theStep};
    const auto eDep{step.GetTotalEnergyDeposit()};

    assert(0 <= step.GetNonIonizingEnergyDeposit());
    assert(step.GetNonIonizingEnergyDeposit() <= eDep);
    if (eDep - step.GetNonIonizingEnergyDeposit() < fIonizingEnergyDepositionThreshold) {
        return false;
    }

    const auto& track{*step.GetTrack()};
    const auto& particle{*track.GetDefinition()};
    const auto& preStepPoint{*step.GetPreStepPoint()};
    const auto& touchable{*preStepPoint.GetTouchable()};
    // transform hit position to local coordinate
    const G4TwoVector hitPosition{*touchable.GetRotation() * (preStepPoint.GetPosition() - touchable.GetTranslation())};
    // calculate (E0, p0)
    const auto vertexEk{track.GetVertexKineticEnergy()};
    const auto vertexMomentum{track.GetVertexMomentumDirection() * std::sqrt(vertexEk * (vertexEk + 2 * particle.GetPDGMass()))};
    // track creator process
    const auto creatorProcess{track.GetCreatorProcess()};
    // new a hit
    const auto& hit{fSplitHit.emplace_back(std::make_unique_for_overwrite<MCPHit>())};
    Get<"EvtID">(*hit) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    Get<"HitID">(*hit) = -1; // to be determined
    Get<"t">(*hit) = preStepPoint.GetGlobalTime();
    Get<"x">(*hit) = hitPosition;
    Get<"Trig">(*hit) = false; // to be determined
    Get<"Edep">(*hit) = eDep;
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

auto MCPSD::EndOfEvent(G4HCofThisEvent*) -> void {
    fHitsCollection->GetVector()->reserve(fSplitHit.size());

    switch (fSplitHit.size()) {
    case 0:
        break;
    case 1: {
        auto& hit{fSplitHit.front()};
        fHitsCollection->insert(hit.release());
    } break;
    default: {
        const auto timeResolutionFWHM{Detector::Description::MCP::Instance().TimeResolutionFWHM()};
        assert(timeResolutionFWHM >= 0);
        // sort hit by time
        muc::timsort(fSplitHit,
                     [](const auto& hit1, const auto& hit2) {
                         return Get<"t">(*hit1) < Get<"t">(*hit2);
                     });
        // loop over all hits and cluster to real hits by times
        std::ranges::subrange cluster{fSplitHit.begin(), fSplitHit.begin()};
        while (cluster.end() != fSplitHit.end()) {
            const auto tFirst{*Get<"t">(**cluster.end())};
            const auto windowClosingTime{tFirst + timeResolutionFWHM};
            if (tFirst == windowClosingTime and // Notice: bad numeric with huge Get<"t">(**clusterFirst)!
                timeResolutionFWHM != 0) [[unlikely]] {
                Mustard::PrintWarning(fmt::format("A huge time ({}) completely rounds off the time resolution ({})", tFirst, timeResolutionFWHM));
            }
            cluster = {cluster.end(), std::ranges::find_if_not(cluster.end(), fSplitHit.end(),
                                                               [&windowClosingTime](const auto& hit) {
                                                                   return Get<"t">(*hit) <= windowClosingTime;
                                                               })};
            // find top hit
            auto& topHit{*std::ranges::min_element(cluster,
                                                   [](const auto& hit1, const auto& hit2) {
                                                       return Get<"TrkID">(*hit1) < Get<"TrkID">(*hit2);
                                                   })};
            // construct real hit
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
    fSplitHit.clear();

    muc::timsort(*fHitsCollection->GetVector(),
                 [](const auto& hit1, const auto& hit2) {
                     return std::tie(Get<"TrkID">(*hit1), Get<"t">(*hit1)) <
                            std::tie(Get<"TrkID">(*hit2), Get<"t">(*hit2));
                 });

    auto& rng{*G4Random::getTheEngine()};
    for (int hitID{}; auto&& hit : *fHitsCollection->GetVector()) {
        if (rng.flat() > fEfficiency->CubicSplineInterpolation(Get<"Ek">(*hit))) {
            continue;
        }
        Get<"HitID">(*hit) = hitID++;
        Get<"Trig">(*hit) = true;
    }
}

} // namespace MACE::inline Simulation::inline SD
