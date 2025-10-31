#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include "muc/array"

#include <utility>

namespace MACE::Detector::Description {

class ECALField final : public Mustard::Detector::Description::DescriptionBase<ECALField> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    ECALField();
    ~ECALField() override = default;

public:
    ///////////////////////////////////////////////////////////
    // Geometry
    ///////////////////////////////////////////////////////////

    auto Radius() const -> auto { return fRadius; }
    auto Length() const -> auto { return fLength; }

    auto Radius(double v) -> void { fRadius = v; }
    auto Length(double v) -> void { fLength = v; }

    auto Center() const -> muc::array3d;

    ///////////////////////////////////////////////////////////
    // Field
    ///////////////////////////////////////////////////////////

    auto FastField() const -> auto { return fFastField; }

    auto FastField(double val) -> void { fFastField = val; }

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
