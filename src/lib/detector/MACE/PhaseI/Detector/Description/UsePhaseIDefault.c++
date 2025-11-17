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

#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Detector/Description/Target.h++"
#include "MACE/PhaseI/Detector/Description/UsePhaseIDefault.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

auto UsePhaseIDefault() -> void {
    using namespace Mustard::LiteralUnit::Length;
    namespace MACE = MACE::Detector::Description;
    { // set accelerator up/downstream length to a random equal value
        auto& accelerator{MACE::Accelerator::Instance()};
        accelerator.MaxPotentialPosition(0);
        accelerator.AccelerateFieldLength(2);
        accelerator.DecelerateFieldLength(2);
    }
    { // Use cylinder target
        auto& target{MACE::Target::Instance()};
        target.ShapeType(MACE::Target::TargetShapeType::Cylinder);
    }
    { // bigger windows for ECAL
        auto& ecal{MACE::ECAL::Instance()};
        ecal.UpstreamWindowRadius(104_mm);
        ecal.DownstreamWindowRadius(104_mm);
        ecal.InnerRadius(260_mm);
    }
}

} // namespace MACE::PhaseI::Detector::Description
