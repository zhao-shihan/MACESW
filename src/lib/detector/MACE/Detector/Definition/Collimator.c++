#include "MACE/Detector/Definition/Collimator.h++"
#include "MACE/Detector/Description/Collimator.h++"
#include "MACE/Detector/Description/SolenoidBeamPipe.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "CLHEP/Vector/RotationZ.h"

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

#include "muc/math"

#include <cmath>

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::Angle;
using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto Collimator::Enabled() const -> bool {
    return Description::Collimator::Instance().Enabled();
}

auto Collimator::Construct(G4bool checkOverlaps) -> void {
    const auto& collimator{Description::Collimator::Instance()};
    const auto& beamPipe{Description::SolenoidBeamPipe::Instance()};

    const auto x0{-(collimator.Count() - 1) * collimator.Pitch() / 2};
    for (int k{}; k < collimator.Count(); ++k) {
        const auto x{x0 + k * collimator.Pitch()};
        const auto halfWidth{std::sqrt(muc::pow(collimator.Radius(), 2) - muc::pow(x, 2))};

        const auto solid{Make<G4Box>(
            collimator.Name(),
            collimator.Thickness() / 2,
            halfWidth,
            collimator.Length() / 2)};

        const auto logic{Make<G4LogicalVolume>(
            solid,
            G4NistManager::Instance()->FindOrBuildMaterial(collimator.MaterialName()),
            collimator.Name())};

        Make<G4PVPlacement>( // clang-format off
            G4Transform3D{{}, {x, 0, 0}}, // clang-format on
            logic,
            collimator.Name(),
            Mother().LogicalVolume(beamPipe.Name() + "S2Vacuum"),
            false,
            k,
            checkOverlaps);
    }
}

} // namespace MACE::Detector::Definition
