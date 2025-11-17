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

#include "MACE/SimMMS/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimMMS/Action/TrackingAction.h++"
#include "MACE/SimMMS/Analysis.h++"
#include "MACE/Simulation/Hit/CDCHit.h++"
#include "MACE/Simulation/Hit/TTCHit.h++"
#include "MACE/Simulation/Hit/TTCSiPMHit.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "fmt/format.h"

#include <stdexcept>

namespace MACE::SimMMS {

Analysis::Analysis() :
    AnalysisBase{this},
    fSaveCDCHitData{true},
    fSaveTTCHitData{true},
    fSaveTTCSiPMHitData{true},
    fPrimaryVertexOutput{},
    fDecayVertexOutput{},
    fTTCSimHitOutput{},
    fTTCSiPMHitOutput{},
    fCDCSimHitOutput{},
    fMMSSimTrackOutput{},
    fPrimaryVertex{},
    fDecayVertex{},
    fCDCHit{},
    fTTCHit{},
    fTTCSiPMHit{},
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
    if (fSaveTTCSiPMHitData) {
        fTTCSiPMHitOutput.emplace(fmt::format("G4Run{}/TTCSiPMHit", runID));
    }
    fMMSSimTrackOutput.emplace(fmt::format("G4Run{}/MMSSimTrack", runID));
}

auto Analysis::EventEndUserAction() -> void {
    const auto mmsTrack{fCDCHit and fTTCHit ?
                            std::optional{fMMSTruthTracker(*fCDCHit, *fTTCHit)} :
                            std::nullopt};
    const auto mmsPassed{mmsTrack == std::nullopt or not mmsTrack->empty()};
    if (mmsPassed) {
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
            if (fTTCSiPMHitOutput) {
                fTTCSiPMHitOutput->Fill(*fTTCSiPMHit);
            }
            if (fCDCSimHitOutput) {
                fCDCSimHitOutput->Fill(*fCDCHit);
            }
            fMMSSimTrackOutput->Fill(*mmsTrack);
        }
    }
    fPrimaryVertex = {};
    fDecayVertex = {};
    fCDCHit = {};
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
    if (fCDCSimHitOutput) {
        fCDCSimHitOutput->Write();
    }
    fMMSSimTrackOutput->Write();
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fTTCSimHitOutput.reset();
    fTTCSiPMHitOutput.reset();
    fCDCSimHitOutput.reset();
    fMMSSimTrackOutput.reset();
}

} // namespace MACE::SimMMS
