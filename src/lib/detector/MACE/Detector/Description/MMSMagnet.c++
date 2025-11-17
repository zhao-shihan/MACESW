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

#include "MACE/Detector/Description/MMSMagnet.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

MMSMagnet::MMSMagnet() :
    DescriptionBase{"MMSMagnet"},
    // Geometry
    fInnerRadius{55_cm},
    fOuterRadius{60_cm},
    fLength{220_cm},
    // Material
    fMaterialName{"G4_Cu"} {}

void MMSMagnet::ImportAllValue(const YAML::Node& node) {
    // Geometry
    ImportValue(node, fInnerRadius, "InnerRadius");
    ImportValue(node, fOuterRadius, "OuterRadius");
    ImportValue(node, fLength, "Length");
    // Material
    ImportValue(node, fMaterialName, "MaterialName");
}

void MMSMagnet::ExportAllValue(YAML::Node& node) const {
    // Geometry
    ExportValue(node, fInnerRadius, "InnerRadius");
    ExportValue(node, fOuterRadius, "OuterRadius");
    ExportValue(node, fLength, "Length");
    // Material
    ExportValue(node, fMaterialName, "MaterialName");
}

} // namespace MACE::Detector::Description
