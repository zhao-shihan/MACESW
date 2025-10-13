#pragma once

#include "Mustard/Detector/Description/DescriptionWithCacheBase.h++"

#include "muc/array"

#include <string>
#include <utility>

namespace MACE::PhaseI::Detector::Description {

class SciFiTracker final : public Mustard::Detector::Description::DescriptionWithCacheBase<SciFiTracker> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    SciFiTracker();
    ~SciFiTracker() = default;

public:
    // Geometry
    auto BracketInnerRadius() const -> auto { return fBracketInnerRadius; }
    auto BracketOuterRadius() const -> auto { return fBracketOuterRadius; }
    auto EpoxyThickness() const -> auto { return fEpoxyThickness; }
    auto SiliconeOilThickness() const -> auto { return fSiliconeOilThickness; }
    auto SiPMLength() const -> auto { return fSiPMLength; }
    auto SiPMWidth() const -> auto { return fSiPMWidth; }
    auto SiPMThickness() const -> auto { return fSiPMThickness; }
    auto FiberCoreWidth() const -> auto { return fFiberCoreWidth; }
    auto FiberCladdingWidth() const -> auto { return fFiberCladdingWidth; }
    auto FiberLength() const -> auto { return fFiberLength; }
    auto TLightGuideLength() const -> auto { return fTLightGuideLength; }
    auto DetectorLayerConfiguration() const -> const auto& { return *fLayerConfiguration; }
    auto DetectorFiberInformation() const -> const auto& { return *fFiberMap; }
    auto NLayer() const -> auto { return *fNLayer; }
    auto TypeOfLayer() const -> auto& { return fTypeOfLayer; }
    auto RLayer() const -> const auto& { return fRLayer; }
    auto IsSecond() const -> const auto& { return fIsSecond; }
    auto FirstIDOfLayer() const -> const auto& { return fFirstIDOfLayer; }
    auto LastIDOfLayer() const -> const auto& { return fLastIDOfLayer; }
    auto NFiber() const -> const auto& { return fNFiber; }
    auto CombinationOfLayer() const -> auto& { return fCombinationOfLayer; }
    // Optical properties
    auto ScintillationTimeConstant1() const -> auto { return fScintillationTimeConstant1; }
    auto ScintillationWaveLengthBin() const -> const auto& { return fScintillationWavelengthBin; }
    auto ScintillationComponent1() const -> const auto& { return fScintillationComponent1; }
    auto SiPMEnergyBin() const -> const auto& { return fSiPMEnergyBin; }
    auto SiPMQuantumEfficiency() const -> const auto& { return fSiPMQuantumEfficiency; }
    // Reconstruction
    auto Threshold() const -> const auto { return fThreshold; }
    auto ClusterLength() const -> const auto { return fClusterLength; }
    auto ThresholdTime() const -> const auto { return fThresholdTime; }
    auto TimeWindow() const -> const auto { return fTimeWindow; }
    auto DeadTime() const -> const auto { return fDeadTime; }
    auto CentroidThetaThreshold() const -> const auto { return fCentroidThetaThreshold; }
    auto CentroidZThreshold() const -> const auto { return fCentroidZThreshold; }
    auto OnePhotonDarkCountRate() const -> const auto { return fOnePhotonDarkCountRate; }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    auto BracketInnerRadius(double val) -> void { fBracketInnerRadius = val; }
    auto BracketOuterRadius(double val) -> void { fBracketOuterRadius = val; }
    auto EpoxyThickness(double val) -> void { fEpoxyThickness = val; }
    auto SiliconeOilThickness(double val) -> void { fSiliconeOilThickness = val; }
    auto SiPMLength(double val) -> void { fSiPMLength = val; }
    auto SiPMWidth(double val) -> void { fSiPMWidth = val; }
    auto SiPMThickness(double val) -> void { fSiPMThickness = val; }
    auto FiberCoreRadius(double val) -> void { fFiberCoreWidth = val; }
    auto FiberCladdingRadius(double val) -> void { fFiberCladdingWidth = val; }
    auto FiberLength(double val) -> void { fFiberLength = val; }
    auto TLightGuideLength(double val) -> void { fTLightGuideLength = val; }
    auto ScintillationTimeConstant1(double val) -> void { fScintillationTimeConstant1 = val; }
    auto NLayer(int val) -> void { fNLayer = val; }
    auto TypeOfLayer(std::vector<std::string> val) -> void { fTypeOfLayer = std::move(val); }
    auto RLayer(std::vector<double> val) -> void { fRLayer = std::move(val); }
    auto IsSecond(std::vector<bool> val) -> void { fIsSecond = std::move(val); }
    auto NFiberOfLayer(std::vector<int> val) -> void { fNFiber = std::move(val); }
    auto CombinationOfLayer(std::vector<std::vector<int>> val) -> void { fCombinationOfLayer = std::move(val); }

    auto ScintillationWaveLengthBin(std::vector<double> val) -> void { fScintillationWavelengthBin = std::move(val); }
    auto ScintillationComponent1(std::vector<double> val) -> void { fScintillationComponent1 = std::move(val); }
    auto SiPMEnergyBin(std::vector<double> val) -> void { fSiPMEnergyBin = std::move(val); }
    auto SiPMQuantumEfficiency(std::vector<double> val) -> void { fSiPMQuantumEfficiency = std::move(val); }

    auto Threshold(int val) -> void { fThreshold = val; }
    auto ClusterLength(int val) -> void { fClusterLength = val; }
    auto ThresholdTime(double val) -> void { fThresholdTime = val; }
    auto TimeWindow(double val) -> void { fTimeWindow = val; }
    auto DeadTime(double val) -> void { fDeadTime = val; }
    auto CentroidThetaThreshold(double val) -> void { fCentroidThetaThreshold = val; }
    auto CentroidZThreshold(double val) -> void { fCentroidZThreshold = val; }
    auto OnePhotonDarkCountRate(double val) -> void { fOnePhotonDarkCountRate = val; }

public:
    struct LayerConfiguration {
        struct FiberConfiguration {
            std::string layerType;
            double radius;
            double pitch;
        };
        int firstID;
        int lastID;
        bool isSecond;
        std::string name;
        FiberConfiguration fiber;
    };

    struct FiberInformation {
        int layerID;
        int localID;
        std::string layerType;
        double radius;
        double pitch;
        double rotationAngle;
    };

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fBracketInnerRadius;
    double fBracketOuterRadius;
    double fSiliconeOilThickness;
    double fFiberCoreWidth;
    double fFiberCladdingWidth;
    double fFiberLength;
    double fSiPMLength;
    double fSiPMWidth;
    double fSiPMThickness;
    double fTLightGuideLength;
    double fEpoxyThickness;

    auto CalculateLayerConfiguration() const -> std::vector<LayerConfiguration>;
    auto CalculateFiberInformation() const -> std::vector<FiberInformation>;

    auto CalculateLayerPitch() const -> std::vector<double>;
    auto CalculateFirstIDOfLayer() const -> std::vector<int>;
    auto CalculateLastIDOfLayer() const -> std::vector<int>;
    Simple<int> fNLayer;
    Simple<std::vector<std::string>> fTypeOfLayer;
    Simple<std::vector<double>> fRLayer;
    Simple<std::vector<bool>> fIsSecond;

    Simple<std::vector<int>> fNFiber;
    Cached<std::vector<int>> fFirstIDOfLayer;
    Cached<std::vector<int>> fLastIDOfLayer;

    Simple<std::vector<std::vector<int>>> fCombinationOfLayer;
    Cached<std::vector<double>> fPitchOfLayer;
    Cached<std::vector<LayerConfiguration>> fLayerConfiguration;
    Cached<std::vector<FiberInformation>> fFiberMap;

    double fScintillationYield;
    double fScintillationTimeConstant1;
    double fResolutionScale;

    std::vector<double> fScintillationWavelengthBin;
    std::vector<double> fScintillationComponent1;
    std::vector<double> fSiPMEnergyBin;
    std::vector<double> fSiPMQuantumEfficiency;

    int fThreshold;
    int fClusterLength;
    double fThresholdTime;
    double fTimeWindow;
    double fDeadTime;
    double fCentroidThetaThreshold;
    double fCentroidZThreshold;
    double fOnePhotonDarkCountRate;
};

} // namespace MACE::PhaseI::Detector::Description
