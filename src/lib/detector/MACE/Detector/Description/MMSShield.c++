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

#include "MACE/Detector/Description/MMSShield.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Angle;

MMSShield::MMSShield() :
    DescriptionBase{"MMSShield"},
    // Geometry
    fInnerRadius{65_cm},
    fInnerLength{230_cm},
    fThickness{5_cm},
    fWindowRadius{250_mm},
    fBeamSlantAngle{0_deg},
    // Material
    fMaterialName{"G4_Fe"} {}

auto MMSShield::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fInnerRadius, "InnerRadius");
    ImportValue(node, fInnerLength, "InnerLength");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fWindowRadius, "WindowRadius");
    ImportValue(node, fBeamSlantAngle, "BeamSlantAngle");
    // Material
    ImportValue(node, fMaterialName, "MaterialName");
}

auto MMSShield::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fInnerRadius, "InnerRadius");
    ExportValue(node, fInnerLength, "InnerLength");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fWindowRadius, "WindowRadius");
    ExportValue(node, fBeamSlantAngle, "BeamSlantAngle");
    // Material
    ExportValue(node, fMaterialName, "MaterialName");
}

} // namespace MACE::Detector::Description
