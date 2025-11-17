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

#include "MACE/Reconstruction/MMSTracking/Field/GenFitMMSField.h++"

#include "Mustard/Utility/ConvertG3G4Unit.h++"

#include "muc/array"

namespace MACE::inline Reconstruction::MMSTracking::inline Field {

auto GenFitMMSField::get(const TVector3& x) const -> TVector3 {
    TVector3 b;
    get(x[0], x[1], x[2], b[0], b[1], b[2]);
    return b;
}

auto GenFitMMSField::get(const double& x, const double& y, const double& z, double& bx, double& by, double& bz) const -> void {
    const auto b{fMMSField.B<muc::array3d>({x, y, z})};
    bx = Mustard::ToG3<"Magnetic field">(b[0]);
    by = Mustard::ToG3<"Magnetic field">(b[1]);
    bz = Mustard::ToG3<"Magnetic field">(b[2]);
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Field
