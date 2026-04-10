#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::PhaseI::Detector::Description {

class Target final : public Mustard::Detector::Description::DescriptionBase<Target> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Target();
    ~Target() override = default;

public:
    auto ProjectionRadius() const -> auto { return fProjectionRadius; }
    auto Thickness() const -> auto { return fThickness; }
    auto InclinationAngle() const -> auto { return fInclinationAngle; }
    auto MeanFreePath() const -> auto { return fMeanFreePath; }
    auto FormationProbability() const -> auto { return fFormationProbability; }

    auto ProjectionRadius(double val) -> void { fProjectionRadius = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto InclinationAngle(double val) -> void { fInclinationAngle = val; }
    auto MeanFreePath(double val) -> void { fMeanFreePath = val; }
    auto FormationProbability(double val) -> void { fFormationProbability = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fProjectionRadius;
    double fThickness;
    double fInclinationAngle;
    double fMeanFreePath;
    double fFormationProbability;
};

} // namespace MACE::PhaseI::Detector::Description
