#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <string>
#include <utility>

namespace MACE::Detector::Description {

class Vacuum final : public Mustard::Detector::Description::DescriptionBase<Vacuum> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Vacuum();
    ~Vacuum() override = default;

public:
    auto Name() const -> auto { return fName; }
    auto Pressure() const -> auto { return fPressure; }
    auto Density() const -> double;

    auto Name(std::string val) -> void { fName = std::move(val); }
    auto Pressure(double val) -> void { fPressure = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    std::string fName;
    double fPressure;
};

} // namespace MACE::Detector::Description
