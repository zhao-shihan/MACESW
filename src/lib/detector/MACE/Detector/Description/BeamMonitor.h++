#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::Detector::Description {

class BeamMonitor final : public Mustard::Detector::Description::DescriptionBase<BeamMonitor> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    BeamMonitor();
    ~BeamMonitor() override = default;

public:
    auto Enabled() const -> auto { return fEnabled; }
    auto Width() const -> auto { return fWidth; }
    auto Thickness() const -> auto { return fThickness; }
    auto DistanceToTarget() const -> auto { return fDistanceToTarget; }

    auto Enabled(bool val) -> void { fEnabled = val; }
    auto Width(double val) -> void { fWidth = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto DistanceToTarget(double val) -> void { fDistanceToTarget = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    bool fEnabled;
    double fWidth;
    double fThickness;
    double fDistanceToTarget;
};

} // namespace MACE::Detector::Description
