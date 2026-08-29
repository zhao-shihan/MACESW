#include "MACE/PhaseI/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "muc/math"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Angle;

Target::Target() :
    DescriptionBase("Target"),
    fProjectionRadius{30_mm},
    fThickness{900_um},
    fInclinationAngle{45_deg},
    fMeanFreePath{0_nm},
    fFormationProbability{0} {}

auto Target::VolumeContain(Mustard::Point3D x) const -> bool {
    const auto alpha{fInclinationAngle};
    const auto [sinAlpha, cosAlpha]{muc::sincos(alpha)};

    const auto xLocal{x[0]};
    const auto yLocal{cosAlpha * x[1] + sinAlpha * x[2]};
    const auto zLocal{-sinAlpha * x[1] + cosAlpha * x[2]};
    const auto semiMajorAxis{fProjectionRadius / cosAlpha};

    return std::abs(zLocal) <= fThickness / 2 and
           muc::pow(xLocal / fProjectionRadius, 2) + muc::pow(yLocal / semiMajorAxis, 2) <= 1;
}

auto Target::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fProjectionRadius, "ProjectionRadius");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fInclinationAngle, "InclinationAngle");
    ImportValue(node, fMeanFreePath, "MeanFreePath");
    ImportValue(node, fFormationProbability, "FormationProbability");
}

auto Target::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fProjectionRadius, "ProjectionRadius");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fInclinationAngle, "InclinationAngle");
    ExportValue(node, fMeanFreePath, "MeanFreePath");
    ExportValue(node, fFormationProbability, "FormationProbability");
}

} // namespace MACE::PhaseI::Detector::Description
