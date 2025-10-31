#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include <string>
#include <utility>

namespace MACE::PhaseI::Detector::Description {

class CentralBeamPipe final : public Mustard::Detector::Description::DescriptionBase<CentralBeamPipe> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    CentralBeamPipe();
    ~CentralBeamPipe() override = default;

public:
    // Geometry

    auto Length() const -> auto { return fLength; }
    auto InnerRadius() const -> auto { return fInnerRadius; }
    auto BerylliumLength() const -> auto { return fBerylliumLength; }
    auto BerylliumThickness() const -> auto { return fBerylliumThickness; }
    auto AluminiumThickness() const -> auto { return fAluminiumThickness; }

    auto Length(double val) -> void { fLength = val; }
    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto BerylliumLength(double val) -> void { fBerylliumLength = val; }
    auto BerylliumThickness(double val) -> void { fBerylliumThickness = val; }
    auto AluminiumThickness(double val) -> void { fAluminiumThickness = val; }

    // Material

    auto VacuumPressure() const -> auto { return fVacuumPressure; }
    auto VacuumDensity() const -> double;

    auto VacuumPressure(double val) -> void { fVacuumPressure = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fLength;
    double fInnerRadius;
    double fBerylliumLength;
    double fBerylliumThickness;
    double fAluminiumThickness;

    // Material

    double fVacuumPressure;
};

} // namespace MACE::PhaseI::Detector::Description
