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
#include "MACE/Detector/Description/Solenoid.h++"
#include "MACE/Detector/Field/SolenoidFieldS2.h++"

namespace MACE::Detector::Field {

SolenoidFieldS2::SolenoidFieldS2() : // clang-format off
    MagneticFieldBase<SolenoidFieldS2>{}, // clang-format on
    fField{FastField{{}}} {
    const auto& fieldOption{Detector::Description::FieldOption::Instance()};
    const auto& solenoid{Detector::Description::Solenoid::Instance()};
    if (fieldOption.UseFast()) {
        fField = FastField{solenoid.FastField(), 0, 0};
    } else {
        fField = FieldMap{fieldOption.ParsedFieldDataFilePath().generic_string(), "SolenoidFieldS2"};
    }
}

} // namespace MACE::Detector::Field
