#include "MACE/SimPTS/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimPTS/Action/TrackingAction.h++"
#include "MACE/SimPTS/Analysis.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "mplr/mplr.hpp"

#include "fmt/format.h"

#include <optional>
#include <stdexcept>

namespace MACE::SimPTS {

Analysis::Analysis() :
    PassiveSingleton{this},
    fFilePath{"SimPTS_untitled"},
    fFileMode{"NEW"},
    fLastUsedFullFilePath{},
    fFile{},
    fPrimaryVertexOutput{},
    fDecayVertexOutput{},
    fVirtualHitOutput{},
    fPrimaryVertex{},
    fDecayVertex{},
    fVirtualHit{},
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
        Mustard::Geant4X::ConvertGeometryToTMacro("SimPTS_gdml", "SimPTS.gdml")->Write();
    }
    // initialize outputs
    if (PrimaryGeneratorAction::Instance().SavePrimaryVertexData()) {
        fPrimaryVertexOutput.emplace(fmt::format("G4Run{}/SimPrimaryVertex", runID));
    }
    if (TrackingAction::Instance().SaveDecayVertexData()) {
        fDecayVertexOutput.emplace(fmt::format("G4Run{}/SimDecayVertex", runID));
    }
    fVirtualHitOutput.emplace(fmt::format("G4Run{}/VirtualHit", runID));
}

auto Analysis::EventEnd() -> void {
    if (fPrimaryVertex and fPrimaryVertexOutput) {
        fPrimaryVertexOutput->Fill(*fPrimaryVertex);
    }
    if (fDecayVertex and fDecayVertexOutput) {
        fDecayVertexOutput->Fill(*fDecayVertex);
    }
    if (fVirtualHit) {
        fVirtualHitOutput->Fill(*fVirtualHit);
    }
    fPrimaryVertex = {};
    fDecayVertex = {};
    fVirtualHit = {};
}

auto Analysis::RunEnd(Option_t* option) -> void {
    // write data
    if (fPrimaryVertexOutput) {
        fPrimaryVertexOutput->Write();
    }
    if (fDecayVertexOutput) {
        fDecayVertexOutput->Write();
    }
    fVirtualHitOutput->Write();
    // close file
    fFile->Close(option);
    delete fFile;
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fVirtualHitOutput.reset();
}

} // namespace MACE::SimPTS
