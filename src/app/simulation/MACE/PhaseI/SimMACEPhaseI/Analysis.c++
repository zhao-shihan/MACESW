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

#include "MACE/PhaseI/SimMACEPhaseI/Action/PrimaryGeneratorAction.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Action/TrackingAction.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Analysis.h++"
#include "MACE/PhaseI/Simulation/Hit/MRPCHit.h++"
#include "MACE/PhaseI/Simulation/Hit/SciFiSiPMHit.h++"
#include "MACE/PhaseI/Simulation/Hit/SciFiSimHit.h++"
#include "MACE/Simulation/Hit/ECALHit.h++"
#include "MACE/Simulation/Hit/ECALPMHit.h++"
#include "MACE/Simulation/Hit/TTCHit.h++"
#include "MACE/Simulation/Hit/TTCSiPMHit.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "fmt/format.h"

#include <stdexcept>

namespace MACE::PhaseI::SimMACEPhaseI {

Analysis::Analysis() :
    AnalysisBase{this},
    fCoincidenceWithMRPC{true},
    fCoincidenceWithECAL{true},
    fSaveTTCHitData{true},
    fSaveTTCSiPMHitData{true},
    fPrimaryVertexOutput{},
    fDecayVertexOutput{},
    fMRPCSimHitOutput{},
    fECALSimHitOutput{},
    fECALPMHitOutput{},
    fSciFiSimHitOutput{},
    fSciFiSiPMHitOutput{},
    fTTCSimHitOutput{},
    fTTCSiPMHitOutput{},
    fPrimaryVertex{},
    fDecayVertex{},
    fMRPCHit{},
    fECALHit{},
    fECALPMHit{},
    fSciFiSimHit{},
    fSciFiSiPMHit{},
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
    fMRPCSimHitOutput.emplace(fmt::format("G4Run{}/MRPCSimHit", runID));
    fECALSimHitOutput.emplace(fmt::format("G4Run{}/ECALSimHit", runID));
    fECALPMHitOutput.emplace(fmt::format("G4Run{}/ECALPMHit", runID));
    fSciFiSimHitOutput.emplace(fmt::format("G4Run{}/SciFiSimHit", runID));
    fSciFiSiPMHitOutput.emplace(fmt::format("G4Run{}/SciFiSiPMHit", runID));
}

auto Analysis::EventEndUserAction() -> void {
    const auto mrpcPassed{not fCoincidenceWithMRPC or fMRPCHit == nullptr or not fMRPCHit->empty()};
    const auto ecalPassed{not fCoincidenceWithECAL or fECALHit == nullptr or not fECALHit->empty()};
    if (mrpcPassed and ecalPassed) {
        if (fPrimaryVertex and fPrimaryVertexOutput) {
            fPrimaryVertexOutput->Fill(*fPrimaryVertex);
        }
        if (fDecayVertex and fDecayVertexOutput) {
            fDecayVertexOutput->Fill(*fDecayVertex);
        }
        if (fMRPCHit) {
            fMRPCSimHitOutput->Fill(*fMRPCHit);
        }
        if (fECALHit) {
            fECALSimHitOutput->Fill(*fECALHit);
        }
        if (fECALPMHit) {
            fECALPMHitOutput->Fill(*fECALPMHit);
        }
        if (fSciFiSimHit) {
            fSciFiSimHitOutput->Fill(*fSciFiSimHit);
        }
        if (fSciFiSiPMHit) {
            fSciFiSiPMHitOutput->Fill(*fSciFiSiPMHit);
        }
        if (fTTCSimHitOutput) {
            fTTCSimHitOutput->Fill(*fTTCHit);
        }
        if (fTTCSiPMHitOutput) {
            fTTCSiPMHitOutput->Fill(*fTTCSiPMHit);
        }
    }
    fPrimaryVertex = {};
    fDecayVertex = {};
    fMRPCHit = {};
    fECALHit = {};
    fECALPMHit = {};
    fSciFiSimHit = {};
    fSciFiSiPMHit = {};
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
    fMRPCSimHitOutput->Write();
    fECALSimHitOutput->Write();
    fECALPMHitOutput->Write();
    fSciFiSimHitOutput->Write();
    fSciFiSiPMHitOutput->Write();
    // reset output
    fPrimaryVertexOutput.reset();
    fDecayVertexOutput.reset();
    fMRPCSimHitOutput.reset();
    fECALSimHitOutput.reset();
    fECALPMHitOutput.reset();
    fSciFiSimHitOutput.reset();
    fSciFiSiPMHitOutput.reset();
    fTTCSimHitOutput.reset();
    fTTCSiPMHitOutput.reset();
}

} // namespace MACE::PhaseI::SimMACEPhaseI
