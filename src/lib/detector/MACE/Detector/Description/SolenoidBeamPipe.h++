#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <string>
#include <utility>

namespace MACE::Detector::Description {

class SolenoidBeamPipe final : public Mustard::Detector::Description::DescriptionBase<SolenoidBeamPipe> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    SolenoidBeamPipe();
    ~SolenoidBeamPipe() override = default;

public:
    // Geometry

    auto InnerRadius() const -> auto { return fInnerRadius; }
    auto Thickness() const -> auto { return fThickness; }

    auto InnerRadius(double val) -> void { fInnerRadius = val; }
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
    double fThickness;

    // Material

    std::string fMaterialName;
};

} // namespace MACE::Detector::Description
