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

#include "Mustard/Math/GeometryRepresentation.h++"
#include "Mustard/Math/Vector.h++"

#include "AbsHMatrix.h"
#include "AbsMeasurement.h"
#include "MeasurementOnPlane.h"

#include "TVector3.h"

namespace MACE::PhaseI::inline Reconstruction::GenFitX {

/// @brief Measurements for helical fibers.
///
/// This hit class models measurements from helical fibers. The hit is described
/// by 7 coordinates:
/// x, y, z, radius, pitch, phi0, r
/// where:
/// - x, y, z: center coordinates of the helix
/// - radius: radius of the helix
/// - pitch: pitch length (z-distance per full turn)
/// - phi0: initial phase angle (radians)
/// - r: radius of the helix fiber
///
/// The measurement plane is constructed at the point of closest approach to the
/// helix, with the V-axis tangent to the helix and the U-axis perpendicular.

class HelixMeasurement : public genfit::AbsMeasurement {

public:
    HelixMeasurement(int nDim = 8);
    HelixMeasurement(const TVectorD& rawHitCoords, const TMatrixDSym& rawHitCov,
                     int detId, int hitId, genfit::TrackPoint* trackPoint);

    auto clone() const -> genfit::AbsMeasurement* override { return new HelixMeasurement(*this); }

    auto constructPlane(const genfit::StateOnPlane& state) const -> genfit::SharedPlanePtr override;

    auto constructMeasurementsOnPlane(const genfit::StateOnPlane& state) const -> std::vector<genfit::MeasurementOnPlane*> override;

    auto constructHMatrix(const genfit::AbsTrackRep*) const -> const genfit::AbsHMatrix* override;

protected:
    struct ClosestPointResult {
        TVector3 point;
        TVector3 tangent;
    };
    auto findClosestPointOnHelix(const TVector3& point) const -> ClosestPointResult;
};

} // namespace MACE::PhaseI::inline Reconstruction::GenFitX
