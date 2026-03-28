/*
   This file is part of GENFIT.

   GENFIT is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   GENFIT is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with GENFIT.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @addtogroup genfit
 * @{
 */

#pragma once

#include "AbsHMatrix.h"
#include "AbsMeasurement.h"
#include "MeasurementOnPlane.h"

#include "TVector3.h"

namespace genfit {

/** @brief Class for measurements in helical detectors (like helical trackers or detectors)
 *  which measure the drift distance from a helical wire.
 *
 * This hit class is designed for helical-shaped detectors where measurements
 * are the drift distance from a helical wire. The hit is described by 7 coordinates:
 * h_x, h_y, h_z, radius, pitch, phi0, rdrift
 * where:
 * - h_x, h_y, h_z: center coordinates of the helix
 * - radius: radius of the helix
 * - pitch: pitch length (z-distance per full turn)
 * - phi0: initial phase angle (in radians)
 * - rdrift: drift distance from the helix wire
 *
 * The measurement plane is dynamically constructed at the point of closest approach
 * to the helix, with the V-axis tangent to the helix and the U-axis perpendicular.
 */
class HelixMeasurement : public AbsMeasurement {

public:
    HelixMeasurement(int nDim = 8);
    HelixMeasurement(const TVectorD& rawHitCoords, const TMatrixDSym& rawHitCov,
                     int detId, int hitId, TrackPoint* trackPoint);

    virtual ~HelixMeasurement() { ; }

    virtual AbsMeasurement* clone() const override { return new HelixMeasurement(*this); }

    virtual SharedPlanePtr constructPlane(const StateOnPlane& state) const override;

    /**  Hits with a small drift distance get a higher weight, whereas hits with
     * big drift distances become weighted down.
     * When these initial weights are used by the DAF, the smoothed track will be closer to the real
     * trajectory than if both sides are weighted with 0.5 regardless of the drift distance.
     * This helps a lot when resolving l/r ambiguities with the DAF.
     * The idea is that for the first iteration of the DAF, the wire positions are taken.
     * For small drift radii, the wire position does not bend the fit away from the
     * trajectory, whereas the wire position for hits with large drift radii is further away
     * from the trajectory and will therefore bias the fit if not weighted down.
     */
    virtual std::vector<MeasurementOnPlane*> constructMeasurementsOnPlane(const StateOnPlane& state) const override;

    virtual const AbsHMatrix* constructHMatrix(const AbsTrackRep*) const override;

protected:
    struct ClosestPointResult {
        TVector3 point;   ///< 螺旋线上最近点
        TVector3 tangent; ///< 该点的切线方向
    };
    ClosestPointResult findClosestPointOnHelix(const TVector3& point) const;

// public:
//     ClassDefOverride(genfit::HelixMeasurement, 1);
};

} /* End of namespace genfit */
// genfit_HelixMeasurement_h

