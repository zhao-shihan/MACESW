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
