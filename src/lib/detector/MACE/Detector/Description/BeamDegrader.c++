#include "MACE/Detector/Description/BeamDegrader.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "muc/utility"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

BeamDegrader::BeamDegrader() :
    DescriptionBase{"BeamDegrader"},
    fEnabled{true},
    // Geometry
    fWidth{10_cm},
    fHeight{5_cm},
    fThickness{270_um},
    fDistanceToTarget{5_mm},
    // Material
    fMaterialName{"G4_MYLAR"} {}

auto BeamDegrader::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fEnabled, "Enabled");
    // Geometry
    ImportValue(node, fWidth, "Width");
    ImportValue(node, fHeight, "Height");
    ImportValue(node, fThickness, "Thickness");
    ImportValue(node, fDistanceToTarget, "DistanceToTarget");
    // Material
    ImportValue(node, fMaterialName, "MaterialName");
}

auto BeamDegrader::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fEnabled, "Enabled");
    // Geometry
    ExportValue(node, fWidth, "Width");
    ExportValue(node, fHeight, "Height");
    ExportValue(node, fThickness, "Thickness");
    ExportValue(node, fDistanceToTarget, "DistanceToTarget");
    // Material
    ExportValue(node, fMaterialName, "MaterialName");
}

} // namespace MACE::Detector::Description
