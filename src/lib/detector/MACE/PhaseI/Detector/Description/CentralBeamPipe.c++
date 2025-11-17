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

#include "MACE/PhaseI/Detector/Description/CentralBeamPipe.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Pressure;

CentralBeamPipe::CentralBeamPipe() :
    DescriptionBase{"CentralBeamPipe"},
    // Geometry
    fLength{1000_mm},
    fInnerRadius{40_mm},
    fBerylliumLength{200_mm},
    fBerylliumThickness{500_um},
    fAluminiumThickness{2_mm},
    // Material
    fVacuumPressure{1e-4_Pa} {}

auto CentralBeamPipe::VacuumDensity() const -> double {
    using namespace Mustard::LiteralUnit::Density;
    return fVacuumPressure / 1_atm * 1.177_kg_m3;
}

auto CentralBeamPipe::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fLength, "Length");
    ImportValue(node, fInnerRadius, "InnerRadius");
    ImportValue(node, fBerylliumLength, "BerylliumLength");
    ImportValue(node, fBerylliumThickness, "BerylliumThickness");
    ImportValue(node, fAluminiumThickness, "AluminiumThickness");
    // Material
    ImportValue(node, fVacuumPressure, "VacuumPressure");
}

auto CentralBeamPipe::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fLength, "Length");
    ExportValue(node, fInnerRadius, "InnerRadius");
    ExportValue(node, fBerylliumLength, "BerylliumLength");
    ExportValue(node, fBerylliumThickness, "BerylliumThickness");
    ExportValue(node, fAluminiumThickness, "AluminiumThickness");
    // Material
    ExportValue(node, fVacuumPressure, "VacuumPressure");
}

} // namespace MACE::PhaseI::Detector::Description
