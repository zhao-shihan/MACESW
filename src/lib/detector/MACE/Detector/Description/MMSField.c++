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

#include "MACE/Detector/Description/MMSField.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::MagneticFluxDensity;

MMSField::MMSField() :
    DescriptionBase{"MMSField"},
    // Geometry
    fRadius{65_cm},
    fLength{220_cm},
    // Field
    fFastField{100_mT} {}

void MMSField::ImportAllValue(const YAML::Node& node) {
    // Geometry
    ImportValue(node, fRadius, "Radius");
    ImportValue(node, fLength, "Length");
    // Field
    ImportValue(node, fFastField, "FastField");
}

void MMSField::ExportAllValue(YAML::Node& node) const {
    // Geometry
    ExportValue(node, fRadius, "Radius");
    ExportValue(node, fLength, "Length");
    // Field
    ExportValue(node, fFastField, "FastField");
}

} // namespace MACE::Detector::Description
