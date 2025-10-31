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
