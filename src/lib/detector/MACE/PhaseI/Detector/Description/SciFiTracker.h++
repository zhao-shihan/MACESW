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
    auto TransverseLightGuideLength() const -> auto { return fTransverseLightGuideLength; }
    auto DetectorLayerConfiguration() const -> const auto& { return *fLayerConfiguration; }
    auto DetectorFiberInformation() const -> const auto& { return *fFiberMap; }
    auto NLayer() const -> auto { return *fNLayer; }
    auto LayerType() const -> auto& { return fLayerType; }
    auto LayerRadius() const -> const auto& { return fLayerRadius; }
    auto LayerFiberIDRange() const -> const auto& { return fLayerFiberIDRange; }
    auto NFiberALayer() const -> const auto& { return fNFiberALayer; }
    // Optical properties
    auto ScintillationTimeConstant1() const -> auto { return fScintillationTimeConstant1; }
    auto ScintillationWaveLengthBin() const -> const auto& { return fScintillationWavelengthBin; }
    auto ScintillationComponent1() const -> const auto& { return fScintillationComponent1; }
    auto SiPMEnergyBin() const -> const auto& { return fSiPMEnergyBin; }
    auto SiPMQuantumEfficiency() const -> const auto& { return fSiPMQuantumEfficiency; }
    // Reconstruction
    auto SiPMOpticalPhotonCountThreshold() const -> const auto { return fSiPMOpticalPhotonCountThreshold; }
    auto ClusterLength() const -> const auto { return fClusterLength; }
    auto ThresholdTime() const -> const auto { return fThresholdTime; }
    auto TimeWindow() const -> const auto { return fTimeWindow; }
    auto SiPMDeadTime() const -> const auto { return fSiPMDeadTime; }
    auto CentroidThetaThreshold() const -> const auto { return fCentroidThetaThreshold; }
    auto CentroidZThreshold() const -> const auto { return fCentroidZThreshold; }

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
    auto TransverseLightGuideLength(double val) -> void { fTransverseLightGuideLength = val; }
    auto ScintillationTimeConstant1(double val) -> void { fScintillationTimeConstant1 = val; }
    auto NLayer(int val) -> void { fNLayer = val; }
    auto LayerType(std::vector<std::string> val) -> void { fLayerType = std::move(val); }
    auto LayerRadius(std::vector<double> val) -> void { fLayerRadius = std::move(val); }
    auto NFiberALayer(std::vector<int> val) -> void { fNFiberALayer = std::move(val); }

    auto ScintillationWaveLengthBin(std::vector<double> val) -> void { fScintillationWavelengthBin = std::move(val); }
    auto ScintillationComponent1(std::vector<double> val) -> void { fScintillationComponent1 = std::move(val); }
    auto SiPMEnergyBin(std::vector<double> val) -> void { fSiPMEnergyBin = std::move(val); }
    auto SiPMQuantumEfficiency(std::vector<double> val) -> void { fSiPMQuantumEfficiency = std::move(val); }

    auto SiPMOpticalPhotonCountThreshold(int val) -> void { fSiPMOpticalPhotonCountThreshold = val; }
    auto ClusterLength(int val) -> void { fClusterLength = val; }
    auto ThresholdTime(double val) -> void { fThresholdTime = val; }
    auto TimeWindow(double val) -> void { fTimeWindow = val; }
    auto SiPMDeadTime(double val) -> void { fSiPMDeadTime = val; }
    auto CentroidThetaThreshold(double val) -> void { fCentroidThetaThreshold = val; }
    auto CentroidZThreshold(double val) -> void { fCentroidZThreshold = val; }

public:
    struct LayerConfiguration {
        struct FiberConfiguration {
            std::string layerType;
            double radius;
            double pitch;
        };
        int firstID;
        int lastID;
        int fiberNumber;
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
    double fTransverseLightGuideLength;
    double fEpoxyThickness;

private:
    auto CalculateLayerConfiguration() const -> std::vector<LayerConfiguration>;
    auto CalculateFiberInformation() const -> std::vector<FiberInformation>;

    auto CalculateLayerPitch() const -> std::vector<double>;
    auto CalculateLayerFiberIDRange() const -> std::vector<std::pair<int, int>>;

    Simple<int> fNLayer;
    Simple<std::vector<std::string>> fLayerType;
    Simple<std::vector<double>> fLayerRadius;

    Simple<std::vector<int>> fNFiberALayer;
    Cached<std::vector<std::pair<int, int>>> fLayerFiberIDRange;

    Cached<std::vector<double>> fLayerPitch;
    Cached<std::vector<LayerConfiguration>> fLayerConfiguration;
    Cached<std::vector<FiberInformation>> fFiberMap;

    double fScintillationYield;
    double fScintillationTimeConstant1;

    std::vector<double> fScintillationWavelengthBin;
    std::vector<double> fScintillationComponent1;
    std::vector<double> fSiPMEnergyBin;
    std::vector<double> fSiPMQuantumEfficiency;

    int fSiPMOpticalPhotonCountThreshold;
    int fClusterLength;
    double fThresholdTime;
    double fTimeWindow;
    double fSiPMDeadTime;
    double fCentroidThetaThreshold;
    double fCentroidZThreshold;
};

} // namespace MACE::PhaseI::Detector::Description
