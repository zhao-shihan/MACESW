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

#include "MACE/Detector/Definition/Accelerator.h++"
#include "MACE/Detector/Definition/AcceleratorField.h++"
#include "MACE/Detector/Definition/BeamDegrader.h++"
#include "MACE/Detector/Definition/BeamMonitor.h++"
#include "MACE/Detector/Definition/CDCBody.h++"
#include "MACE/Detector/Definition/CDCCell.h++"
#include "MACE/Detector/Definition/CDCGas.h++"
#include "MACE/Detector/Definition/CDCSenseLayer.h++"
#include "MACE/Detector/Definition/CDCSuperLayer.h++"
#include "MACE/Detector/Definition/Collimator.h++"
#include "MACE/Detector/Definition/ECALCrystal.h++"
#include "MACE/Detector/Definition/ECALField.h++"
#include "MACE/Detector/Definition/ECALMagnet.h++"
#include "MACE/Detector/Definition/ECALPhotoSensor.h++"
#include "MACE/Detector/Definition/ECALShield.h++"
#include "MACE/Detector/Definition/MCP.h++"
#include "MACE/Detector/Definition/MCPChamber.h++"
#include "MACE/Detector/Definition/MMSBeamPipe.h++"
#include "MACE/Detector/Definition/MMSField.h++"
#include "MACE/Detector/Definition/MMSMagnet.h++"
#include "MACE/Detector/Definition/MMSShield.h++"
#include "MACE/Detector/Definition/ShieldingWall.h++"
#include "MACE/Detector/Definition/SolenoidBeamPipeS1.h++"
#include "MACE/Detector/Definition/SolenoidBeamPipeS2.h++"
#include "MACE/Detector/Definition/SolenoidBeamPipeS3.h++"
#include "MACE/Detector/Definition/SolenoidBeamPipeT1.h++"
#include "MACE/Detector/Definition/SolenoidBeamPipeT2.h++"
#include "MACE/Detector/Definition/SolenoidFieldS1.h++"
#include "MACE/Detector/Definition/SolenoidFieldS2.h++"
#include "MACE/Detector/Definition/SolenoidFieldS3.h++"
#include "MACE/Detector/Definition/SolenoidFieldT1.h++"
#include "MACE/Detector/Definition/SolenoidFieldT2.h++"
#include "MACE/Detector/Definition/SolenoidS1.h++"
#include "MACE/Detector/Definition/SolenoidS2.h++"
#include "MACE/Detector/Definition/SolenoidS3.h++"
#include "MACE/Detector/Definition/SolenoidShieldS1.h++"
#include "MACE/Detector/Definition/SolenoidShieldS2.h++"
#include "MACE/Detector/Definition/SolenoidShieldS3.h++"
#include "MACE/Detector/Definition/SolenoidShieldT1.h++"
#include "MACE/Detector/Definition/SolenoidShieldT2.h++"
#include "MACE/Detector/Definition/SolenoidT1.h++"
#include "MACE/Detector/Definition/SolenoidT2.h++"
#include "MACE/Detector/Definition/TTC.h++"
#include "MACE/Detector/Definition/Target.h++"
#include "MACE/Detector/Definition/World.h++"
#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Detector/Description/MCP.h++"
#include "MACE/Detector/Field/AcceleratorField.h++"
#include "MACE/Detector/Field/ECALField.h++"
#include "MACE/Detector/Field/MMSField.h++"
#include "MACE/Detector/Field/SolenoidFieldS1.h++"
#include "MACE/Detector/Field/SolenoidFieldS2.h++"
#include "MACE/Detector/Field/SolenoidFieldS3.h++"
#include "MACE/Detector/Field/SolenoidFieldT1.h++"
#include "MACE/Detector/Field/SolenoidFieldT2.h++"
#include "MACE/SimDose/Action/DetectorConstruction.h++"
#include "MACE/SimDose/Messenger/DetectorMessenger.h++"

#include "Mustard/Detector/Definition/DefinitionBase.h++"
#include "Mustard/Detector/Field/AsG4Field.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4InterpolationDriver.hh"
#include "G4ProductionCuts.hh"
#include "G4ProductionCutsTable.hh"
#include "G4TDormandPrince45.hh"
#include "G4TMagFieldEquation.hh"
#include "G4Track.hh"
#include "G4UserLimits.hh"

#include "gsl/gsl"

namespace MACE::SimDose::inline Action {

using namespace Mustard::LiteralUnit::Length;

DetectorConstruction::DetectorConstruction() :
    PassiveSingleton{this},
    G4VUserDetectorConstruction{},
    fCheckOverlap{},
    fMinDriverStep{2_um},
    fDeltaChord{2_um},
    fVacuumStepLimit{std::make_unique_for_overwrite<G4UserLimits>()},
    fWorld{},
    fNumericMessengerRegister{this} {
    DetectorMessenger::EnsureInstantiation();
}

DetectorConstruction::~DetectorConstruction() = default;

auto DetectorConstruction::VacuumStepLimit() const -> double {
    return fVacuumStepLimit->GetMaxAllowedStep({});
}

auto DetectorConstruction::VacuumStepLimit(double val) -> void {
    fVacuumStepLimit->SetMaxAllowedStep(val);
}

auto DetectorConstruction::Construct() -> G4VPhysicalVolume* {
    ////////////////////////////////////////////////////////////////
    // Construct volumes
    ////////////////////////////////////////////////////////////////

    // 0

    fWorld = std::make_unique<Detector::Definition::World>();

    // 1

    auto& ecalField{fWorld->NewDaughter<Detector::Definition::ECALField>(fCheckOverlap)};
    fWorld->NewDaughter<Detector::Definition::ECALShield>(fCheckOverlap);
    auto& mmsField{fWorld->NewDaughter<Detector::Definition::MMSField>(fCheckOverlap)};
    fWorld->NewDaughter<Detector::Definition::MMSShield>(fCheckOverlap);
    fWorld->NewDaughter<Detector::Definition::ShieldingWall>(fCheckOverlap);
    auto& solenoidFieldS1{fWorld->NewDaughter<Detector::Definition::SolenoidFieldS1>(fCheckOverlap)};
    auto& solenoidFieldS2{fWorld->NewDaughter<Detector::Definition::SolenoidFieldS2>(fCheckOverlap)};
    auto& solenoidFieldS3{fWorld->NewDaughter<Detector::Definition::SolenoidFieldS3>(fCheckOverlap)};
    auto& solenoidFieldT1{fWorld->NewDaughter<Detector::Definition::SolenoidFieldT1>(fCheckOverlap)};
    auto& solenoidFieldT2{fWorld->NewDaughter<Detector::Definition::SolenoidFieldT2>(fCheckOverlap)};

    // 2

    ecalField.NewDaughter<Detector::Definition::ECALCrystal>(fCheckOverlap);
    ecalField.NewDaughter<Detector::Definition::ECALMagnet>(fCheckOverlap);
    ecalField.NewDaughter<Detector::Definition::ECALPhotoSensor>(fCheckOverlap);
    auto& mcpChamber{ecalField.NewDaughter<Detector::Definition::MCPChamber>(fCheckOverlap)};

    auto& solenoidBeamPipeS1{solenoidFieldS1.NewDaughter<Detector::Definition::SolenoidBeamPipeS1>(fCheckOverlap)};
    solenoidFieldS1.NewDaughter<Detector::Definition::SolenoidS1>(fCheckOverlap);
    solenoidFieldS1.NewDaughter<Detector::Definition::SolenoidShieldS1>(fCheckOverlap);

    auto& solenoidBeamPipeS2{solenoidFieldS2.NewDaughter<Detector::Definition::SolenoidBeamPipeS2>(fCheckOverlap)};
    solenoidFieldS2.NewDaughter<Detector::Definition::SolenoidS2>(fCheckOverlap);
    solenoidFieldS2.NewDaughter<Detector::Definition::SolenoidShieldS2>(fCheckOverlap);

    auto& solenoidBeamPipeS3{solenoidFieldS3.NewDaughter<Detector::Definition::SolenoidBeamPipeS3>(fCheckOverlap)};
    solenoidFieldS3.NewDaughter<Detector::Definition::SolenoidS3>(fCheckOverlap);
    solenoidFieldS3.NewDaughter<Detector::Definition::SolenoidShieldS3>(fCheckOverlap);

    auto& solenoidBeamPipeT1{solenoidFieldT1.NewDaughter<Detector::Definition::SolenoidBeamPipeT1>(fCheckOverlap)};
    solenoidFieldT1.NewDaughter<Detector::Definition::SolenoidShieldT1>(fCheckOverlap);
    solenoidFieldT1.NewDaughter<Detector::Definition::SolenoidT1>(fCheckOverlap);

    auto& solenoidBeamPipeT2{solenoidFieldT2.NewDaughter<Detector::Definition::SolenoidBeamPipeT2>(fCheckOverlap)};
    solenoidFieldT2.NewDaughter<Detector::Definition::SolenoidShieldT2>(fCheckOverlap);
    solenoidFieldT2.NewDaughter<Detector::Definition::SolenoidT2>(fCheckOverlap);

    auto& cdcBody{mmsField.NewDaughter<Detector::Definition::CDCBody>(fCheckOverlap)};
    auto& mmsBeamPipe{mmsField.NewDaughter<Detector::Definition::MMSBeamPipe>(fCheckOverlap)};
    mmsField.NewDaughter<Detector::Definition::MMSMagnet>(fCheckOverlap);
    mmsField.NewDaughter<Detector::Definition::TTC>(fCheckOverlap);

    // 3

    mcpChamber.NewDaughter<Detector::Definition::MCP>(fCheckOverlap);

    auto& acceleratorField{mmsBeamPipe.NewDaughter<Detector::Definition::AcceleratorField>(fCheckOverlap)};

    solenoidBeamPipeS2.NewDaughter<Detector::Definition::Collimator>(fCheckOverlap);

    auto& cdcGas{cdcBody.NewDaughter<Detector::Definition::CDCGas>(fCheckOverlap)};

    // 4

    acceleratorField.NewDaughter<Detector::Definition::Accelerator>(fCheckOverlap);
    acceleratorField.NewDaughter<Detector::Definition::BeamDegrader>(fCheckOverlap);
    acceleratorField.NewDaughter<Detector::Definition::BeamMonitor>(fCheckOverlap);
    acceleratorField.NewDaughter<Detector::Definition::Target>(fCheckOverlap);

    auto& cdcSuperLayer{cdcGas.NewDaughter<Detector::Definition::CDCSuperLayer>(fCheckOverlap)};

    // 5

    auto& cdcSenseLayer{cdcSuperLayer.NewDaughter<Detector::Definition::CDCSenseLayer>(fCheckOverlap)};

    // 6

    cdcSenseLayer.NewDaughter<Detector::Definition::CDCCell>(fCheckOverlap);

    ////////////////////////////////////////////////////////////////
    // Register regions and apply limit
    ////////////////////////////////////////////////////////////////
    {
        const auto vacuumRegion{new G4Region{"Vacuum"}};
        vacuumRegion->SetUserLimits(fVacuumStepLimit.get());

        mcpChamber.RegisterRegion("MCPChamberPipeVacuum", vacuumRegion);
        mcpChamber.RegisterRegion("MCPChamberVacuum", vacuumRegion);
        mmsBeamPipe.RegisterRegion("MMSBeamPipeVacuum", vacuumRegion);
        solenoidBeamPipeS1.RegisterRegion("SolenoidBeamPipeS1Vacuum", vacuumRegion);
        solenoidBeamPipeS2.RegisterRegion("SolenoidBeamPipeS2Vacuum", vacuumRegion);
        solenoidBeamPipeS3.RegisterRegion("SolenoidBeamPipeS3Vacuum", vacuumRegion);
        solenoidBeamPipeT1.RegisterRegion("SolenoidBeamPipeT1Vacuum", vacuumRegion);
        solenoidBeamPipeT2.RegisterRegion("SolenoidBeamPipeT2Vacuum", vacuumRegion);
    }

    ////////////////////////////////////////////////////////////////
    // Register background fields
    ////////////////////////////////////////////////////////////////
    {
        { // Magnetic fields
            const auto registerField{
                [this]<typename AField>(Mustard::Detector::Definition::DefinitionBase& detector, AField* field, bool forceToAllDaughters) {
                    using Equation = G4TMagFieldEquation<AField>;
                    using Stepper = G4TDormandPrince45<Equation, 6>;
                    using Driver = G4InterpolationDriver<Stepper>;
                    const auto equation{new Equation{field}}; // clang-format off
                    const auto stepper{new Stepper{equation, 6}};
                    const auto driver{new Driver{fMinDriverStep, stepper, 6}}; // clang-format on
                    const auto chordFinder{new G4ChordFinder{driver}};
                    chordFinder->SetDeltaChord(fDeltaChord);
                    detector.RegisterField(std::make_unique<G4FieldManager>(field, chordFinder), forceToAllDaughters);
                }};
            registerField(mmsField, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::MMSField>, false);
            registerField(solenoidFieldS1, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldS1>, false);
            registerField(solenoidFieldT1, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldT1>, false);
            registerField(solenoidFieldS2, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldS2>, false);
            registerField(solenoidFieldT2, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldT2>, false);
            registerField(solenoidFieldS3, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldS3>, false);
            registerField(ecalField, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::ECALField>, false);
        }
        { // Accelerator EM field, must be reigstered after MMS magnetic field
            using Field = Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::AcceleratorField>;
            using Equation = G4EqMagElectricField;
            using Stepper = G4TDormandPrince45<Equation, 8>;
            using Driver = G4InterpolationDriver<Stepper>;
            const auto field{new Field};
            const auto equation{new Equation{field}}; // clang-format off
            const auto stepper{new Stepper{equation, 8}};
            const auto driver{new Driver{fMinDriverStep, stepper, 8}}; // clang-format on
            const auto chordFinder{new G4ChordFinder{driver}};
            chordFinder->SetDeltaChord(fDeltaChord);
            acceleratorField.RegisterField(std::make_unique<G4FieldManager>(field, chordFinder), false);
        }
    }

    return fWorld->PhysicalVolume();
}

} // namespace MACE::SimDose::inline Action
