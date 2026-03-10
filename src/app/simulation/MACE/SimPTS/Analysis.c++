// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

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
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fVirtualHitOutput.reset();
    // close file
    fFile->Close(option);
    delete fFile;
}

} // namespace MACE::SimPTS
