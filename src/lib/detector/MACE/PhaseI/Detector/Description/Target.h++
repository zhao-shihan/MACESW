#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <string>
#include <utility>

namespace MACE::PhaseI::Detector::Description {

class Target final : public Mustard::Detector::Description::DescriptionBase<Target> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Target();
    ~Target() override = default;

public:
    auto MajorAxisLength() const -> auto { return fMajorAxisLength; }
    auto MinorAxisLength() const -> auto { return fMinorAxisLength; }
    auto Thickness() const -> auto { return fThickness; }
    auto InclinationAngle() const -> auto { return fInclinationAngle; }

    auto MajorAxisLength(double val) -> void { fMajorAxisLength = val; }
    auto MinorAxisLength(double val) -> void { fMinorAxisLength = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto InclinationAngle(double val) -> void { fInclinationAngle = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fMajorAxisLength;
    double fMinorAxisLength;
    double fThickness;
    double fInclinationAngle;
};

} // namespace MACE::PhaseI::Detector::Description
