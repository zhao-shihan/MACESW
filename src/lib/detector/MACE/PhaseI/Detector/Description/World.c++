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
