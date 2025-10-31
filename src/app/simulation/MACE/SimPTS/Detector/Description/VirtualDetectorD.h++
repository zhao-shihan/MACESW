#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::SimPTS::Detector::Description {

class VirtualDetectorD final : public Mustard::Detector::Description::DescriptionBase<VirtualDetectorD> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    VirtualDetectorD();
    ~VirtualDetectorD() override = default;

public:
    auto Thickness() const -> auto { return fThickness; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fThickness;
};

} // namespace MACE::SimPTS::Detector::Description
