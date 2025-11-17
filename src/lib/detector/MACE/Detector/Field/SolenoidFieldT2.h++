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

#include "Mustard/Concept/NumericVector.h++"
#include "Mustard/Detector/Field/MagneticFieldBase.h++"
#include "Mustard/Detector/Field/MagneticFieldMap.h++"
#include "Mustard/Detector/Field/ToroidField.h++"

#include <variant>

namespace MACE::Detector::Field {

class SolenoidFieldT2 : public Mustard::Detector::Field::MagneticFieldBase<SolenoidFieldT2> {
public:
    SolenoidFieldT2();

    template<Mustard::Concept::NumericVector3D T> // clang-format off
    auto B(T x) const -> T { return std::visit([&x](auto&& f) { return f.B(x); }, fField); } // clang-format on

private:
    using FastField = Mustard::Detector::Field::ToroidField;
    using FieldMap = Mustard::Detector::Field::MagneticFieldMapSymmetryY<>;

private:
    std::variant<FastField, FieldMap> fField;
};

} // namespace MACE::Detector::Field
