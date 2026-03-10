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

#include "MACE/Detector/Description/MCP.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::LiteralUnit::Length;

MCP::MCP() :
    DescriptionBase{"MCP"},
    // Geometry
    fDiameter{10_cm},
    fThickness{1_mm},
    fAnodeDistance{5_mm},
    fAnodeThickness{1_mm},
    // Detection
    fTimeResolutionFWHM{1_ns},
    fEfficiencyEnergy{0.04843584244266253_keV, 0.05718602834634275_keV, 0.06751698067190572_keV, 0.07971427306407833_keV,
                      0.0941150695261843_keV, 0.11111744448573568_keV, 0.13119138657816534_keV, 0.15489179032110528_keV,
                      0.18287379480193877_keV, 0.21591089337873542_keV, 0.25491631499249356_keV, 0.30096826812422506_keV,
                      0.3553397452036867_keV, 0.41953371133897877_keV, 0.49532465007247223_keV, 0.5848076145928109_keV,
                      0.6904561403025169_keV, 0.8151906195910004_keV, 0.962459028864598_keV, 1.1363322393328625_keV,
                      1.3416165461822431_keV, 1.5839865267280526_keV, 1.8701419000799793_keV, 2.207992724318932_keV,
                      2.606878050503463_keV, 3.077824077655397_keV, 3.6338489447812012_keV, 4.29032258515131_keV,
                      5.065391590119529_keV, 5.980480826792828_keV, 7.0608856755322735_keV, 8.336471258227133_keV,
                      9.842497985779692_keV, 11.408137229828988_keV},
    fEfficiencyValue{0.45226243044550063, 0.4904065042352933, 0.5229878483204101, 0.5522738821562811,
                     0.5805578523987902, 0.6047734038429532, 0.6290462262988287, 0.6508364661376876,
                     0.6698238927972839, 0.6862422969080331, 0.7014688266270745, 0.7094767174773542,
                     0.7105513206762596, 0.7105513206762596, 0.7062626491181344, 0.695653876102249,
                     0.6800387566590625, 0.660761770339832, 0.6386392277332411, 0.6144610592336859,
                     0.588965535363074, 0.5628216467042225, 0.5354017597085551, 0.5070104224441712,
                     0.4786734877754322, 0.4512368489519376, 0.42344580308379676, 0.3961653626140163,
                     0.3703620841055646, 0.3441496442080233, 0.3200344880821422, 0.29581284422432463,
                     0.27301088914452576, 0.253825661135394} {}

auto MCP::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fDiameter, "Diameter");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fAnodeDistance, "AnodeDistance");
    ImportValue(node, fAnodeThickness, "AnodeThickness");
    // Detection
    ImportValue(node, fTimeResolutionFWHM, "TimeResolutionFWHM");
    ImportValue(node, fEfficiencyEnergy, "EfficiencyEnergy");
    ImportValue(node, fEfficiencyValue, "EfficiencyValue");
}

auto MCP::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fDiameter, "Diameter");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fAnodeDistance, "AnodeDistance");
    ExportValue(node, fAnodeThickness, "AnodeThickness");
    // Detection
    ExportValue(node, fTimeResolutionFWHM, "TimeResolutionFWHM");
    ExportValue(node, fEfficiencyEnergy, "EfficiencyEnergy");
    ExportValue(node, fEfficiencyValue, "EfficiencyValue");
}

} // namespace MACE::Detector::Description
