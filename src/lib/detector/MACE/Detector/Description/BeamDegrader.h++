#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <string>
#include <utility>

namespace MACE::Detector::Description {

class BeamDegrader final : public Mustard::Detector::Description::DescriptionBase<BeamDegrader> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    BeamDegrader();
    ~BeamDegrader() = default;

public:
    auto Enabled() const -> auto { return fEnabled; }

    auto Enabled(bool val) -> void { fEnabled = val; }

    // Geometry

    auto Width() const -> auto { return fWidth; }
    auto Height() const -> auto { return fHeight; }
    auto Thickness() const -> auto { return fThickness; }
    auto DistanceToTarget() const -> auto { return fDistanceToTarget; }

    auto Width(double val) -> void { fWidth = val; }
    auto Height(double val) -> void { fHeight = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto DistanceToTarget(double val) -> void { fDistanceToTarget = val; }

    // Material

    auto MaterialName() const -> const auto& { return fMaterialName; }

    auto MaterialName(std::string val) { fMaterialName = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    bool fEnabled;

    // Geometry

    double fWidth;
    double fHeight;
    double fThickness;
    double fDistanceToTarget;

    // Material

    std::string fMaterialName;
};

} // namespace MACE::Detector::Description
