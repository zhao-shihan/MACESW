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
#include "MACE/Detector/Field/SolenoidFieldT1.h++"

namespace MACE::Detector::Field {

SolenoidFieldT1::SolenoidFieldT1() : // clang-format off
    MagneticFieldBase<SolenoidFieldT1>{}, // clang-format on
    fField{FastField{{}, {}, {}, {}}} {
    const auto& fieldOption{Detector::Description::FieldOption::Instance()};
    const auto& solenoid{Detector::Description::Solenoid::Instance()};
    if (fieldOption.UseFast()) { // clang-format off
        fField = FastField{solenoid.FastField(), solenoid.T1Radius(), solenoid.T1Center(), {0, 0, 1}}; // clang-format on
    } else {
        fField = FieldMap{fieldOption.ParsedFieldDataFilePath().generic_string(), "SolenoidFieldT1"};
    }
}

} // namespace MACE::Detector::Field
