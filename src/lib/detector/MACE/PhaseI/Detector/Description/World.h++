#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::PhaseI::Detector::Description {

class World final : public Mustard::Detector::Description::DescriptionBase<World> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    World();
    ~World() override = default;

public:
    auto XExtent() const -> auto { return fXExtent; }
    auto YExtent() const -> auto { return fYExtent; }
    auto ZExtent() const -> auto { return fZExtent; }

    auto XExtent(double val) -> void { fXExtent = val; }
    auto YExtent(double val) -> void { fYExtent = val; }
    auto ZExtent(double val) -> void { fZExtent = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fXExtent;
    double fYExtent;
    double fZExtent;
};

} // namespace MACE::PhaseI::Detector::Description
