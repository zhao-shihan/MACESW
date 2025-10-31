#pragma once

#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Detector/Description/Target.h++"
#include "MACE/PhaseI/Detector/Description/CentralBeamPipe.h++"
#include "MACE/PhaseI/Detector/Description/MRPC.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Description/TTC.h++"
#include "MACE/PhaseI/Detector/Description/World.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Region.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/ECALPMSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/ECALSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/MRPCSD.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4VUserDetectorConstruction.hh"

#include <memory>
#include <tuple>

namespace Mustard::Detector::Definition {
class DefinitionBase;
} // namespace Mustard::Detector::Definition

namespace MACE::PhaseI::SimMACEPhaseI::inline Action {

class DetectorConstruction final : public Mustard::Env::Memory::PassiveSingleton<DetectorConstruction>,
                                   public G4VUserDetectorConstruction {
public:
    DetectorConstruction();

    auto Construct() -> G4VPhysicalVolume* override;

    auto SetCheckOverlaps(G4bool checkOverlaps) -> void { fCheckOverlap = checkOverlaps; }

public:
    using ProminentDescription = std::tuple<MACE::Detector::Description::ECAL,
                                            PhaseI::Detector::Description::CentralBeamPipe,
                                            MACE::Detector::Description::Target,
                                            PhaseI::Detector::Description::SciFiTracker,
                                            PhaseI::Detector::Description::TTC,
                                            PhaseI::Detector::Description::World>;

private:
    G4bool fCheckOverlap;

    std::unique_ptr<Mustard::Detector::Definition::DefinitionBase> fWorld;
};

} // namespace MACE::PhaseI::SimMACEPhaseI::inline Action
