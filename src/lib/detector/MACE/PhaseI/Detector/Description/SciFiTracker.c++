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
    fLightGuideCurvature{80_mm},
    fSiPMLength{1.3_mm},
    fSiPMWidth{1.3_mm},
    fSiPMThickness{0.055_mm},
    fTLightGuideLength{10_mm},
    fEpoxyThickness{0.105_mm},
    fNLayer{this, 16},
    fTypeOfLayer{this, {"Transverse", "Transverse", "LHelical", "LHelical", "Transverse", "Transverse", "RHelical", "RHelical", "Transverse", "Transverse", "LHelical", "LHelical", "Transverse", "Transverse", "RHelical", "RHelical" /**/}},
    fRLayer{this, {45_mm, 46.8_mm, 48.6_mm, 50.4_mm, 52.2_mm, 54_mm, 55.8_mm, 57.6_mm, 59.4_mm, 61.2_mm, 63_mm, 64.8_mm, 66.6_mm, 68.4_mm, 70.2_mm, 72_mm /**/}},
    // fRLayer{this, {45_mm, 46.8_mm, 50_mm, 51.8_mm, 55_mm, 56.8_mm, 60_mm, 61.8_mm, 65_mm, 66.8_mm, 70_mm, 76.8_mm, 80_mm, 86.8_mm, 90_mm, 91.8_mm /**/}},
    fIsSecond{this, {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 /**/}},
    fNFiber{this, {140, 140, 120, 120, 160, 160, 120, 120, 180, 180, 140, 140, 180, 180, 140, 140 /**/}},
    fFirstIDOfLayer{this, [this] { return CalculateFirstIDOfLayer(); }},
    fLastIDOfLayer{this, [this] { return CalculateLastIDOfLayer(); }},
    fCombinationOfLayer{this, {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} /**/}},
    fPitchOfLayer{this, [this] { return CalculateLayerPitch(); }},
    fLayerConfiguration{this, [this] { return CalculateLayerConfiguration(); }},
    fFiberMap{this, [this] { return CalculateFiberInformation(); }},
    // Optical properties
    fScintillationYield{8000},
    fScintillationTimeConstant1{3_ns},
    fResolutionScale{1},
    fScintillationWavelengthBin{646.6652846, 645.7000652, 644.6470987, 643.5941322, 642.5411657,
                                641.4881991, 640.372556, 639.3920158, 638.3919761, 637.1635152,
                                636.047872, 634.9949055, 633.941939, 632.9767197, 631.8360059,
                                630.607545, 629.5545784, 628.4138647, 626.4483272, 624.7986796,
                                623.5000209, 622.5348016, 621.481835, 620.4288685, 619.375902,
                                618.3229354, 617.2699689, 616.1643541, 615.1640359, 614.1110693,
                                613.0581028, 611.4600107, 610.6011809, 609.0982287, 607.8099839,
                                606.3893148, 605.1316047, 604.2833817, 603.2304152, 602.1774486,
                                601.1244821, 600.0715156, 599.0185491, 597.9655825, 596.912616,
                                595.8596495, 594.8066829, 593.7537164, 592.7007499, 591.6477833,
                                590.5948168, 589.5418503, 588.4888837, 587.4359172, 586.3829507,
                                585.3299842, 584.2770176, 583.2240511, 582.1710846, 581.118118,
                                580.0651515, 579.012185, 577.9592184, 576.9062519, 575.8532854,
                                574.8003188, 573.7473523, 572.6943858, 571.6414193, 570.5884527,
                                569.5354862, 568.4825197, 567.4295531, 566.4643338, 565.5868617,
                                564.7971368, 564.0074119, 563.0421926, 561.9892261, 560.9362595,
                                559.883293, 558.8303265, 557.7773599, 556.7243934, 555.6714269,
                                554.6184603, 553.5654938, 552.5125273, 551.4595607, 550.4065942,
                                549.3536277, 548.3006612, 547.2476946, 546.1947281, 545.1417616,
                                544.088795, 543.0358285, 541.982862, 540.9298954, 539.8769289,
                                538.8239624, 537.7709959, 536.7180293, 535.5773156, 534.6120963,
                                533.5591297, 532.4886138, 531.4531967, 530.4002301, 529.3472636,
                                528.3820443, 527.5923194, 526.8903417, 526.188364, 525.4863863,
                                524.7844086, 524.082431, 523.3804533, 522.6784756, 521.9764979,
                                521.3622674, 520.8357841, 520.3093009, 519.7828176, 519.2563344,
                                518.7298511, 518.2033678, 517.6768846, 517.1504013, 516.7116652,
                                516.3606764, 516.0096875, 515.6586987, 515.3077099, 514.956721,
                                514.6057322, 514.2547433, 513.9037545, 513.5527656, 513.1140296,
                                512.6752935, 512.2365575, 511.7100742, 511.183591, 510.6571077,
                                510.1306244, 509.6041412, 509.0776579, 508.5511746, 508.0246914,
                                507.4982081, 506.9717248, 506.4452416, 505.9187583, 505.392275,
                                504.8657918, 504.3393085, 503.8915727, 503.1108475, 502.7013606,
                                501.8823866, 500.9171673, 499.8642007, 498.8112342, 497.7056194,
                                496.7053012, 495.6523346, 494.5116209, 493.5464016, 492.493435,
                                491.4404685, 490.387502, 489.3345354, 488.321454}, // 3HF
    fScintillationComponent1{4.59E-02, 4.71E-02, 4.81E-02, 4.95E-02, 5.06E-02,
                             5.31E-02, 5.35E-02, 5.47E-02, 5.25E-02, 5.78E-02,
                             5.72E-02, 5.88E-02, 5.97E-02, 6.02E-02, 6.53E-02,
                             6.69E-02, 6.81E-02, 6.98E-02, 7.32E-02, 7.65E-02,
                             8.46E-02, 8.91E-02, 9.42E-02, 1.00E-01, 1.06E-01,
                             1.12E-01, 1.17E-01, 1.20E-01, 1.20E-01, 0.120206658,
                             0.120206658, 0.122302158, 0.12440689, 1.31E-01, 1.39E-01,
                             1.48E-01, 1.55E-01, 1.61E-01, 1.68E-01, 1.76E-01,
                             1.84E-01, 1.93E-01, 2.01E-01, 2.09E-01, 2.18E-01,
                             2.26E-01, 2.34E-01, 2.42E-01, 2.50E-01, 2.57E-01,
                             2.64E-01, 2.71E-01, 2.78E-01, 2.85E-01, 2.91E-01,
                             2.98E-01, 3.07E-01, 3.16E-01, 3.25E-01, 3.34E-01,
                             3.43E-01, 3.52E-01, 3.61E-01, 3.69E-01, 3.78E-01,
                             3.88E-01, 3.97E-01, 4.07E-01, 4.16E-01, 4.25E-01,
                             4.33E-01, 4.40E-01, 4.48E-01, 4.57E-01, 4.67E-01,
                             4.77E-01, 4.87E-01, 4.97E-01, 5.07E-01, 5.15E-01,
                             5.21E-01, 5.28E-01, 5.35E-01, 5.41E-01, 5.48E-01,
                             5.55E-01, 5.64E-01, 5.73E-01, 5.82E-01, 5.90E-01,
                             5.97E-01, 6.04E-01, 6.11E-01, 6.19E-01, 6.26E-01,
                             6.35E-01, 6.43E-01, 6.50E-01, 6.55E-01, 6.58E-01,
                             6.60E-01, 6.61E-01, 6.61E-01, 6.60E-01, 6.57E-01,
                             6.55E-01, 6.51E-01, 6.48E-01, 6.44E-01, 6.37E-01,
                             6.28E-01, 6.19E-01, 6.08E-01, 5.97E-01, 5.86E-01,
                             5.75E-01, 5.64E-01, 5.52E-01, 5.40E-01, 5.28E-01,
                             5.18E-01, 5.08E-01, 4.99E-01, 4.89E-01, 4.79E-01,
                             4.69E-01, 4.58E-01, 4.46E-01, 4.33E-01, 4.22E-01,
                             4.12E-01, 4.02E-01, 3.90E-01, 3.79E-01, 3.68E-01,
                             3.56E-01, 3.44E-01, 3.34E-01, 3.24E-01, 3.13E-01,
                             0.301656707, 0.290736102, 0.278135404, 0.266654769, 0.255174133,
                             0.243413481, 0.23165283, 0.220172194, 0.208411543, 0.196930907,
                             0.185730287, 0.174809682, 0.16304903, 0.151568395, 0.140367774,
                             0.129167154, 0.117686518, 0.112024666, 0.094165215, 0.088844921,
                             0.076524238, 0.066023657, 0.05832323, 0.052162889, 0.047626638,
                             0.043762424, 0.040402238, 0.037210061, 0.034801927, 0.033915212,
                             0.02906161, 0.0252814, 0.021921214, 0.013444381}, // 3HF
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
    fThreshold{2},
    fClusterLength{3},
    fThresholdTime{10},
    fTimeWindow{10},
    fDeadTime{10},
    fCentroidThetaThreshold{0.02 * std::numbers::pi},
    fCentroidZThreshold{25_mm},
    fOnePhotonDarkCountRate{1000000} {}
auto SciFiTracker::CalculateLayerPitch() const -> std::vector<double> {
    std::vector<double> Pitch;
    for (int i{}; i < fNLayer; i++) {
        if (fTypeOfLayer->at(i) == "LHelical") {
            Pitch.push_back(std::atan(fFiberLength / (2_pi * fRLayer->at(i))));
        } else if (fTypeOfLayer->at(i) == "RHelical") {
            Pitch.push_back(-std::atan(fFiberLength / (2_pi * fRLayer->at(i))));
        } else if (fTypeOfLayer->at(i) == "Transverse") {
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
        layer.firstID = fFirstIDOfLayer->at(i);
        layer.lastID = fLastIDOfLayer->at(i);
        layer.isSecond = fIsSecond->at(i);
        layer.fiber.layerType = fTypeOfLayer->at(i);
        layer.fiber.pitch = fPitchOfLayer->at(i);
        layer.fiber.radius = fRLayer->at(i);
    }
    return layerConfig;
}

auto SciFiTracker::CalculateFiberInformation() const -> std::vector<FiberInformation> {
    std::vector<FiberInformation> fiberMap;
    for (int i{}; i < fNLayer; i++) {
        for (int j{}; j < fNFiber->at(i); j++) {
            FiberInformation fiber{};
            fiber.layerID = i;
            fiber.localID = j;
            fiber.layerType = fTypeOfLayer->at(i);
            fiber.radius = fRLayer->at(i);
            fiber.pitch = fPitchOfLayer->at(i);
            fiber.rotationAngle = (j + fIsSecond->at(i) * 0.5) / fNFiber->at(i) * 2_pi;
            fiberMap.push_back(fiber);
        }
    }
    return fiberMap;
}

auto SciFiTracker::CalculateFirstIDOfLayer() const -> std::vector<int> {
    std::vector<int> firstID;
    int id{0};
    firstID.push_back(0);
    for (int i{}; i < (fNLayer - 1); i++) {
        id += fNFiber->at(i);
        firstID.push_back(id);
    }
    return firstID;
}

auto SciFiTracker::CalculateLastIDOfLayer() const -> std::vector<int> {
    std::vector<int> lastID;
    int id{-1};
    for (int i{}; i < fNLayer; i++) {
        id += fNFiber->at(i);
        lastID.push_back(id);
    }
    return lastID;
}

auto SciFiTracker::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fEpoxyThickness, "EpoxyThickness");
    ImportValue(node, fSiPMLength, "SiPMLength");
    ImportValue(node, fSiPMWidth, "SiPMWidth");
    ImportValue(node, fSiPMThickness, "SiPMThickness");
    ImportValue(node, fFiberCoreWidth, "CoreRadius");
    ImportValue(node, fFiberCladdingWidth, "CladdingRadius");
    ImportValue(node, fLightGuideCurvature, "CurvatureOfLightGuide");
    ImportValue(node, fFiberLength, "FiberHalfLength");
    ImportValue(node, fTLightGuideLength, "LengthOFTransverseLightGuide");
    ImportValue(node, fNLayer, "NumberOfFiber");
    ImportValue(node, fTypeOfLayer, "TypeOfLayer");
    ImportValue(node, fRLayer, "RadiusOfLayer");
    ImportValue(node, fIsSecond, "IfThisLayerNumberIsEven");
    ImportValue(node, fNFiber, "NFiberOfFiberInALayer");
    ImportValue(node, fCombinationOfLayer, "TheseLayersWillReconstructOneHitPoint");
    // Optical properties
    ImportValue(node, fScintillationWavelengthBin, "ScintillationWavelengthBin");
    ImportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ImportValue(node, fScintillationYield, "ScintillationYield");
    ImportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    ImportValue(node, fResolutionScale, "ResolutionScale");
    // Reconstruction
    ImportValue(node, fThreshold, "OptPhoThresholdNumberOfSiPM");
    ImportValue(node, fClusterLength, "LengthOfCluster");
    ImportValue(node, fThresholdTime, "OptPhoThresholdTimeOfSiPM");
    ImportValue(node, fTimeWindow, "TimeWindowOfSiPM");
    ImportValue(node, fDeadTime, "DeadTimeOfSiPM");
}

auto SciFiTracker::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fEpoxyThickness, "EpoxyThickness");
    ExportValue(node, fSiPMLength, "SiPMLength");
    ExportValue(node, fSiPMWidth, "SiPMWidth");
    ExportValue(node, fSiPMThickness, "SiPMThickness");
    ExportValue(node, fFiberCoreWidth, "CoreRadius");
    ExportValue(node, fFiberCladdingWidth, "CladdingRadius");
    ExportValue(node, fLightGuideCurvature, "CurvatureOfLightGuide");
    ExportValue(node, fFiberLength, "FiberHalfLength");
    ExportValue(node, fTLightGuideLength, "LengthOFTransverseLightGuide");
    ExportValue(node, fNLayer, "NumberOfFiber");
    ExportValue(node, fTypeOfLayer, "TypeOfLayer");
    ExportValue(node, fRLayer, "RadiusOfLayer");
    ExportValue(node, fIsSecond, "IfThisLayerNumberIsEven");
    ExportValue(node, fNFiber, "NFiberOfFiberInALayer");
    ExportValue(node, fCombinationOfLayer, "TheseLayersWillReconstructOneHitPoint");
    // Optical properties
    ExportValue(node, fScintillationWavelengthBin, "ScintillationWavelengthBin");
    ExportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ExportValue(node, fScintillationYield, "ScintillationYield");
    ExportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    ExportValue(node, fResolutionScale, "ResolutionScale");
    // Reconstruction
    ExportValue(node, fThreshold, "OptPhoThresholdNumberOfSiPM");
    ExportValue(node, fClusterLength, "LengthOfCluster");
    ExportValue(node, fThresholdTime, "OptPhoThresholdTimeOfSiPM");
    ExportValue(node, fTimeWindow, "TimeWindowOfSiPM");
    ExportValue(node, fDeadTime, "DeadTimeOfSiPM");
}

} // namespace MACE::PhaseI::Detector::Description
