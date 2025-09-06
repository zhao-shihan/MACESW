#include "MACE/SimECAL/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimECAL/Action/TrackingAction.h++"
#include "MACE/SimECAL/Analysis.h++"
#include "MACE/Simulation/Hit/ECALHit.h++"
#include "MACE/Simulation/Hit/ECALPMHit.h++"
#include "MACE/Simulation/Hit/MCPHit.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "mplr/mplr.hpp"

#include "fmt/core.h"

#include <algorithm>
#include <stdexcept>

namespace MACE::SimECAL {

Analysis::Analysis() :
    PassiveSingleton{this},
    fFilePath{"SimECAL_untitled"},
    fFileMode{"NEW"},
    fCoincidenceWithECAL{true},
    fCoincidenceWithMCP{false},
    fLastUsedFullFilePath{},
    fFile{},
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

auto Analysis::RunBegin(G4int runID) -> void {
    // open ROOT file
    auto fullFilePath{Mustard::Parallel::ProcessSpecificPath(fFilePath).replace_extension(".root").generic_string()};
    const auto filePathChanged{fullFilePath != fLastUsedFullFilePath};
    fFile = TFile::Open(fullFilePath.c_str(), filePathChanged ? fFileMode.c_str() : "UPDATE",
                        "", ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose);
    if (fFile == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot open file '{}' with mode '{}'",
                                                       fullFilePath, fFileMode));
    }
    fLastUsedFullFilePath = std::move(fullFilePath);
    // save geometry
    if (filePathChanged and mplr::comm_world().rank() == 0) {
        Mustard::Geant4X::ConvertGeometryToTMacro("SimECAL_gdml", "SimECAL.gdml")->Write();
    }
    // initialize outputs
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

auto Analysis::EventEnd() -> void {
    const auto ecalPassed{not fCoincidenceWithECAL or fECALHit == nullptr or fECALHit->size() > 0};
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

auto Analysis::RunEnd(Option_t* option) -> void {
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
    // close file
    fFile->Close(option);
    delete fFile;
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fECALSimHitOutput.reset();
    fECALPMHitOutput.reset();
    fMCPSimHitOutput.reset();
}

} // namespace MACE::SimECAL
