#include "MACE/PhaseI/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Angle;

Target::Target() :
    DescriptionBase("Target"),
    fProjectionRadius{30_mm},
    fThickness{900_um},
    fInclinationAngle{45_deg},
    fMeanFreePath{0},
    fFormationProbability{0.5} {}

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
