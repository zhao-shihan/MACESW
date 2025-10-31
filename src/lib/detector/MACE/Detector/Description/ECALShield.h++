#pragma once

#include "MACE/Detector/Description/ECALField.h++"

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <string>
#include <utility>

namespace MACE::Detector::Description {

class ECALShield final : public Mustard::Detector::Description::DescriptionBase<ECALShield> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    ECALShield();
    ~ECALShield() override = default;

public:
    // Geometry

    auto InnerRadius() const -> const auto& { return fInnerRadius; }
    auto InnerLength() const -> const auto& { return fInnerLength; }
    auto GapAroundWindow() const -> const auto& { return fGapAroundWindow; }
    auto Thickness() const -> const auto& { return fThickness; }

    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto InnerLength(double val) -> void { fInnerLength = val; }
    auto GapAroundWindow(double val) -> void { fGapAroundWindow = val; }
    auto Thickness(double val) -> void { fThickness = val; }

    // Material

    auto MaterialName() const -> const auto& { return fMaterialName; }

    auto MaterialName(std::string val) -> void { fMaterialName = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fInnerRadius;
    double fInnerLength;
    double fGapAroundWindow;
    double fThickness;

    // Material

    std::string fMaterialName;
};

} // namespace MACE::Detector::Description
