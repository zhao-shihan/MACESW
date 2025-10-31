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
    fLength{this, 5.5_cm},
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

    fWindowLength{this, 0.3_cm},
    fWindowWidth{this, 0.3_cm},
    fWindowThickness{this, 0.05_cm},

    fLightCouplerLength{this, 0.3_cm},
    fLightCouplerWidth{this, 0.3_cm},
    fLightCouplerThickness{this, 0.01_cm},

    fSiliconeLength{this, 0.2_cm},
    fSiliconeWidth{this, 0.2_cm},
    fSiliconeThickness{this, 0.03_cm},
    fNSiPM{this, 2},

    // Material
    // Scintillator
    fDensity{this, 1.023_g_cm3},
    fRIndexEnergyBin{this, {3.524_eV, 2.504_eV}},
    fRIndex{this, {1.58, 1.58}},
    fAbsLengthEnergyBin{this, {3.524_eV, 2.504_eV}},
    fAbsLength{this, {120_cm, 120_cm}},
    fScintillationComponent1EnergyBin{this, {}},
    fScintillationComponent1{this, {}},
    fScintillationYield{this, 10200.},
    fScintillationRiseTimeConstant1{this, 0.5_ns},
    fScintillationDecayTimeConstant1{this, 1.4_ns},
    fResolutionScale{this, 1.}, // EJ-228
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
    fCathodeSurface{this, {0., 0.}} {

    fScintillationComponent1EnergyBin = {2.504819136_eV, 2.521592988_eV, 2.539408255_eV, 2.557477046_eV, 2.575804812_eV, 2.59439716_eV, 2.613259861_eV, 2.632398856_eV, 2.651820259_eV, 2.671530368_eV, 2.691535669_eV, 2.711842842_eV, 2.732458774_eV, 2.753390559_eV, 2.774645512_eV, 2.796231176_eV, 2.818155329_eV, 2.840425997_eV, 2.863051459_eV, 2.886040262_eV, 2.906394904_eV, 2.923384057_eV, 2.944410319_eV, 2.962816647_eV, 2.980685764_eV, 2.997634931_eV, 3.010115985_eV, 3.019888402_eV, 3.036775989_eV, 3.053708796_eV, 3.069566302_eV, 3.087015674_eV, 3.104739359_eV, 3.126096292_eV, 3.15050817_eV, 3.178367427_eV, 3.206723788_eV, 3.230303556_eV, 3.247550205_eV, 3.258420118_eV, 3.270591444_eV, 3.282004315_eV, 3.289429591_eV, 3.297204066_eV, 3.305932403_eV, 3.314615_eV, 3.321827566_eV, 3.329396596_eV, 3.338634135_eV, 3.350137711_eV, 3.369731216_eV, 3.393905016_eV, 3.423290686_eV, 3.456208313_eV, 3.489765144_eV, 3.52397998_eV};
    fScintillationComponent1 = {3.099257375, 3.600582756, 4.177771706, 4.831465768, 6.07630214, 6.915788312, 7.584040279, 8.481205118, 9.631816441, 10.60692155, 12.06332877, 13.84358499, 16.0643281, 18.40928004, 21.32113585, 24.99304269, 29.4130934, 33.20457762, 37.9171966, 43.12405877, 47.9005916, 52.76037928, 57.9711994, 64.13302755, 70.17870731, 76.29147146, 81.32775287, 85.31294593, 91.57546175, 97.97906398, 104.4194155, 111.8734693, 117.1275661, 121.0455413, 123.7363844, 122.6795446, 118.8843922, 112.0745972, 105.2053533, 98.39699985, 89.23084955, 80.11516599, 74.03630652, 67.40914101, 60.23838556, 53.48535228, 46.13470723, 40.48172683, 33.515501, 27.03645471, 19.72571879, 13.20465403, 7.535038289, 3.374747136, 1.483118612, 0.335316811};
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
    ExportValue(node, fScintillationDecayTimeConstant1, "ScintillationDcayTimeConstant1");
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
}

} // namespace MACE::PhaseI::Detector::Description
