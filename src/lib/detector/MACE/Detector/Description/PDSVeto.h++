#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <vector>
namespace MACE::Detector::Description {

class PDSVeto final : public Mustard::Detector::Description::DescriptionBase<PDSVeto> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    PDSVeto();
    ~PDSVeto() = default;

public:
    // Geometry

    auto TopPSLength() const -> const auto& { return fTopPSLength; }
    auto BottomPSLength() const -> const auto& { return fBottomPSLength; }
    auto SidePSLength() const -> const auto& { return fSidePSLength; }
    auto Cap1PSLength() const -> const auto& { return fCap1PSLength; }
    auto Cap2PSLength() const -> const auto& { return fCap2PSLength; }
    auto PSWidth() const -> const auto& { return fPSWidth; }
    auto PSThickness() const -> const auto& { return fPSThickness; }
    auto PSFiberRadius() const -> const auto& { return fPSFiberRadius; }
    auto PSFiberCurvatureRadius() const -> const auto& { return fPSFiberCurvatureRadius; }
    auto PSHoleRadius() const -> const auto& { return fPSHoleRadius; }
    auto InterPSGap() const -> const auto& { return fInterPSGap; }
    auto InterModuleGap() const -> const auto& { return fInterModuleGap; }
    auto Cap12Gap() const -> const auto& { return fCap12Gap; }
    auto ModuleOffset() const -> const auto& { return fModuleOffset; }
    auto AlAbsorberThickness() const -> const auto& { return fAlAbsorberThickness; }

    auto TopLayer() const -> const auto& { return fTopLayer; }
    auto SideLayer() const -> const auto& { return fSideLayer; }
    auto BottomLayer() const -> const auto& { return fBottomLayer; }
    auto CapLayer() const -> const auto& { return fCapLayer; }

    auto SiPMThickness() const -> const auto& { return fSiPMThickness; }
    auto SiPMCathodeThickness() const -> const auto& { return fSiPMCathodeThickness; }
    auto SiPMCouplerThickness() const -> const auto& { return fSiPMCouplerThickness; }
    auto SiPMSize() const -> const auto& { return fSiPMSize; }
    // Material

    auto PSMaterialName() const -> const auto& { return fPSMaterialName; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fTopPSLength;
    double fBottomPSLength;
    double fSidePSLength;
    double fCap1PSLength;
    double fCap2PSLength;
    // CRV-T&B and CRV-C1 135cm, CRV-S 125cm, CRV-C2 35cm
    double fPSWidth;                // 10cm
    double fPSThickness;            // 2cm
    double fPSFiberRadius;          // 0.7mm
    double fPSHoleRadius;           //>0.7mm
    double fPSFiberCurvatureRadius; // 61cm finely caculated, do not change arbitrarily
    double fInterPSGap;             // 0.3cm
    double fInterModuleGap;
    double fCap12Gap;
    double fModuleOffset;        // 1cm
    double fAlAbsorberThickness; // 0.6cm
    int fTopLayer;
    int fSideLayer;
    int fBottomLayer;
    int fCapLayer;

    double fSiPMThickness;
    double fSiPMCathodeThickness;
    double fSiPMCouplerThickness;
    double fSiPMSize;

    // Material

    std::string fPSMaterialName;
};
} // namespace MACE::Detector::Description