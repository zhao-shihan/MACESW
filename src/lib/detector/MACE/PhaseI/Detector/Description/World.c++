#include "MACE/PhaseI/Detector/Description/World.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

World::World() :
    DescriptionBase{"World"},
    fXExtent{2_m},
    fYExtent{2_m},
    fZExtent{2_m} {}

auto World::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fXExtent, "XExtent");
    ImportValue(node, fYExtent, "YExtent");
    ImportValue(node, fZExtent, "ZExtent");
}

auto World::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fXExtent, "XExtent");
    ExportValue(node, fYExtent, "YExtent");
    ExportValue(node, fZExtent, "ZExtent");
}

} // namespace MACE::PhaseI::Detector::Description
