#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <utility>
#include <vector>

namespace MACE::Detector::Description {

class MCP final : public Mustard::Detector::Description::DescriptionBase<MCP> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MCP();
    ~MCP() override = default;

public:
    // Geometry

    auto Diameter() const -> auto { return fDiameter; }
    auto Thickness() const -> auto { return fThickness; }
    auto AnodeDistance() const -> auto { return fAnodeDistance; }
    auto AnodeThickness() const -> auto { return fAnodeThickness; }

    auto Diameter(double val) -> void { fDiameter = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto AnodeDistance(double val) -> void { fAnodeDistance = val; }
    auto AnodeThickness(double val) -> void { fAnodeThickness = val; }

    // Detection

    auto TimeResolutionFWHM() const -> auto { return fTimeResolutionFWHM; }
    auto EfficiencyEnergy() const -> const auto& { return fEfficiencyEnergy; }
    auto EfficiencyValue() const -> const auto& { return fEfficiencyValue; }

    auto TimeResolutionFWHM(double val) -> void { fTimeResolutionFWHM = val; }
    auto EfficiencyEnergy(std::vector<double> val) -> void { fEfficiencyEnergy = std::move(val); }
    auto EfficiencyValue(std::vector<double> val) -> void { fEfficiencyValue = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fDiameter;
    double fThickness;
    double fAnodeDistance;
    double fAnodeThickness;

    // Detection

    double fTimeResolutionFWHM;
    std::vector<double> fEfficiencyEnergy;
    std::vector<double> fEfficiencyValue;
};

} // namespace MACE::Detector::Description
