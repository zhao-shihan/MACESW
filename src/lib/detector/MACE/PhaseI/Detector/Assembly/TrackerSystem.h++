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

#pragma once

#include "MACE/Detector/Description/FieldOption.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Description/TTC.h++"

#include "Mustard/Detector/Assembly/AssemblyBase.h++"

namespace MACE::PhaseI::Detector::Assembly {

class TrackerSystem : public Mustard::Detector::Assembly::AssemblyBase {
public:
    using DescriptionInUse = std::tuple<MACE::PhaseI::Detector::Description::SciFiTracker,
                                        MACE::PhaseI::Detector::Description::TTC>;

public:
    TrackerSystem(Mustard::Detector::Definition::DefinitionBase& mother, bool checkOverlap);
};

} // namespace MACE::PhaseI::Detector::Assembly
