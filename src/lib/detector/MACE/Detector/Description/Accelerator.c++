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

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::ElectricPotential;
using namespace Mustard::LiteralUnit::Length;

Accelerator::Accelerator() :
    DescriptionBase{"Accelerator"},
    // Geometry
    fUpstreamLength{110_mm},
    fDownstreamLength{270_mm},
    fElectrodePitch{20_mm},
    fElectrodeInnerRadius{50_mm},
    fElectrodeOuterRadius{60_mm},
    fElectrodeThickness{0.5_mm},
    fFieldRadius{70_mm},
    fDecelerateFieldLength{165_mm},
    fAccelerateFieldLength{360_mm},
    // Material
    fElectrodeMaterialName{"G4_Be"},
    // Field
    fMaxPotential{586.7_V},
    fMaxPotentialPosition{-65_mm} {}

auto Accelerator::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fUpstreamLength, "UpstreamLength");
    ImportValue(node, fDownstreamLength, "DownstreamLength");
    ImportValue(node, fElectrodePitch, "ElectrodePitch");
    ImportValue(node, fElectrodeInnerRadius, "ElectrodeInnerRadius");
    ImportValue(node, fElectrodeOuterRadius, "ElectrodeOuterRadius");
    ImportValue(node, fElectrodeThickness, "ElectrodeThickness");
    ImportValue(node, fFieldRadius, "FieldRadius");
    ImportValue(node, fDecelerateFieldLength, "DecelerateFieldLength");
    ImportValue(node, fAccelerateFieldLength, "AccelerateFieldLength");
    // Material
    ImportValue(node, fElectrodeMaterialName, "ElectrodeMaterialName");
    // Field
    ImportValue(node, fMaxPotential, "MaxPotential");
    ImportValue(node, fMaxPotentialPosition, "MaxPotentialPosition");
}

auto Accelerator::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fUpstreamLength, "UpstreamLength");
    ExportValue(node, fDownstreamLength, "DownstreamLength");
    ExportValue(node, fElectrodePitch, "ElectrodePitch");
    ExportValue(node, fElectrodeInnerRadius, "ElectrodeInnerRadius");
    ExportValue(node, fElectrodeOuterRadius, "ElectrodeOuterRadius");
    ExportValue(node, fElectrodeThickness, "ElectrodeThickness");
    ExportValue(node, fFieldRadius, "FieldRadius");
    ExportValue(node, fDecelerateFieldLength, "DecelerateFieldLength");
    ExportValue(node, fAccelerateFieldLength, "AccelerateFieldLength");
    // Material
    ExportValue(node, fElectrodeMaterialName, "ElectrodeMaterialName");
    // Field
    ExportValue(node, fMaxPotential, "MaxPotential");
    ExportValue(node, fMaxPotentialPosition, "MaxPotentialPosition");
}

} // namespace MACE::Detector::Description
