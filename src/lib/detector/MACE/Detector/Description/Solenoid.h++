#pragma once

#include "MACE/Detector/Description/MMSField.h++"

#include "Mustard/Detector/Description/DescriptionBase.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "muc/array"

#include <string>
#include <utility>

namespace MACE::Detector::Description {

using namespace Mustard::VectorArithmeticOperator;

class Solenoid final : public Mustard::Detector::Description::DescriptionBase<Solenoid> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Solenoid();
    ~Solenoid() override = default;

public:
    // Geometry

    auto S1Length() const -> auto { return fS1Length; }
    auto T1Radius() const -> auto { return fT1Radius; }
    auto S2Length() const -> auto { return fS2Length; }
    auto T2Radius() const -> auto { return fT2Radius; }
    auto S3Length() const -> auto { return fS3Length; }
    auto InnerRadius() const -> auto { return fInnerRadius; }
    auto OuterRadius() const -> auto { return fOuterRadius; }
    auto CoilThickness() const -> auto { return fCoilThickness; }
    auto ReferenceCoilSpacing() const -> auto { return fReferenceCoilSpacing; }
    auto FieldRadius() const -> auto { return fFieldRadius; }

    auto S1Length(double val) -> void { fS1Length = val; }
    auto T1Radius(double val) -> void { fT1Radius = val; }
    auto S2Length(double val) -> void { fS2Length = val; }
    auto T2Radius(double val) -> void { fT2Radius = val; }
    auto S3Length(double val) -> void { fS3Length = val; }
    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto OuterRadius(double val) -> void { fOuterRadius = val; }
    auto CoilThickness(double val) -> void { fCoilThickness = val; }
    auto ReferenceCoilSpacing(double val) -> void { fReferenceCoilSpacing = val; }
    auto FieldRadius(double val) -> void { fFieldRadius = val; }

    auto S1Center() const -> muc::array3d { return {0, 0, (MMSField::Instance().Length() + fS1Length) / 2}; }
    auto T1Center() const -> muc::array3d { return S1Center() + muc::array3d{fT1Radius, 0, fS1Length / 2}; }
    auto S2Center() const -> muc::array3d { return T1Center() + muc::array3d{fS2Length / 2, 0, fT1Radius}; }
    auto T2Center() const -> muc::array3d { return S2Center() + muc::array3d{fS2Length / 2, 0, fT2Radius}; }
    auto S3Center() const -> muc::array3d { return T2Center() + muc::array3d{fT2Radius, 0, fS3Length / 2}; }

    // Material

    auto MaterialName() const -> const auto& { return fMaterialName; }

    auto MaterialName(std::string val) -> void { fMaterialName = std::move(val); }

    // Field

    auto FastField() const -> auto { return fFastField; }

    auto FastField(double val) -> void { fFastField = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fS1Length;
    double fT1Radius;
    double fS2Length;
    double fT2Radius;
    double fS3Length;
    double fInnerRadius;
    double fOuterRadius;
    double fCoilThickness;
    double fReferenceCoilSpacing;
    double fFieldRadius;

    // Material

    std::string fMaterialName;

    // Field

    double fFastField;
};

} // namespace MACE::Detector::Description
