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

#include "MACE/Detector/Description/BeamDegrader.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "muc/utility"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

BeamDegrader::BeamDegrader() :
    DescriptionBase{"BeamDegrader"},
    fEnabled{true},
    // Geometry
    fWidth{10_cm},
    fHeight{5_cm},
    fThickness{270_um},
    fDistanceToTarget{5_mm},
    // Material
    fMaterialName{"G4_MYLAR"} {}

auto BeamDegrader::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fEnabled, "Enabled");
    // Geometry
    ImportValue(node, fWidth, "Width");
    ImportValue(node, fHeight, "Height");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fDistanceToTarget, "DistanceToTarget");
    // Material
    ImportValue(node, fMaterialName, "MaterialName");
}

auto BeamDegrader::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fEnabled, "Enabled");
    // Geometry
    ExportValue(node, fWidth, "Width");
    ExportValue(node, fHeight, "Height");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fDistanceToTarget, "DistanceToTarget");
    // Material
    ExportValue(node, fMaterialName, "MaterialName");
}

} // namespace MACE::Detector::Description
