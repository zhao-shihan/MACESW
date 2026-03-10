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

#include "MACE/Data/SensorHit.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/Data/SimVertex.h++"
#include "MACE/SimECAL/Messenger/AnalysisMessenger.h++"

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
class ECALHit;
class ECALPMHit;
class MCPHit;
} // namespace MACE::inline Simulation::inline Hit

namespace MACE::SimECAL {

class Analysis final : public Mustard::Simulation::AnalysisBase<Analysis, "SimECAL"> {
public:
    Analysis();

    auto CoincidenceWithECAL(bool val) -> void { fCoincidenceWithECAL = val; }
    auto CoincidenceWithMCP(bool val) -> void { fCoincidenceWithMCP = val; }

    auto SubmitPrimaryVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>& data) -> void { fPrimaryVertex = &data; }
    auto SubmitDecayVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>& data) -> void { fDecayVertex = &data; }
    auto SubmitECALHC(const std::vector<gsl::owner<ECALHit*>>& hc) -> void { fECALHit = &hc; }
    auto SubmitECALPMHC(const std::vector<gsl::owner<ECALPMHit*>>& hc) -> void { fECALPMHit = &hc; }
    auto SubmitMCPHC(const std::vector<gsl::owner<MCPHit*>>& hc) -> void { fMCPHit = &hc; }

    auto RunBeginUserAction(int runID) -> void override;
    auto EventEndUserAction() -> void override;
    auto RunEndUserAction(int) -> void override;

private:
    bool fCoincidenceWithECAL;
    bool fCoincidenceWithMCP;

    std::filesystem::path fLastUsedFullFilePath;

    std::optional<Mustard::Data::Output<Data::SimPrimaryVertex>> fPrimaryVertexOutput;
    std::optional<Mustard::Data::Output<Data::SimDecayVertex>> fDecayVertexOutput;
    std::optional<Mustard::Data::Output<Data::ECALSimHit>> fECALSimHitOutput;
    std::optional<Mustard::Data::Output<Data::ECALPMHit>> fECALPMHitOutput;
    std::optional<Mustard::Data::Output<Data::MCPSimHit>> fMCPSimHitOutput;

    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>* fPrimaryVertex;
    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>* fDecayVertex;
    const std::vector<gsl::owner<ECALHit*>>* fECALHit;
    const std::vector<gsl::owner<ECALPMHit*>>* fECALPMHit;
    const std::vector<gsl::owner<MCPHit*>>* fMCPHit;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::SimECAL
