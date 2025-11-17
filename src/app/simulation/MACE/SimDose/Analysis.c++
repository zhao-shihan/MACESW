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

#include "MACE/SimDose/Action/DetectorConstruction.h++"
#include "MACE/SimDose/Analysis.h++"

#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/IO/PrettyLog.h++"

#include "TDirectory.h"
#include "TH3F.h"

#include "G4ParticleDefinition.hh"
#include "G4StepPoint.hh"
#include "G4StepStatus.hh"
#include "G4SystemOfUnits.hh"
#include "G4Transportation.hh"

#include "muc/math"

#include "fmt/core.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace MACE::SimDose {

Analysis::Analysis() :
    AnalysisBase{this},
    fMapModel{},
    fMap{},
    fMessengerRegister{this} {}

auto Analysis::AddMap(std::string name) -> void {
    fMapModel.emplace_back().name = std::move(name);
}

auto Analysis::MapNBinX(int val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().nBinX = val;
}

auto Analysis::MapXMin(double val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().xMin = val;
}

auto Analysis::MapXMax(double val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().xMax = val;
}

auto Analysis::MapNBinY(int val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().nBinY = val;
}

auto Analysis::MapYMin(double val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().yMin = val;
}

auto Analysis::MapYMax(double val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().yMax = val;
}

auto Analysis::MapNBinZ(int val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().nBinZ = val;
}

auto Analysis::MapZMin(double val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().zMin = val;
}

auto Analysis::MapZMax(double val) -> void {
    if (not CheckMapAdded()) {
        return;
    }
    fMapModel.back().zMax = val;
}

auto Analysis::FillMap(const G4Step& step) const -> void {
    const auto& post{*step.GetPostStepPoint()};
    const auto status{post.GetStepStatus()};
    if (status == fGeomBoundary or status == fWorldBoundary or status == fUndefined) {
        return;
    }

    const auto eDep{step.GetTotalEnergyDeposit()};
    if (eDep == 0) {
        return;
    }

    if (dynamic_cast<const G4Transportation*>(post.GetProcessDefinedStep())) {
        return;
    }

    const auto& pre{*step.GetPreStepPoint()};
    const auto x0{pre.GetPosition()};
    const auto x{post.GetPosition()};
    for (auto&& [eDepMap, doseMap, deltaV, minDelta] : std::as_const(fMap)) {
        const auto fill{
            [&](G4ThreeVector x, double eDep, double dose) {
                eDepMap->Fill(x.x(), x.y(), x.z(), eDep / joule);
                doseMap->Fill(x.x(), x.y(), x.z(), dose / gray);
            }};
        if (status != fAlongStepDoItProc) {
            const auto deltaM{post.GetMaterial()->GetDensity() * deltaV};
            const auto dose{eDep / deltaM};
            fill(x, eDep, dose);
        } else {
            const auto deltaM{pre.GetMaterial()->GetDensity() * deltaV};
            const auto dose{eDep / deltaM};

            const auto segment{x - x0};
            const auto nFill{muc::lltrunc(segment.mag() / minDelta) + 1};

            const auto eDepFill{eDep / nFill};
            const auto doseFill{dose / nFill};
            const auto deltaFill{segment / nFill};

            auto xFill{x0 + deltaFill / 2};
            for (int i{}; i < nFill; ++i) {
                fill(xFill, eDepFill, doseFill);
                xFill += deltaFill;
            }
        }
    }
}

auto Analysis::RunBeginUserAction(int) -> void {
    fMap.reserve(fMapModel.size());
    for (auto&& [name, nBinX, xMin, xMax, nBinY, yMin, yMax, nBinZ, zMin, zMax] : std::as_const(fMapModel)) {
        if (nBinX == 0) {
            Mustard::Throw<std::runtime_error>("Map nBinX == 0");
        }
        if (nBinY == 0) {
            Mustard::Throw<std::runtime_error>("Map nBinY == 0");
        }
        if (nBinZ == 0) {
            Mustard::Throw<std::runtime_error>("Map nBinZ == 0");
        }
        if (xMin >= xMax) {
            Mustard::Throw<std::runtime_error>("Map xMin >= xMax");
        }
        if (yMin >= yMax) {
            Mustard::Throw<std::runtime_error>("Map yMin >= yMax");
        }
        if (zMin >= zMax) {
            Mustard::Throw<std::runtime_error>("Map zMin >= zMax");
        }

        auto& map{fMap.emplace_back()};

        map.eDepMap = new TH3F{fmt::format("{}EdepMap", name).c_str(),
                               "Energy deposition (J)",
                               nBinX, xMin, xMax,
                               nBinY, yMin, yMax,
                               nBinZ, zMin, zMax};
        map.doseMap = new TH3F{fmt::format("{}DoseMap", name).c_str(),
                               "Absorbed dose (Gy)",
                               nBinX, xMin, xMax,
                               nBinY, yMin, yMax,
                               nBinZ, zMin, zMax};

        const auto dx{(xMax - xMin) / nBinX};
        const auto dy{(yMax - yMin) / nBinY};
        const auto dz{(zMax - zMin) / nBinZ};
        map.deltaV = dx * dy * dz;
        map.minDelta = std::min({dx, dy, dz});

        auto& detectorConstruction{DetectorConstruction::Instance()};
        if (map.minDelta < detectorConstruction.VacuumStepLimit()) {
            detectorConstruction.VacuumStepLimit(map.minDelta);
        }
    }
}

auto Analysis::RunEndUserAction(int runID) -> void {
    gDirectory->mkdir(fmt::format("G4Run{}", runID).c_str(), "", true)->cd();
    for (auto&& [eDepMap, doseMap, _1, _2] : std::as_const(fMap)) {
        eDepMap->Write();
        doseMap->Write();
    }
    fMap.clear();
    DetectorConstruction::Instance().VacuumStepLimit(std::numeric_limits<double>::max());
}

auto Analysis::CheckMapAdded() -> bool {
    if (fMapModel.empty()) {
        Mustard::PrintWarning("No map was added");
        return false;
    }
    return true;
}

} // namespace MACE::SimDose
