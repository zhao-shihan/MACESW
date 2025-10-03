#pragma once

#include "MACE/Detector/Assembly/MMS.h++"
#include "MACE/Detector/Description/World.h++"
#include "MACE/Simulation/Messenger/NumericMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4VUserDetectorConstruction.hh"

#include "muc/tuple"

#include <memory>

namespace Mustard::Detector::Definition {
class DefinitionBase;
} // namespace Mustard::Detector::Definition

namespace MACE::SimMMS::inline Action {

class DetectorConstruction final : public Mustard::Env::Memory::PassiveSingleton<DetectorConstruction>,
                                   public G4VUserDetectorConstruction {
public:
    DetectorConstruction();

    auto SetCheckOverlaps(G4bool checkOverlaps) -> void { fCheckOverlap = checkOverlaps; }

    auto MinDriverStep(double val) -> void { fMinDriverStep = val; }
    auto DeltaChord(double val) -> void { fDeltaChord = val; }

    auto Construct() -> G4VPhysicalVolume* override;

public:
    using ProminentDescription = muc::tuple_unique_t<muc::tuple_concat_t<
        Detector::Assembly::MMS::ProminentDescription,
        std::tuple<Detector::Description::World>>>;

private:
    G4bool fCheckOverlap;

    double fMinDriverStep;
    double fDeltaChord;

    std::unique_ptr<Mustard::Detector::Definition::DefinitionBase> fWorld;

    NumericMessenger<DetectorConstruction>::Register<DetectorConstruction> fNumericMessengerRegister;
};

} // namespace MACE::SimMMS::inline Action
