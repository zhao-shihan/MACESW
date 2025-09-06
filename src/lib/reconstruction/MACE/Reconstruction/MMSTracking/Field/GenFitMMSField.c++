#include "MACE/Reconstruction/MMSTracking/Field/GenFitMMSField.h++"

#include "Mustard/Utility/ConvertG3G4Unit.h++"

#include "muc/array"

namespace MACE::inline Reconstruction::MMSTracking::inline Field {

auto GenFitMMSField::get(const TVector3& x) const -> TVector3 {
    TVector3 B;
    get(x[0], x[1], x[2], B[0], B[1], B[2]);
    return B;
}

auto GenFitMMSField::get(const double& x, const double& y, const double& z, double& Bx, double& By, double& Bz) const -> void {
    const auto B{fMMSField.B<muc::array3d>({x, y, z})};
    Bx = Mustard::ToG3<"Magnetic field">(B[0]);
    By = Mustard::ToG3<"Magnetic field">(B[1]);
    Bz = Mustard::ToG3<"Magnetic field">(B[2]);
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Field
