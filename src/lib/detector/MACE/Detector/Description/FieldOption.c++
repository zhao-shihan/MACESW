// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

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
