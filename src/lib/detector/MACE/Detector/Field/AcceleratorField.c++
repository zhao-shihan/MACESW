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

#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/FieldOption.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Field/AcceleratorField.h++"

namespace MACE::Detector::Field {

AcceleratorField::AcceleratorField() : // clang-format off
    ElectromagneticFieldBase<AcceleratorField>{}, // clang-format on
    fField{} {
    const auto& fieldOption{Detector::Description::FieldOption::Instance()};
    if (not fieldOption.UseFast()) {
        fField = FieldMap{fieldOption.ParsedFieldDataFilePath().generic_string(), "AcceleratorField"};
    }
}

AcceleratorField::FastField::FastField() :
    fB{Detector::Description::MMSField::Instance().FastField()} {
    const auto& accelerator{Description::Accelerator::Instance()};
    fZ0 = accelerator.MaxPotentialPosition();
    fE1 = -accelerator.MaxPotential() / accelerator.DecelerateFieldLength();
    fE2 = accelerator.MaxPotential() / accelerator.AccelerateFieldLength();
}

} // namespace MACE::Detector::Field
