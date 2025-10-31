#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::Detector::Description {

class MMSField final : public Mustard::Detector::Description::DescriptionBase<MMSField> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MMSField();
    ~MMSField() override = default;

public:
    ///////////////////////////////////////////////////////////
    // Geometry
    ///////////////////////////////////////////////////////////

    auto Radius() const -> auto { return fRadius; }
    auto Length() const -> auto { return fLength; }

    auto Radius(double val) -> void { fRadius = val; }
    auto Length(double val) -> void { fLength = val; }

    ///////////////////////////////////////////////////////////
    // Field
    ///////////////////////////////////////////////////////////

    auto FastField() const -> auto { return fFastField; }

    auto FastField(double v) -> void { fFastField = v; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    ///////////////////////////////////////////////////////////
    // Geometry
    ///////////////////////////////////////////////////////////

    double fRadius;
    double fLength;

    ///////////////////////////////////////////////////////////
    // Field
    ///////////////////////////////////////////////////////////

    double fFastField;
};

} // namespace MACE::Detector::Description
