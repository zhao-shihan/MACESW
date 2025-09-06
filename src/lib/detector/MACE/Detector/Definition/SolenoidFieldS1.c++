#include "MACE/Detector/Definition/SolenoidFieldS1.h++"
#include "MACE/Detector/Description/Solenoid.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto SolenoidFieldS1::Construct(G4bool checkOverlaps) -> void {
    const auto& solenoid{Description::Solenoid::Instance()};
    const auto name{solenoid.Name() + "FieldS1"};

    const auto mother{Mother().LogicalVolume()};
    const auto solid{Make<G4Tubs>(
        name,
        0,
        solenoid.FieldRadius(),
        solenoid.S1Length() / 2,
        0,
        2_pi)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        mother->GetMaterial(),
        name)};
    Make<G4PVPlacement>(
        G4Transform3D{{}, Mustard::VectorCast<G4ThreeVector>(solenoid.S1Center())},
        logic,
        name,
        mother,
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
