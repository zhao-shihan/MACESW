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

#include "MACE/PhaseI/Detector/Description/TTC.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "fmt/core.h"

#include <algorithm>
#include <cmath>
#include <deque>

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;

TTC::TTC() : // clang-format off
    DescriptionWithCacheBase{"TTC"}, // clang-format on
    // Geometry
    fLength{this, 5.2_cm},
    fWidth{this, 5_cm},
    fThickness{this, 0.5_cm},
    fRadius{this, {8.5_cm, 8.5_cm, 8.5_cm, 8.5_cm, 8.5_cm, 8.5_cm, 8.5_cm, 8.5_cm, 8.5_cm}},
    fSlantAngle{this, 17_deg},
    fNAlongPhi{this, {12, 12, 12, 12, 12, 12, 12, 12, 12}},
    fZPosition{this, {-20.6_cm, -15.45_cm, -10.3_cm, -5.15_cm, 0_cm, 5.15_cm, 10.3_cm, 15.45_cm, 20.6_cm}},
    fBarrelLength{this, 30_cm},

    fPCBLength{this, 3_cm},
    fPCBWidth{this, 1_cm},
    fPCBThickness{this, 0.1_cm},

    fWindowLength{this, 0.34_cm},
    fWindowWidth{this, 0.34_cm},
    fWindowThickness{this, 0.05_cm},

    fLightCouplerLength{this, 0.34_cm},
    fLightCouplerWidth{this, 0.34_cm},
    fLightCouplerThickness{this, 0.01_cm},

    fSiliconeLength{this, 0.3_cm},
    fSiliconeWidth{this, 0.3_cm},
    fSiliconeThickness{this, 0.03_cm},
    fNSiPM{this, 2},

    // Material
    // Scintillator
    fDensity{this, 1.05_g_cm3},
    fRIndexEnergyBin{this, {3.524_eV, 2.504_eV}},
    fRIndex{this, {1.58, 1.58}},
    fAbsLengthEnergyBin{this, {3.524_eV, 2.504_eV}},
    fAbsLength{this, {250_cm, 250_cm}},
    fScintillationComponent1EnergyBin{this, {}},
    fScintillationComponent1{this, {}},
    fScintillationYield{this, 7600.},
    fScintillationRiseTimeConstant1{this, 1_ns},
    fScintillationDecayTimeConstant1{this, 2.4_ns},
    fResolutionScale{this, 1.}, // HaoTang

    // LightCoupler
    fLightCouplerDensity{this, 1.06_g_cm3},
    fLightCouplerCarbonElement{this, 2},
    fLightCouplerHydrogenElement{this, 6},
    fLightCouplerOxygenElement{this, 1},
    fLightCouplerSiliconElement{this, 1},
    fLightCouplerRIndex{this, {1.46, 1.46}},
    fLightCouplerAbsLength{this, {100_cm, 100_cm}},
    // Window
    fWindowDensity{this, 1.18_g_cm3},
    fWindowCarbonElement{this, 0.7362},
    fWindowHydrogenElement{this, 0.0675},
    fWindowOxygenElement{this, 0.1963},
    fWindowRIndex{this, {1.55, 1.55}},

    fSiPMEnergyBin{this, {}},
    fSiPMEfficiency{this, {}}, // S13360-3050VE

    // Optical Surface
    fReflectorReflectivity{this, {0.985, 0.985}},
    fCouplerTransmittance{this, {1, 1}},
    fAirPaintReflectivity{this, {0, 0}},
    fCathodeSurface{this, {0., 0.}},

    // Option
    fUseOptics{this, true} {

    // HaoTang
    fScintillationComponent1EnergyBin = {2.364652_eV, 2.383758_eV, 2.405135_eV, 2.426898_eV, 2.449058_eV, 2.471627_eV, 2.494616_eV, 2.518037_eV, 2.541901_eV, 2.566222_eV, 2.591013_eV, 2.616288_eV, 2.640878_eV, 2.662326_eV, 2.680468_eV, 2.696392_eV, 2.710014_eV, 2.722518_eV, 2.735138_eV, 2.749156_eV, 2.765909_eV, 2.789445_eV, 2.818761_eV, 2.8487_eV, 2.872274_eV, 2.886324_eV, 2.897663_eV, 2.907659_eV, 2.916281_eV, 2.924242_eV, 2.935627_eV, 2.947291_eV, 2.9558_eV, 2.966124_eV, 2.975098_eV, 2.982617_eV, 2.990174_eV, 2.99777_eV, 3.003875_eV, 3.010004_eV, 3.016159_eV, 3.022338_eV, 3.028543_eV, 3.034774_eV, 3.04103_eV, 3.047312_eV, 3.05362_eV, 3.059955_eV, 3.066315_eV, 3.072702_eV, 3.079116_eV};
    fScintillationComponent1 = {0.434403, 1.582292, 3.084886, 4.612752, 6.271417, 8.051304, 10.12726, 12.376528, 14.988649, 17.939165, 21.472481, 25.741068, 30.767769, 35.904437, 41.161412, 46.389252, 51.290832, 55.99863, 60.371415, 65.183536, 69.599645, 72.422953, 72.822121, 73.213013, 76.559015, 80.622638, 85.027247, 89.716946, 94.083527, 98.012299, 100.0, 98.816769, 95.760048, 89.251097, 82.966493, 77.479502, 71.70861, 65.792378, 61.000588, 56.047106, 50.934253, 46.157431, 40.828953, 35.836262, 30.591153, 25.485149, 20.323703, 15.011678, 9.934578, 4.612821, 0.0};

    fSiPMEnergyBin = {1.381765901_eV, 1.389860942_eV, 1.413941862_eV, 1.438871953_eV, 1.464696939_eV, 1.491465885_eV, 1.515567315_eV, 1.530107599_eV, 1.548050535_eV, 1.573696281_eV, 1.603350003_eV, 1.622968195_eV, 1.639964259_eV, 1.665832212_eV, 1.694126388_eV, 1.721745595_eV, 1.73734153_eV, 1.760578484_eV, 1.784762827_eV, 1.81761524_eV, 1.84374762_eV, 1.876357093_eV, 1.909234563_eV, 1.935760263_eV, 1.961301125_eV, 1.982167418_eV, 2.004295799_eV, 2.027344665_eV, 2.050068622_eV, 2.076146155_eV, 2.101786541_eV, 2.129654281_eV, 2.166919814_eV, 2.19815906_eV, 2.22816776_eV, 2.25616833_eV, 2.284881603_eV, 2.314335139_eV, 2.347623692_eV, 2.385048049_eV, 2.414729138_eV, 2.443476628_eV, 2.485519666_eV, 2.530019157_eV, 2.571851852_eV, 2.609638812_eV, 2.647494553_eV, 2.700339349_eV, 2.746214689_eV, 2.783294612_eV, 2.832885744_eV, 2.878302168_eV, 2.958945749_eV, 3.004258321_eV, 3.050445335_eV, 3.086101104_eV, 3.125499496_eV, 3.181700781_eV, 3.207393207_eV, 3.250647849_eV, 3.279281103_eV, 3.286251959_eV, 3.317020733_eV, 3.345719986_eV, 3.383552152_eV, 3.427850284_eV, 3.449805753_eV, 3.474283919_eV, 3.503664297_eV, 3.540347333_eV, 3.55916676_eV, 3.573399416_eV, 3.592466775_eV, 3.602076965_eV, 3.616589044_eV, 3.636121361_eV, 3.650909616_eV, 3.665818652_eV, 3.678695081_eV, 3.699227364_eV, 3.703629464_eV, 3.726692679_eV, 3.740001084_eV, 3.773691692_eV, 3.805688536_eV, 3.846455793_eV};
    fSiPMEfficiency = {3.425656704, 3.648011349, 4.27165845, 4.946418576, 5.668359955, 6.429619045, 7.11080412, 7.497475278, 8.039291484, 8.821556901, 9.725285753, 10.25184232, 10.83322274, 11.69811431, 12.64197787, 13.57371735, 13.9856202, 14.80405969, 15.76358392, 16.75862246, 17.6931202, 18.89748135, 19.87240864, 20.95305937, 22.05877027, 23.28104155, 24.20768377, 25.00459784, 25.80394272, 26.89978015, 27.83080795, 28.83258008, 30.09983902, 31.17956649, 32.19281801, 33.10216044, 34.03745256, 34.97274467, 35.91391372, 36.90276753, 37.50595677, 38.13880536, 38.91429156, 39.30108444, 39.61979201, 39.86809295, 40.14914912, 40.16543674, 39.9601454, 39.64945991, 39.23407951, 38.63977117, 37.62528396, 36.96457658, 36.28223308, 35.54774213, 34.47404186, 33.17361068, 32.28915844, 30.68109949, 29.99828974, 29.44758984, 28.30684101, 27.12648793, 25.99030638, 24.78502023, 23.73010063, 22.79343306, 21.55889817, 20.49402454, 19.57991462, 18.52052398, 17.43525619, 16.35128295, 15.24864181, 14.15760743, 13.13714031, 12.12532309, 10.86827626, 9.937387296, 9.115710781, 8.311601404, 7.157647218, 5.786493629, 4.148860814, 2.819490889}; // S13360-3050VE
}

auto TTC::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fLength, "Length");
    ImportValue(node, fWidth, "Width");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fRadius, "DistanceToCDC");
    ImportValue(node, fSlantAngle, "SlantAngle");
    ImportValue(node, fNAlongPhi, "NAlongPhi");
    ImportValue(node, fZPosition, "ZPosition");
    ImportValue(node, fBarrelLength, "BarrelLength");
    ImportValue(node, fPCBLength, "PCBLength");
    ImportValue(node, fPCBWidth, "PCBWidth");
    ImportValue(node, fPCBThickness, "PCBThickness");
    ImportValue(node, fWindowLength, "WindowLength");
    ImportValue(node, fWindowWidth, "WindowWidth");
    ImportValue(node, fWindowThickness, "WindowThickness");
    ImportValue(node, fLightCouplerLength, "LightCouplerLength");
    ImportValue(node, fLightCouplerWidth, "LightCouplerWidth");
    ImportValue(node, fLightCouplerThickness, "LightCouplerThickness");
    ImportValue(node, fSiliconeLength, "SiliconeLength");
    ImportValue(node, fSiliconeWidth, "SiliconeWidth");
    ImportValue(node, fSiliconeThickness, "SiliconeThickness");
    ImportValue(node, fNSiPM, "NSiPM");
    // Material
    ImportValue(node, fDensity, "Density");
    ImportValue(node, fRIndexEnergyBin, "RIndexEnergyBin");
    ImportValue(node, fRIndex, "RIndex");
    ImportValue(node, fAbsLengthEnergyBin, "AbsLengthEnergyBin");
    ImportValue(node, fAbsLength, "AbsLength");
    ImportValue(node, fScintillationComponent1EnergyBin, "ScintillationComponent1EnergyBin");
    ImportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ImportValue(node, fScintillationYield, "ScintillationYield");
    ImportValue(node, fScintillationDecayTimeConstant1, "ScintillationDecayTimeConstant1");
    ImportValue(node, fScintillationRiseTimeConstant1, "ScintillationRiseTimeConstant1");
    ImportValue(node, fResolutionScale, "ResolutionScale");
    ImportValue(node, fLightCouplerDensity, "LightCouplerDensity");
    ImportValue(node, fLightCouplerCarbonElement, "LightCouplerCarbonElement");
    ImportValue(node, fLightCouplerHydrogenElement, "LightCouplerHydrogenElement");
    ImportValue(node, fLightCouplerOxygenElement, "LightCouplerOxygenElement");
    ImportValue(node, fLightCouplerSiliconElement, "LightCouplerSiliconElement");
    ImportValue(node, fLightCouplerRIndex, "LightCouplerRIndex");
    ImportValue(node, fLightCouplerAbsLength, "LightCouplerAbsLength");
    ImportValue(node, fWindowDensity, "WindowDensity");
    ImportValue(node, fWindowCarbonElement, "WindowCarbonElement");
    ImportValue(node, fWindowHydrogenElement, "WindowHydrogenElement");
    ImportValue(node, fWindowOxygenElement, "WindowOxygenElement");
    ImportValue(node, fWindowRIndex, "WindowRIndex");
    ImportValue(node, fSiPMEnergyBin, "SiPMEnergyBin");
    ImportValue(node, fSiPMEfficiency, "SiPMEfficiency");
    ImportValue(node, fReflectorReflectivity, "ReflectorReflectivity");
    ImportValue(node, fCouplerTransmittance, "CouplerTransmittance");
    ImportValue(node, fAirPaintReflectivity, "AirPaintReflectivity");
    ImportValue(node, fCathodeSurface, "CathodeSurface");
    // Option
    ImportValue(node, fUseOptics, "UseOptics");
}

auto TTC::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fLength, "Length");
    ExportValue(node, fWidth, "Width");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fRadius, "DistanceToCDC");
    ExportValue(node, fSlantAngle, "SlantAngle");
    ExportValue(node, fNAlongPhi, "NAlongPhi");
    ExportValue(node, fZPosition, "ZPosition");
    ExportValue(node, fBarrelLength, "BarrelLength");
    ExportValue(node, fPCBLength, "PCBLength");
    ExportValue(node, fPCBWidth, "PCBWidth");
    ExportValue(node, fPCBThickness, "PCBThickness");
    ExportValue(node, fWindowLength, "WindowLength");
    ExportValue(node, fWindowWidth, "WindowWidth");
    ExportValue(node, fWindowThickness, "WindowThickness");
    ExportValue(node, fLightCouplerLength, "LightCouplerLength");
    ExportValue(node, fLightCouplerWidth, "LightCouplerWidth");
    ExportValue(node, fLightCouplerThickness, "LightCouplerThickness");
    ExportValue(node, fSiliconeLength, "SiliconeLength");
    ExportValue(node, fSiliconeWidth, "SiliconeWidth");
    ExportValue(node, fSiliconeThickness, "SiliconeThickness");
    ExportValue(node, fNSiPM, "NSiPM");
    // Material
    ExportValue(node, fDensity, "Density");
    ExportValue(node, fRIndexEnergyBin, "RIndexEnergyBin");
    ExportValue(node, fRIndex, "RIndex");
    ExportValue(node, fAbsLengthEnergyBin, "AbsLengthEnergyBin");
    ExportValue(node, fAbsLength, "AbsLength");
    ExportValue(node, fScintillationComponent1EnergyBin, "ScintillationComponent1EnergyBin");
    ExportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ExportValue(node, fScintillationYield, "ScintillationYield");
    ExportValue(node, fScintillationRiseTimeConstant1, "ScintillationRiseTimeConstant1");
    ExportValue(node, fScintillationDecayTimeConstant1, "ScintillationDecayTimeConstant1");
    ExportValue(node, fResolutionScale, "ResolutionScale");
    ExportValue(node, fLightCouplerDensity, "LightCouplerDensity");
    ExportValue(node, fLightCouplerCarbonElement, "LightCouplerCarbonElement");
    ExportValue(node, fLightCouplerHydrogenElement, "LightCouplerHydrogenElement");
    ExportValue(node, fLightCouplerOxygenElement, "LightCouplerOxygenElement");
    ExportValue(node, fLightCouplerSiliconElement, "LightCouplerSiliconElement");
    ExportValue(node, fLightCouplerRIndex, "LightCouplerRIndex");
    ExportValue(node, fLightCouplerAbsLength, "LightCouplerAbsLength");
    ExportValue(node, fWindowDensity, "WindowDensity");
    ExportValue(node, fWindowCarbonElement, "WindowCarbonElement");
    ExportValue(node, fWindowHydrogenElement, "WindowHydrogenElement");
    ExportValue(node, fWindowOxygenElement, "WindowOxygenElement");
    ExportValue(node, fWindowRIndex, "WindowRIndex");
    ExportValue(node, fSiPMEnergyBin, "SiPMEnergyBin");
    ExportValue(node, fSiPMEfficiency, "SiPMEfficiency");
    ExportValue(node, fReflectorReflectivity, "ReflectorReflectivity");
    ExportValue(node, fCouplerTransmittance, "CouplerTransmittance");
    ExportValue(node, fAirPaintReflectivity, "AirPaintReflectivity");
    ExportValue(node, fCathodeSurface, "CathodeSurface");
    // Option
    ExportValue(node, fUseOptics, "UseOptics");
}

} // namespace MACE::PhaseI::Detector::Description
