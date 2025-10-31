#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::Detector::Description {

class World final : public Mustard::Detector::Description::DescriptionBase<World> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    World();
    ~World() override = default;

public:
    auto HalfXExtent() const -> auto { return fHalfXExtent; }
    auto HalfYExtent() const -> auto { return fHalfYExtent; }
    auto HalfZExtent() const -> auto { return fHalfZExtent; }

    auto HalfXExtent(double val) -> void { fHalfXExtent = val; }
    auto HalfYExtent(double val) -> void { fHalfYExtent = val; }
    auto HalfZExtent(double val) -> void { fHalfZExtent = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fHalfXExtent;
    double fHalfYExtent;
    double fHalfZExtent;
};

} // namespace MACE::Detector::Description
