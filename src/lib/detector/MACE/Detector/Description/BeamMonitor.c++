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

#include "MACE/Detector/Description/BeamMonitor.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

BeamMonitor::BeamMonitor() :
    DescriptionBase{"BeamMonitor"},
    fEnabled{false},
    fWidth{4_cm},
    fThickness{500_um},
    fDistanceToTarget{5_mm} {}

auto BeamMonitor::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fEnabled, "Enabled");
    ImportValue(node, fWidth, "Width");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fDistanceToTarget, "DistanceToTarget");
}

auto BeamMonitor::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fEnabled, "Enabled");
    ExportValue(node, fWidth, "Width");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fDistanceToTarget, "DistanceToTarget");
}

} // namespace MACE::Detector::Description
