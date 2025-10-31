#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::Detector::Description {

class MCPChamber final : public Mustard::Detector::Description::DescriptionBase<MCPChamber> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MCPChamber();
    ~MCPChamber() override = default;

public:
    auto InnerRadius() const -> auto { return fInnerRadius; }
    auto Thickness() const -> auto { return fThickness; }

    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto Thickness(double val) -> void { fThickness = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fInnerRadius;
    double fThickness;
};

} // namespace MACE::Detector::Description
