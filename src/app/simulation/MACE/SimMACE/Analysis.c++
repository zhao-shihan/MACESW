#include "MACE/SimMACE/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimMACE/Action/TrackingAction.h++"
#include "MACE/SimMACE/Analysis.h++"
#include "MACE/Simulation/Hit/CDCHit.h++"
#include "MACE/Simulation/Hit/ECALHit.h++"
#include "MACE/Simulation/Hit/MCPHit.h++"
#include "MACE/Simulation/Hit/TTCHit.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "fmt/core.h"

#include <algorithm>
#include <optional>
#include <stdexcept>

namespace MACE::SimMACE {

Analysis::Analysis() :
    AnalysisBase{this},
    fCoincidenceWithMMS{true},
    fCoincidenceWithMCP{true},
    fCoincidenceWithECAL{true},
    fSaveCDCHitData{true},
    fSaveTTCHitData{true},
    fPrimaryVertexOutput{},
    fDecayVertexOutput{},
    fTTCSimHitOutput{},
    fCDCSimHitOutput{},
    fMMSSimTrackOutput{},
    fMCPSimHitOutput{},
    fECALSimHitOutput{},
    fPrimaryVertex{},
    fDecayVertex{},
    fTTCHit{},
    fCDCHit{},
    fMCPHit{},
    fECALHit{},
    fMMSTruthTracker{},
    fMessengerRegister{this} {}

auto Analysis::RunBeginUserAction(int runID) -> void {
    if (PrimaryGeneratorAction::Instance().SavePrimaryVertexData()) {
        fPrimaryVertexOutput.emplace(fmt::format("G4Run{}/SimPrimaryVertex", runID));
    }
    if (TrackingAction::Instance().SaveDecayVertexData()) {
        fDecayVertexOutput.emplace(fmt::format("G4Run{}/SimDecayVertex", runID));
    }
    if (fSaveTTCHitData) {
        fTTCSimHitOutput.emplace(fmt::format("G4Run{}/TTCSimHit", runID));
    }
    if (fSaveCDCHitData) {
        fCDCSimHitOutput.emplace(fmt::format("G4Run{}/CDCSimHit", runID));
    }
    fMMSSimTrackOutput.emplace(fmt::format("G4Run{}/MMSSimTrack", runID));
    fMCPSimHitOutput.emplace(fmt::format("G4Run{}/MCPSimHit", runID));
    fECALSimHitOutput.emplace(fmt::format("G4Run{}/ECALSimHit", runID));
}

auto Analysis::EventEndUserAction() -> void {
    const auto mmsTrack{fCDCHit and fTTCHit ?
                            std::optional{fMMSTruthTracker(*fCDCHit, *fTTCHit)} :
                            std::nullopt};
    const auto mmsPassed{not fCoincidenceWithMMS or mmsTrack == std::nullopt or not mmsTrack->empty()};
    const auto mcpPassed{not fCoincidenceWithMCP or fMCPHit == nullptr or std::ranges::any_of(*fMCPHit, [](auto&& hit) { return Get<"Trig">(*hit); })};
    const auto ecalPassed{not fCoincidenceWithECAL or fECALHit == nullptr or not fECALHit->empty()};
    if (mmsPassed and mcpPassed and ecalPassed) {
        if (fPrimaryVertex and fPrimaryVertexOutput) {
            fPrimaryVertexOutput->Fill(*fPrimaryVertex);
        }
        if (fDecayVertex and fDecayVertexOutput) {
            fDecayVertexOutput->Fill(*fDecayVertex);
        }
        if (mmsTrack) {
            if (fTTCSimHitOutput) {
                fTTCSimHitOutput->Fill(*fTTCHit);
            }
            if (fCDCSimHitOutput) {
                fCDCSimHitOutput->Fill(*fCDCHit);
            }
            fMMSSimTrackOutput->Fill(*mmsTrack);
        }
        if (fMCPHit) {
            fMCPSimHitOutput->Fill(*fMCPHit);
        }
        if (fECALHit) {
            fECALSimHitOutput->Fill(*fECALHit);
        }
    }
    fPrimaryVertex = {};
    fDecayVertex = {};
    fCDCHit = {};
    fTTCHit = {};
    fMCPHit = {};
    fECALHit = {};
}

auto Analysis::RunEndUserAction(int) -> void {
    // write data
    if (fPrimaryVertexOutput) {
        fPrimaryVertexOutput->Write();
    }
    if (fDecayVertexOutput) {
        fDecayVertexOutput->Write();
    }
    if (fTTCSimHitOutput) {
        fTTCSimHitOutput->Write();
    }
    if (fCDCSimHitOutput) {
        fCDCSimHitOutput->Write();
    }
    fMMSSimTrackOutput->Write();
    fMCPSimHitOutput->Write();
    fECALSimHitOutput->Write();
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fTTCSimHitOutput.reset();
    fCDCSimHitOutput.reset();
    fMMSSimTrackOutput.reset();
    fMCPSimHitOutput.reset();
    fECALSimHitOutput.reset();
}

} // namespace MACE::SimMACE
