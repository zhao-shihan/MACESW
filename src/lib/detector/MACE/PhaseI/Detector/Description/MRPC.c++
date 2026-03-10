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

#include "MACE/PhaseI/Detector/Description/MRPC.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Time;

MRPC::MRPC() :
    DescriptionBase{"MRPC"},
    fHoneyCombThickness{4_mm},
    fPCBThickness{1.5_mm},
    fMylarThickness{0.35_mm},
    fAnodeThickness{0.13_mm},
    fOuterGlassThickness{0.7_mm},
    fInnerGlassThickness{0.54_mm},
    fNGaps{6},
    fGasGapThickness{0.22_mm},
    fGasTankThickness{1.5_mm},
    fNMRPCs{8},
    fCentralRadius{6.35_cm},
    fCentralMRPCLength{30_cm},
    fCentralMRPCWidth{10_cm},
    fCornerRadius{11.6_cm},
    fCornerMRPCLength{30_cm},
    fCornerMRPCWidth{10_cm},
    fTimeResolutionFWHM{100_ps},
    fSpacialResolutionFWHM{1_mm} {}

auto MRPC::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fHoneyCombThickness, "HoneyCombThickness");
    ImportValue(node, fPCBThickness, "PCBThickness");
    ImportValue(node, fMylarThickness, "MylarThickness");
    ImportValue(node, fAnodeThickness, "AnodeThickness");
    ImportValue(node, fOuterGlassThickness, "OuterGlassThickness");
    ImportValue(node, fInnerGlassThickness, "InnerGlassThickness");
    ImportValue(node, fNGaps, "NGaps");
    ImportValue(node, fGasGapThickness, "GasGapThickness");
    ImportValue(node, fGasTankThickness, "GasTankThickness");
    ImportValue(node, fNMRPCs, "NMRPCs");
    ImportValue(node, fCentralRadius, "CentralRadius");
    ImportValue(node, fCentralMRPCLength, "CentralMRPCLength");
    ImportValue(node, fCentralMRPCWidth, "CentralMRPCWidth");
    ImportValue(node, fCornerRadius, "CornerRadius");
    ImportValue(node, fCornerMRPCLength, "CornerMRPCLength");
    ImportValue(node, fCornerMRPCWidth, "CornerMRPCWidth");
    ImportValue(node, fTimeResolutionFWHM, "TimeResolutionFWHM");
    ImportValue(node, fSpacialResolutionFWHM, "SpacialResolutionFWHM");
}

auto MRPC::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fHoneyCombThickness, "HoneyCombThickness");
    ExportValue(node, fPCBThickness, "PCBThickness");
    ExportValue(node, fMylarThickness, "MylarThickness");
    ExportValue(node, fAnodeThickness, "AnodeThickness");
    ExportValue(node, fOuterGlassThickness, "OuterGlassThickness");
    ExportValue(node, fInnerGlassThickness, "InnerGlassThickness");
    ExportValue(node, fNGaps, "NGaps");
    ExportValue(node, fGasGapThickness, "GasGapThickness");
    ExportValue(node, fGasTankThickness, "GasTankThickness");
    ExportValue(node, fNMRPCs, "NMRPCs");
    ExportValue(node, fCentralRadius, "CentralRadius");
    ExportValue(node, fCentralMRPCLength, "CentralMRPCLength");
    ExportValue(node, fCentralMRPCWidth, "CentralMRPCWidth");
    ExportValue(node, fCornerRadius, "CornerRadius");
    ExportValue(node, fCornerMRPCLength, "CornerMRPCLength");
    ExportValue(node, fCornerMRPCWidth, "CornerMRPCWidth");
    ExportValue(node, fTimeResolutionFWHM, "TimeResolutionFWHM");
    ExportValue(node, fSpacialResolutionFWHM, "SpacialResolutionFWHM");
}

} // namespace MACE::PhaseI::Detector::Description
