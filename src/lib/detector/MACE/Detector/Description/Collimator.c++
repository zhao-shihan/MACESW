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

#include "MACE/Detector/Description/Collimator.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

Collimator::Collimator() :
    DescriptionBase{"Collimator"},
    // Geometry
    fEnabled{true},
    fLength{500_mm},
    fRadius{49_mm},
    fThickness{0.2_mm},
    fPitch{1.15_mm},
    // Material
    fMaterialName{"G4_BRASS"} {}

auto Collimator::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fEnabled, "Enabled");
    ImportValue(node, fLength, "Length");
    ImportValue(node, fRadius, "Radius");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fPitch, "Pitch");
    // Material
    ImportValue(node, fMaterialName, "MaterialName");
}

auto Collimator::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fEnabled, "Enabled");
    ExportValue(node, fLength, "Length");
    ExportValue(node, fRadius, "Radius");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fPitch, "Pitch");
    // Material
    ExportValue(node, fMaterialName, "MaterialName");
}

} // namespace MACE::Detector::Description
