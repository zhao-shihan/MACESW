#include "MACE/PhaseI/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Angle;

Target::Target() :
    DescriptionBase("Target"),
    fMajorAxisLength{84.85_mm},
    fMinorAxisLength{60_mm},
    fThickness{900_um},
    fInclinationAngle{45_deg} {}

auto Target::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fMajorAxisLength, "MajorAxisLength");
    ImportValue(node, fMinorAxisLength, "MinorAxisLength");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fInclinationAngle, "InclinationAngle");
}

auto Target::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fMajorAxisLength, "MajorAxisLength");
    ExportValue(node, fMinorAxisLength, "MinorAxisLength");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fInclinationAngle, "InclinationAngle");
}

} // namespace MACE::PhaseI::Detector::Description
