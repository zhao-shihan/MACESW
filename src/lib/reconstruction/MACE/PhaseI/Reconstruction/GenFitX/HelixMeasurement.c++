/* HelixMeasurement.cpp */
#include "HelixMeasurement.h++"

#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Math/POCA.h++"

#include "DetPlane.h"
#include "Exception.h"
#include "HMatrixU.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "RKTrackRep.h"
#include "StateOnPlane.h"

#include "TBuffer.h"
#include "TClass.h"
#include "TMath.h"

#include "muc/numeric"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace MACE::PhaseI::inline Reconstruction::GenFitX {

HelixMeasurement::HelixMeasurement(int nDim) :
    AbsMeasurement(nDim) {
    assert(nDim == 8);
}

HelixMeasurement::HelixMeasurement(const TVectorD& rawHitCoords, const TMatrixDSym& rawHitCov,
                                   int detId, int hitId, genfit::TrackPoint* trackPoint) :
    AbsMeasurement(rawHitCoords, rawHitCov, detId, hitId, trackPoint) {
    if (rawHitCoords_.GetNrows() != 8) {
        throw genfit::Exception("HelixMeasurement requires 8-dimensional rawHitCoords", __LINE__, __FILE__);
    }
}

auto HelixMeasurement::constructPlane(const genfit::StateOnPlane& state) const -> genfit::SharedPlanePtr {

    const genfit::AbsTrackRep* rep{state.getRep()};
    genfit::StateOnPlane st(state);
    rep->extrapolateToCylinder(st, rawHitCoords_(3));
    TVector3 currentPos{rep->getPos(st)};

    auto closest{FindClosestPointOnHelix(currentPos)};
    const TVector3& pocaOnHelix{closest.point};
    TVector3 tangent{closest.tangent};
    tangent.SetMag(1.0);

    TVector3 dirInPoca{rep->getMom(st)};
    dirInPoca.SetMag(1.0);

    if (std::fabs(tangent.Angle(dirInPoca)) < muc::default_abs_tol<double>) {
        Mustard::Throw<std::runtime_error>(
            "HelixMeasurement::constructPlane: Direction is parallel to helix tangent");
    }

    TVector3 u{dirInPoca.Cross(tangent)};
    if (u.Mag2() < muc::default_abs_tol<double>) {
        u = TVector3(1, 0, 0).Cross(tangent);
        if (u.Mag2() < muc::default_abs_tol<double>) {
            u = TVector3(0, 1, 0).Cross(tangent);
        }
    }
    u.SetMag(1.0);
    auto plane{new genfit::DetPlane(pocaOnHelix, u, tangent)};

    return genfit::SharedPlanePtr(plane);
}

auto HelixMeasurement::constructMeasurementsOnPlane(const genfit::StateOnPlane& state) const -> std::vector<genfit::MeasurementOnPlane*> {
    double d{};
    double v{rawHitCov_(7, 7)};
    return {new genfit::MeasurementOnPlane(
        TVectorD(1, &d),
        TMatrixDSym(1, &v),
        state.getPlane(),
        state.getRep(),
        constructHMatrix(state.getRep()))};
}

auto HelixMeasurement::constructHMatrix(const genfit::AbsTrackRep* rep) const -> const genfit::AbsHMatrix* {
    if (dynamic_cast<const genfit::RKTrackRep*>(rep) == nullptr) {
        Mustard::Throw<std::runtime_error>("HelixMeasurement can only handle state vectors of type RKTrackRep!");
    }
    return new genfit::HMatrixU();
}

auto HelixMeasurement::FindClosestPointOnHelix(const TVector3& point) const -> HelixMeasurement::ClosestPointResult {
    ClosestPointResult result;

    const Mustard::Point2D center2{rawHitCoords_(0), rawHitCoords_(1)};
    const TVector3 center3(rawHitCoords_(0), rawHitCoords_(1), rawHitCoords_(2));
    const double radius{rawHitCoords_(3)};
    const double pitchAngle{rawHitCoords_(4)}; // as returned by SciFiTracker::CalculateLayerPitch()
    const double phi0{rawHitCoords_(5)};
    const double phiTotal{rawHitCoords_(6)};

    const double tanA{std::tan(pitchAngle)};      // tan(pitch)
    const double tanAR{radius * tanA};            // r * tan(pitch)
    const double zOffset{phiTotal / 2.0 * tanAR}; // matches original parametrization

    // z0 is the z coordinate at phi = phi0
    const double z0{center3.Z() - zOffset};

    // Mustard::Helix expects 'lambda' (dip angle). Relationship: lambda = pi/2 - pitchAngle
    const double lambda{std::acos(-1.0) / 2.0 - pitchAngle};

    const Mustard::Helix helix{center2, radius, phi0, z0, lambda};

    const Mustard::Point3D targetPoint{point.X(), point.Y(), point.Z()};

    const auto pocaResult{POCA(helix, targetPoint, 0.0, phiTotal,
                               1, 300, muc::default_abs_tol<double>, muc::default_rel_tol<double>)};

    if (not pocaResult.has_value()) {
        Mustard::Throw<std::runtime_error>(
            "HelixMeasurement::findClosestPointOnHelix: Failed to find POCA");
    }

    const auto& [poca, doca]{*pocaResult};
    result.point = TVector3(poca.x(), poca.y(), poca.z());

    // Compute azimuthal angle (absolute) and convert to helix-relative phi
    const double phiAbs{std::atan2(poca.y() - center2.y(), poca.x() - center2.x())};
    const double phiRel{phiAbs - phi0};

    const auto dir{helix.DirectionAt(phiRel)}; // Mustard::Vector3D
    result.tangent.SetXYZ(dir.x(), dir.y(), dir.z());
    result.tangent = result.tangent.Unit();

    return result;
}

} // namespace MACE::PhaseI::inline Reconstruction::GenFitX
