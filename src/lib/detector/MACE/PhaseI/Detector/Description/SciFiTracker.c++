#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorCast.h++"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::PhysicalConstant;
using namespace Mustard::LiteralUnit;

SciFiTracker::SciFiTracker() : // clang-format off
    DescriptionWithCacheBase{"SciFiTracker"}, // clang-format on
    // Geometry
    fBracketInnerRadius{43_mm},
    fBracketOuterRadius{75_mm},
    fSiliconeOilThickness{0.01_mm},
    fFiberCoreWidth{0.96 * 1_mm},
    fFiberCladdingWidth{1_mm},
    fFiberLength{325.4_mm},
    fSiPMLength{1.3_mm},
    fSiPMWidth{1.3_mm},
    fSiPMThickness{0.055_mm},
    fTransverseLightGuideLength{10_mm},
    fEpoxyThickness{0.105_mm},
    fNLayer{this, 16},
    fLayerType{this, {"Transverse", "Transverse", "LHelical", "LHelical", "Transverse", "Transverse", "RHelical", "RHelical", "Transverse", "Transverse", "LHelical", "LHelical", "Transverse", "Transverse", "RHelical", "RHelical" /**/}},
    fLayerRadius{this, {45_mm, 46.8_mm, 48.6_mm, 50.4_mm, 52.2_mm, 54_mm, 55.8_mm, 57.6_mm, 59.4_mm, 61.2_mm, 63_mm, 64.8_mm, 66.6_mm, 68.4_mm, 70.2_mm, 72_mm /**/}},
    fNFiberALayer{this, {140, 140, 120, 120, 160, 160, 120, 120, 180, 180, 140, 140, 180, 180, 140, 140 /**/}},
    fLayerFiberIDRange{this, [this] { return CalculateLayerFiberIDRange(); }},
    fLayerPitch{this, [this] { return CalculateLayerPitch(); }},
    fLayerConfiguration{this, [this] { return CalculateLayerConfiguration(); }},
    fFiberMap{this, [this] { return CalculateFiberInformation(); }},
    // Optical properties
    fScintillationYield{8000},
    fScintillationTimeConstant1{3_ns},
    fScintillationWavelengthBin{641.2860796, 635.2575813, 629.2290829, 623.2005846,
                                617.1720862, 611.1435879, 605.1150895, 599.0865912, 593.0580928,
                                587.0295944, 581.0010961, 574.9725977, 568.9440994, 562.915601,
                                556.8871027, 552.7767629, 548.1183778, 542.5009134, 537.4314943,
                                533.8691999, 530.8853976, 528.3887468, 525.598698, 522.6342711,
                                520.1680672, 517.975886, 515.6466935, 512.3127512, 510.5224699,
                                508.6591158, 506.4669346, 504.1377421, 500.6576544, 496.282426,
                                489.7972232, 484.849592, 483.0379978, 481.8734015, 480.7088053,
                                479.4632477, 478.3796127, 478.1329923, 477.1465108, 476.5984655,
                                476.3244428, 475.5023749, 475.4240827, 474.6803069, 474.5432956,
                                474.0713677, 473.1579588, 472.214103, 470.9124954, 469.1998539,
                                468.7016308, 464.5625474}, // BCF20
    fScintillationComponent1{1.56350902e-04, 1.59084310e-04, 2.49286753e-04, 3.51887152e-04,
                             4.34084617e-04, 5.49961565e-04, 7.38566675e-04, 9.35372006e-04,
                             1.23878023e-03, 1.36998378e-03, 1.62419067e-03, 2.02873496e-03,
                             2.38681133e-03, 2.93895962e-03, 3.43917317e-03, 4.15532590e-03,
                             4.81681049e-03, 5.56599190e-03, 6.35669506e-03, 7.07437720e-03,
                             7.77678697e-03, 8.43994198e-03, 9.09185964e-03, 9.79297863e-03,
                             1.04770138e-02, 1.10966187e-02, 1.17623987e-02, 1.24773365e-02,
                             1.32069741e-02, 1.40625610e-02, 1.48163957e-02, 1.55233394e-02,
                             1.62775894e-02, 1.66606240e-02, 1.67661286e-02, 1.59317560e-02,
                             1.51637825e-02, 1.44120954e-02, 1.34060876e-02, 1.21614761e-02,
                             1.11610490e-02, 1.02864194e-02, 9.40210138e-03, 8.69551557e-03,
                             7.74588429e-03, 5.67373370e-03, 6.38103541e-03, 7.04180410e-03,
                             5.10245156e-03, 4.16534840e-03, 3.27501687e-03, 2.44147947e-03,
                             1.63717435e-03, 6.01349624e-06, 8.93004192e-04, 1.80636176e-04}, // BCF20
    fSiPMEnergyBin{1.391655126_eV, 1.413303953_eV, 1.436778788_eV, 1.461046623_eV, 1.486148332_eV, 1.512127645_eV, 1.533451437_eV,
                   1.553243676_eV, 1.579239384_eV, 1.601331725_eV, 1.618380329_eV, 1.644070091_eV, 1.668575932_eV, 1.695332333_eV,
                   1.722960822_eV, 1.750124077_eV, 1.769418592_eV, 1.796139112_eV, 1.823679036_eV, 1.850275906_eV, 1.880443107_eV,
                   1.905389876_eV, 1.927096598_eV, 1.948591293_eV, 1.968750142_eV, 1.989552718_eV, 2.012699006_eV, 2.044509747_eV,
                   2.086746867_eV, 2.108593697_eV, 2.13196381_eV, 2.162065168_eV, 2.196197125_eV, 2.211510324_eV, 2.240146328_eV,
                   2.277682676_eV, 2.31087249_eV, 2.342157708_eV, 2.374301646_eV, 2.410389305_eV, 2.452932479_eV, 2.507137915_eV,
                   2.565089699_eV, 2.6485729_eV, 2.732230683_eV, 2.795153262_eV, 2.842415847_eV, 2.907557927_eV, 2.975755876_eV,
                   3.029226473_eV, 3.073774257_eV, 3.139907148_eV, 3.179961838_eV, 3.207163267_eV, 3.244659512_eV, 3.289219352_eV,
                   3.307290247_eV, 3.327878491_eV, 3.363166032_eV, 3.399209944_eV, 3.435444057_eV, 3.46043183_eV, 3.48852431_eV,
                   3.516457699_eV, 3.550580722_eV, 3.563879348_eV, 3.57404697_eV, 3.587160946_eV, 3.60984247_eV, 3.628195256_eV,
                   3.637441808_eV, 3.651400344_eV, 3.670179231_eV, 3.684390646_eV, 3.696659746_eV, 3.720794081_eV, 3.721383683_eV,
                   3.742354188_eV, 3.755013028_eV, 3.787037996_eV, 3.809782448_eV, 3.840536792_eV}, // S13360
    fSiPMQuantumEfficiency{0.038361565, 0.043881036, 0.050158203, 0.056879188, 0.063896051, 0.071245776, 0.077096023, 0.082806497, 0.09049302,
                           0.096846043, 0.102167517, 0.110439587, 0.118523116, 0.127329437, 0.136497387, 0.144697087, 0.152101728, 0.160383488,
                           0.169784037, 0.17901796, 0.189932435, 0.197788503, 0.2061559, 0.215114701, 0.226013151, 0.23524354, 0.244663368,
                           0.255647156, 0.270165176, 0.278993559, 0.28798576, 0.297864503, 0.310058789, 0.315182084, 0.324696839, 0.336285043,
                           0.347467345, 0.356656635, 0.364883088, 0.373827813, 0.382839555, 0.3902882, 0.395495771, 0.400343653, 0.401560403,
                           0.396238242, 0.390316468, 0.384591232, 0.375317514, 0.365819506, 0.358254968, 0.346811362, 0.335599315, 0.326263057,
                           0.314742636, 0.300694748, 0.29059875, 0.283296713, 0.270049269, 0.257841507, 0.247144881, 0.237727757, 0.22854291,
                           0.217013978, 0.206721701, 0.200096265, 0.191250653, 0.181575856, 0.168947005, 0.159534377, 0.149674853, 0.139503115,
                           0.129670093, 0.120230653, 0.108280609, 0.091831406, 0.098424138, 0.083937488, 0.073056832, 0.060399447, 0.047887957,
                           0.034501313}, // S13360
    // reconstruction
    fSiPMOpticalPhotonCountThreshold{2},
    fClusterLength{3},
    fThresholdTime{10},
    fTimeWindow{10},
    fSiPMDeadTime{10},
    fCentroidThetaThreshold{0.02 * std::numbers::pi},
    fCentroidZThreshold{25_mm} {}
auto SciFiTracker::CalculateLayerPitch() const -> std::vector<double> {
    std::vector<double> Pitch;
    for (int i{}; i < fNLayer; i++) {
        if (fLayerType->at(i) == "LHelical") {
            Pitch.push_back(std::atan(fFiberLength / (2_pi * fLayerRadius->at(i))));
        } else if (fLayerType->at(i) == "RHelical") {
            Pitch.push_back(-std::atan(fFiberLength / (2_pi * fLayerRadius->at(i))));
        } else if (fLayerType->at(i) == "Transverse") {
            Pitch.push_back(0);
        }
    }
    return Pitch;
}

auto SciFiTracker::CalculateLayerConfiguration() const -> std::vector<LayerConfiguration> {
    std::vector<LayerConfiguration> layerConfig;
    layerConfig.reserve(fNLayer);
    for (int i{}; i < fNLayer; i++) {
        auto& layer{layerConfig.emplace_back()};
        layer.firstID = fLayerFiberIDRange->at(i).first;
        layer.lastID = fLayerFiberIDRange->at(i).second;
        layer.fiberNumber = fNFiberALayer->at(i);
        layer.fiber.layerType = fLayerType->at(i);
        layer.fiber.pitch = fLayerPitch->at(i);
        layer.fiber.radius = fLayerRadius->at(i);
    }
    return layerConfig;
}

auto SciFiTracker::CalculateFiberInformation() const -> std::vector<FiberInformation> {
    std::vector<FiberInformation> fiberMap;
    for (int i{}; i < fNLayer; i++) {
        for (int j{}; j < fNFiberALayer->at(i); j++) {
            FiberInformation fiber{};
            fiber.layerID = i;
            fiber.localID = j;
            fiber.layerType = fLayerType->at(i);
            fiber.radius = fLayerRadius->at(i);
            fiber.pitch = fLayerPitch->at(i);
            fiber.rotationAngle = (j + i % 2 * 0.5) / fNFiberALayer->at(i) * 2_pi;
            fiberMap.push_back(fiber);
        }
    }
    return fiberMap;
}

auto SciFiTracker::CalculateLayerFiberIDRange() const -> std::vector<std::pair<int, int>> {
    std::vector<std::pair<int, int>> layerFiberIDRange;
    int currentID{0};
    for (int i{}; i < fNLayer; i++) {
        int firstID = currentID;
        currentID += fNFiberALayer->at(i);
        int lastID = currentID - 1;
        layerFiberIDRange.emplace_back(firstID, lastID);
    }
    return layerFiberIDRange;
}

auto SciFiTracker::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fBracketInnerRadius, "BracketInnerRadius");
    ImportValue(node, fBracketOuterRadius, "BracketOuterRadius");
    ImportValue(node, fSiliconeOilThickness, "SiliconeOilThickness");
    ImportValue(node, fEpoxyThickness, "EpoxyThickness");
    ImportValue(node, fSiPMLength, "SiPMLength");
    ImportValue(node, fSiPMWidth, "SiPMWidth");
    ImportValue(node, fSiPMThickness, "SiPMThickness");
    ImportValue(node, fFiberCoreWidth, "CoreWidth");
    ImportValue(node, fFiberCladdingWidth, "CladdingWidth");
    ImportValue(node, fFiberLength, "FiberLength");
    ImportValue(node, fTransverseLightGuideLength, "TransverseLightGuideLength");
    ImportValue(node, fNLayer, "LayerNumber");
    ImportValue(node, fLayerType, "LayerType");
    ImportValue(node, fLayerRadius, "LayerRadius");
    ImportValue(node, fNFiberALayer, "FiberNumberALayer");

    // Optical properties
    ImportValue(node, fScintillationWavelengthBin, "ScintillationWavelengthBin");
    ImportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ImportValue(node, fSiPMEnergyBin, "SiPMEnergyBin");
    ImportValue(node, fSiPMQuantumEfficiency, "SiPMQuantumEfficiency");
    ImportValue(node, fScintillationYield, "ScintillationYield");
    ImportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    // Reconstruction
    ImportValue(node, fSiPMOpticalPhotonCountThreshold, "SiPMOpticalPhotonCountThreshold");
    ImportValue(node, fClusterLength, "ClusterLength");
    ImportValue(node, fThresholdTime, "SiPMOptPhoThresholdTime");
    ImportValue(node, fTimeWindow, "SiPMTimeWindow");
    ImportValue(node, fSiPMDeadTime, "SiPMDeadTime");
    ImportValue(node, fCentroidThetaThreshold, "CentroidThetaThreshold");
    ImportValue(node, fCentroidZThreshold, "CentroidZThreshold");
}

auto SciFiTracker::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fBracketInnerRadius, "BracketInnerRadius");
    ExportValue(node, fBracketOuterRadius, "BracketOuterRadius");
    ExportValue(node, fSiliconeOilThickness, "SiliconeOilThickness");
    ExportValue(node, fEpoxyThickness, "EpoxyThickness");
    ExportValue(node, fSiPMLength, "SiPMLength");
    ExportValue(node, fSiPMWidth, "SiPMWidth");
    ExportValue(node, fSiPMThickness, "SiPMThickness");
    ExportValue(node, fFiberCoreWidth, "CoreWidth");
    ExportValue(node, fFiberCladdingWidth, "CladdingWidth");
    ExportValue(node, fFiberLength, "FiberLength");
    ExportValue(node, fTransverseLightGuideLength, "TransverseLightGuideLength");
    ExportValue(node, fNLayer, "NLayer");
    ExportValue(node, fLayerType, "LayerType");
    ExportValue(node, fLayerRadius, "LayerRadius");
    ExportValue(node, fNFiberALayer, "FiberNumberALayer");

    // Optical properties
    ExportValue(node, fScintillationWavelengthBin, "ScintillationWavelengthBin");
    ExportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ExportValue(node, fSiPMEnergyBin, "SiPMEnergyBin");
    ExportValue(node, fSiPMQuantumEfficiency, "SiPMQuantumEfficiency");
    ExportValue(node, fScintillationYield, "ScintillationYield");
    ExportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    // Reconstruction
    ExportValue(node, fSiPMOpticalPhotonCountThreshold, "SiPMOpticalPhotonCountThreshold");
    ExportValue(node, fClusterLength, "ClusterLength");
    ExportValue(node, fThresholdTime, "SiPMOptPhoThresholdTime");
    ExportValue(node, fTimeWindow, "SiPMTimeWindow");
    ExportValue(node, fSiPMDeadTime, "SiPMDeadTime");
    ExportValue(node, fCentroidThetaThreshold, "CentroidThetaThreshold");
    ExportValue(node, fCentroidZThreshold, "CentroidZThreshold");
}

} // namespace MACE::PhaseI::Detector::Description
