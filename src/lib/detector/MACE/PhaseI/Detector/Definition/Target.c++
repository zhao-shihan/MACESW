#include "MACE/PhaseI/Detector/Definition/Target.h++"
#include "MACE/PhaseI/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "G4EllipticalTube.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

#include <cmath>

namespace MACE::PhaseI::Detector::Definition {

using namespace Mustard::LiteralUnit;
using namespace Mustard::PhysicalConstant;

auto Target::Construct(G4bool checkOverlaps) -> void {
    const auto& target{Description::Target::Instance()};
    const auto semiMajorAxis{target.ProjectionRadius() / std::cos(target.InclinationAngle())};

    const auto solid{Make<G4EllipticalTube>(
        target.Name(),
        target.ProjectionRadius(),
        semiMajorAxis,
        target.Thickness() / 2)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_MYLAR"),
        target.Name())};
    Make<G4PVPlacement>(
        G4RotateX3D{target.InclinationAngle()},
        logic,
        target.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::PhaseI::Detector::Definition
