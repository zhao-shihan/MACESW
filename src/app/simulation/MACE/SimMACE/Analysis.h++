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

#pragma once

#include "MACE/Data/MMSTrack.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/Data/SimVertex.h++"
#include "MACE/SimMACE/Messenger/AnalysisMessenger.h++"
#include "MACE/Simulation/Analysis/MMSTruthTracker.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Simulation/AnalysisBase.h++"

#include "muc/ptrvec"

#include "gsl/gsl"

#include <filesystem>
#include <utility>
#include <vector>

class TFile;

namespace MACE::inline Simulation::inline Hit {
class CDCHit;
class ECALHit;
class MCPHit;
class TTCHit;
} // namespace MACE::inline Simulation::inline Hit

namespace MACE::SimMACE {

class Analysis final : public Mustard::Simulation::AnalysisBase<Analysis, "SimMACE"> {
public:
    Analysis();

    auto CoincidenceWithMMS(bool val) -> void { fCoincidenceWithMMS = val; }
    auto CoincidenceWithMCP(bool val) -> void { fCoincidenceWithMCP = val; }
    auto CoincidenceWithECAL(bool val) -> void { fCoincidenceWithECAL = val; }
    auto SaveCDCHitData(bool val) -> void { fSaveCDCHitData = val; }
    auto SaveTTCHitData(bool val) -> void { fSaveTTCHitData = val; }

    auto SubmitPrimaryVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>& data) -> void { fPrimaryVertex = &data; }
    auto SubmitDecayVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>& data) -> void { fDecayVertex = &data; }
    auto SubmitTTCHC(const std::vector<gsl::owner<TTCHit*>>& hc) -> void { fTTCHit = &hc; }
    auto SubmitCDCHC(const std::vector<gsl::owner<CDCHit*>>& hc) -> void { fCDCHit = &hc; }
    auto SubmitMCPHC(const std::vector<gsl::owner<MCPHit*>>& hc) -> void { fMCPHit = &hc; }
    auto SubmitECALHC(const std::vector<gsl::owner<ECALHit*>>& hc) -> void { fECALHit = &hc; }

private:
    auto RunBeginUserAction(int runID) -> void override;
    auto EventEndUserAction() -> void override;
    auto RunEndUserAction(int) -> void override;

private:
    bool fCoincidenceWithMMS;
    bool fCoincidenceWithMCP;
    bool fCoincidenceWithECAL;
    bool fSaveCDCHitData;
    bool fSaveTTCHitData;

    std::optional<Mustard::Data::Output<Data::SimPrimaryVertex>> fPrimaryVertexOutput;
    std::optional<Mustard::Data::Output<Data::SimDecayVertex>> fDecayVertexOutput;
    std::optional<Mustard::Data::Output<Data::TTCSimHit>> fTTCSimHitOutput;
    std::optional<Mustard::Data::Output<Data::CDCSimHit>> fCDCSimHitOutput;
    std::optional<Mustard::Data::Output<Data::MMSSimTrack>> fMMSSimTrackOutput;
    std::optional<Mustard::Data::Output<Data::MCPSimHit>> fMCPSimHitOutput;
    std::optional<Mustard::Data::Output<Data::ECALSimHit>> fECALSimHitOutput;

    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>* fPrimaryVertex;
    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>* fDecayVertex;
    const std::vector<gsl::owner<TTCHit*>>* fTTCHit;
    const std::vector<gsl::owner<CDCHit*>>* fCDCHit;
    const std::vector<gsl::owner<MCPHit*>>* fMCPHit;
    const std::vector<gsl::owner<ECALHit*>>* fECALHit;

    Simulation::Analysis::MMSTruthTracker fMMSTruthTracker;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::SimMACE
