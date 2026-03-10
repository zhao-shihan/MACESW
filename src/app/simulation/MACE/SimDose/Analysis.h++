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

#include "MACE/SimDose/Messenger/AnalysisMessenger.h++"

#include "Mustard/Simulation/AnalysisBase.h++"

#include "G4Step.hh"
#include "G4ThreeVector.hh"

#include <vector>

class TH3F;

namespace MACE::SimDose {

class Analysis final : public Mustard::Simulation::AnalysisBase<Analysis, "SimDose"> {
public:
    Analysis();

    auto AddMap(std::string name) -> void;
    auto MapNBinX(int val) -> void;
    auto MapXMin(double val) -> void;
    auto MapXMax(double val) -> void;
    auto MapNBinY(int val) -> void;
    auto MapYMin(double val) -> void;
    auto MapYMax(double val) -> void;
    auto MapNBinZ(int val) -> void;
    auto MapZMin(double val) -> void;
    auto MapZMax(double val) -> void;

    auto FillMap(const G4Step& step) const -> void;

private:
    auto RunBeginUserAction(int) -> void override;
    auto EventEndUserAction() -> void override {}
    auto RunEndUserAction(int runID) -> void override;

    auto CheckMapAdded() -> bool;

private:
    struct MapModel {
        std::string name{};
        int nBinX{};
        double xMin{};
        double xMax{};
        int nBinY{};
        double yMin{};
        double yMax{};
        int nBinZ{};
        double zMin{};
        double zMax{};
    };

    struct MapData {
        TH3F* eDepMap{};
        TH3F* doseMap{};
        double deltaV{};
        double minDelta{};
    };

private:
    std::vector<MapModel> fMapModel;
    std::vector<MapData> fMap;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::SimDose
