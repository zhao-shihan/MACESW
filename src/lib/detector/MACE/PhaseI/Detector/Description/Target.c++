#include "MACE/PhaseI/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Angle;

Target::Target() :
    DescriptionBase("Target"),
    fProjectionRadius{30_mm},
    fThickness{900_um},
    fInclinationAngle{45_deg} {}

auto Target::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fProjectionRadius, "ProjectionRadius");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fInclinationAngle, "InclinationAngle");
}

auto Target::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fProjectionRadius, "ProjectionRadius");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fInclinationAngle, "InclinationAngle");
}

} // namespace MACE::PhaseI::Detector::Description
