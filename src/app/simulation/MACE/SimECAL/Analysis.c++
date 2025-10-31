#include "MACE/SimECAL/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimECAL/Action/TrackingAction.h++"
#include "MACE/SimECAL/Analysis.h++"
#include "MACE/Simulation/Hit/ECALHit.h++"
#include "MACE/Simulation/Hit/ECALPMHit.h++"
#include "MACE/Simulation/Hit/MCPHit.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "fmt/core.h"

#include <algorithm>
#include <optional>
#include <stdexcept>

namespace MACE::SimECAL {

Analysis::Analysis() :
    AnalysisBase{this},
    fCoincidenceWithECAL{true},
    fCoincidenceWithMCP{false},
    fPrimaryVertexOutput{},
    fDecayVertexOutput{},
    fECALSimHitOutput{},
    fECALPMHitOutput{},
    fMCPSimHitOutput{},
    fPrimaryVertex{},
    fDecayVertex{},
    fECALHit{},
    fECALPMHit{},
    fMCPHit{},
    fMessengerRegister{this} {}

auto Analysis::RunBeginUserAction(int runID) -> void {
    if (PrimaryGeneratorAction::Instance().SavePrimaryVertexData()) {
        fPrimaryVertexOutput.emplace(fmt::format("G4Run{}/SimPrimaryVertex", runID));
    }
    if (TrackingAction::Instance().SaveDecayVertexData()) {
        fDecayVertexOutput.emplace(fmt::format("G4Run{}/SimDecayVertex", runID));
    }
    fECALSimHitOutput.emplace(fmt::format("G4Run{}/ECALSimHit", runID));
    fECALPMHitOutput.emplace(fmt::format("G4Run{}/ECALPMHit", runID));
    fMCPSimHitOutput.emplace(fmt::format("G4Run{}/MCPSimHit", runID));
}

auto Analysis::EventEndUserAction() -> void {
    const auto ecalPassed{not fCoincidenceWithECAL or fECALHit == nullptr or not fECALHit->empty()};
    const auto mcpPassed{not fCoincidenceWithMCP or fMCPHit == nullptr or std::ranges::any_of(*fMCPHit, [](auto&& hit) { return Get<"Trig">(*hit); })};
    if (ecalPassed and mcpPassed) {
        if (fPrimaryVertex and fPrimaryVertexOutput) {
            fPrimaryVertexOutput->Fill(*fPrimaryVertex);
        }
        if (fDecayVertex and fDecayVertexOutput) {
            fDecayVertexOutput->Fill(*fDecayVertex);
        }
        if (fECALHit) {
            fECALSimHitOutput->Fill(*fECALHit);
        }
        if (fECALPMHit) {
            fECALPMHitOutput->Fill(*fECALPMHit);
        }
        if (fMCPHit) {
            fMCPSimHitOutput->Fill(*fMCPHit);
        }
    }
    fPrimaryVertex = {};
    fDecayVertex = {};
    fECALHit = {};
    fECALPMHit = {};
    fMCPHit = {};
}

auto Analysis::RunEndUserAction(int) -> void {
    // write data
    if (fPrimaryVertexOutput) {
        fPrimaryVertexOutput->Write();
    }
    if (fDecayVertexOutput) {
        fDecayVertexOutput->Write();
    }
    fECALSimHitOutput->Write();
    fECALPMHitOutput->Write();
    fMCPSimHitOutput->Write();
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fECALSimHitOutput.reset();
    fECALPMHitOutput.reset();
    fMCPSimHitOutput.reset();
}

} // namespace MACE::SimECAL
