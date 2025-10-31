#include "MACE/Detector/Description/FieldOption.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

FieldOption::FieldOption() :
    DescriptionBase{"FieldOption"},
    fUseFast{false},
    fFieldDataFilePath{"${MACESW_DATA_DIR}/em_field_data.root"} {}

auto FieldOption::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fUseFast, "UseFast");
    ImportValue(node, fFieldDataFilePath, "FieldDataFilePath");
}

auto FieldOption::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fUseFast, "UseFast");
    ExportValue(node, fFieldDataFilePath, "FieldDataFilePath");
}

} // namespace MACE::Detector::Description
