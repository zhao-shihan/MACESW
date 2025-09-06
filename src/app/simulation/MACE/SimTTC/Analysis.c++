#include "MACE/SimTTC/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimTTC/Action/TrackingAction.h++"
#include "MACE/SimTTC/Analysis.h++"
#include "MACE/Simulation/Hit/TTCHit.h++"
#include "MACE/Simulation/Hit/TTCSiPMHit.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "fmt/format.h"

#include <stdexcept>

namespace MACE::SimTTC {

Analysis::Analysis() :
    AnalysisBase{this},
    fSaveTTCHitData{true},
    fSaveTTCSiPMHitData{true},
    fPrimaryVertexOutput{},
    fDecayVertexOutput{},
    fTTCSimHitOutput{},
    fTTCSiPMHitOutput{},
    fPrimaryVertex{},
    fDecayVertex{},
    fTTCHit{},
    fTTCSiPMHit{},
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
    if (fSaveTTCSiPMHitData) {
        fTTCSiPMHitOutput.emplace(fmt::format("G4Run{}/TTCSiPMHit", runID));
    }
}

auto Analysis::EventEndUserAction() -> void {
    if (fPrimaryVertex and fPrimaryVertexOutput) {
        fPrimaryVertexOutput->Fill(*fPrimaryVertex);
    }
    if (fDecayVertex and fDecayVertexOutput) {
        fDecayVertexOutput->Fill(*fDecayVertex);
    }
    if (fTTCSimHitOutput) {
        fTTCSimHitOutput->Fill(*fTTCHit);
    }
    if (fTTCSiPMHitOutput) {
        fTTCSiPMHitOutput->Fill(*fTTCSiPMHit);
    }
    fPrimaryVertex = {};
    fDecayVertex = {};
    fTTCHit = {};
    fTTCSiPMHit = {};
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
    if (fTTCSiPMHitOutput) {
        fTTCSiPMHitOutput->Write();
    }
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fTTCSimHitOutput.reset();
    fTTCSiPMHitOutput.reset();
}

} // namespace MACE::SimTTC
