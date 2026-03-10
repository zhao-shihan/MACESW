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

#include "MACE/Detector/Field/MMSField.h++"

#include "AbsBField.h"

#include "TVector3.h"

namespace MACE::inline Reconstruction::MMSTracking::inline Field {

class GenFitMMSField : public genfit::AbsBField {
public:
    auto get(const TVector3& x) const -> TVector3 override;
    auto get(const double& x, const double& y, const double& z, double& bx, double& by, double& bz) const -> void override;

private:
    Detector::Field::MMSField fMMSField;
};

} // namespace MACE::inline Reconstruction::MMSTracking::inline Field
