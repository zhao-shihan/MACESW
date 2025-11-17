// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "MACE/Detector/Assembly/MMS.h++"
#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/BeamDegrader.h++"
#include "MACE/Detector/Description/BeamMonitor.h++"
#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/Collimator.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Detector/Description/ECALField.h++"
#include "MACE/Detector/Description/ECALMagnet.h++"
#include "MACE/Detector/Description/ECALShield.h++"
#include "MACE/Detector/Description/FieldOption.h++"
#include "MACE/Detector/Description/MCP.h++"
#include "MACE/Detector/Description/MCPChamber.h++"
#include "MACE/Detector/Description/MMSBeamPipe.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/MMSMagnet.h++"
#include "MACE/Detector/Description/MMSShield.h++"
#include "MACE/Detector/Description/ShieldingWall.h++"
#include "MACE/Detector/Description/Solenoid.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/Detector/Description/Target.h++"
#include "MACE/Detector/Description/Vacuum.h++"
#include "MACE/Detector/Description/World.h++"
#include "MACE/SimMACE/Messenger/PhysicsMessenger.h++"
#include "MACE/Simulation/Messenger/NumericMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4VUserDetectorConstruction.hh"

#include "muc/tuple"

#include <memory>

namespace Mustard::Detector::Definition {
class DefinitionBase;
} // namespace Mustard::Detector::Definition

namespace MACE::SimMACE::inline Action {

class DetectorConstruction final : public Mustard::Env::Memory::PassiveSingleton<DetectorConstruction>,
                                   public G4VUserDetectorConstruction {
public:
    DetectorConstruction();

    auto SetCheckOverlaps(G4bool checkOverlaps) -> void { fCheckOverlap = checkOverlaps; }

    auto MinDriverStep(double val) -> void { fMinDriverStep = val; }
    auto DeltaChord(double val) -> void { fDeltaChord = val; }

    auto Construct() -> G4VPhysicalVolume* override;

    static auto ApplyProductionCutNearTarget(bool apply) -> void;

public:
    using ProminentDescription = muc::tuple_unique_t<muc::tuple_concat_t<
        Detector::Assembly::MMS::ProminentDescription,
        std::tuple<Detector::Description::Accelerator,
                   Detector::Description::BeamDegrader,
                   Detector::Description::BeamMonitor,
                   Detector::Description::Collimator,
                   Detector::Description::ECAL,
                   Detector::Description::ECALField,
                   Detector::Description::ECALMagnet,
                   Detector::Description::ECALShield,
                   Detector::Description::MCP,
                   Detector::Description::MCPChamber,
                   Detector::Description::ShieldingWall,
                   Detector::Description::Solenoid,
                   Detector::Description::Target,
                   Detector::Description::Vacuum,
                   Detector::Description::World>>>;

private:
    G4bool fCheckOverlap;

    double fMinDriverStep;
    double fDeltaChord;

    std::unique_ptr<Mustard::Detector::Definition::DefinitionBase> fWorld;

    PhysicsMessenger::Register<DetectorConstruction> fPhysicsMessengerRegister;
    NumericMessenger<DetectorConstruction>::Register<DetectorConstruction> fNumericMessengerRegister;
};

} // namespace MACE::SimMACE::inline Action
