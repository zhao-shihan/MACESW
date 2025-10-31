#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::Detector::Description {

class ShieldingWall final : public Mustard::Detector::Description::DescriptionBase<ShieldingWall> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    ShieldingWall();
    ~ShieldingWall() override = default;

public:
    auto Enabled() const -> auto { return fEnabled; }
    auto Thickness() const -> auto { return fThickness; }
    auto Length() const -> auto { return fLength; }
    auto Rotation() const -> auto { return fRotation; }

    auto Enabled(bool val) -> void { fEnabled = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto Length(double val) -> void { fLength = val; }
    auto Rotation(double val) -> void { fRotation = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    bool fEnabled;
    double fThickness;
    double fLength;
    double fRotation;
};

} // namespace MACE::Detector::Description
