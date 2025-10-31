#include "MACE/Detector/Definition/Accelerator.h++"
#include "MACE/Detector/Definition/AcceleratorField.h++"
#include "MACE/Detector/Definition/Collimator.h++"
#include "MACE/Detector/Definition/ECALField.h++"
#include "MACE/Detector/Definition/ECALMagnet.h++"
#include "MACE/Detector/Definition/ECALShield.h++"
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
#include "MACE/Detector/Definition/World.h++"
#include "MACE/Detector/Field/AcceleratorField.h++"
#include "MACE/Detector/Field/ECALField.h++"
#include "MACE/Detector/Field/MMSField.h++"
#include "MACE/Detector/Field/SolenoidFieldS1.h++"
#include "MACE/Detector/Field/SolenoidFieldS2.h++"
#include "MACE/Detector/Field/SolenoidFieldS3.h++"
#include "MACE/Detector/Field/SolenoidFieldT1.h++"
#include "MACE/Detector/Field/SolenoidFieldT2.h++"
#include "MACE/SimPTS/Action/DetectorConstruction.h++"
#include "MACE/SimPTS/Detector/Definition/VirtualDetectorA.h++"
#include "MACE/SimPTS/Detector/Definition/VirtualDetectorB.h++"
#include "MACE/SimPTS/Detector/Definition/VirtualDetectorC.h++"
#include "MACE/SimPTS/Detector/Definition/VirtualDetectorD.h++"
#include "MACE/SimPTS/Messenger/DetectorMessenger.h++"
#include "MACE/SimPTS/SD/VirtualSD.h++"

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

#include "gsl/gsl"

namespace MACE::SimPTS::inline Action {

using namespace Mustard::LiteralUnit::Length;

DetectorConstruction::DetectorConstruction() :
    PassiveSingleton{this},
    G4VUserDetectorConstruction{},
    fCheckOverlap{},
    fMinDriverStep{2_um},
    fDeltaChord{2_um},
    fWorld{},
    fNumericMessengerRegister{this} {
    DetectorMessenger::EnsureInstantiation();
}

auto DetectorConstruction::Construct() -> G4VPhysicalVolume* {
    ////////////////////////////////////////////////////////////////
    // Construct volumes
    ////////////////////////////////////////////////////////////////

    // 0

    fWorld = std::make_unique<MACE::Detector::Definition::World>();

    // 1

    auto& ecalField{fWorld->NewDaughter<MACE::Detector::Definition::ECALField>(fCheckOverlap)};
    auto& ecalShield{fWorld->NewDaughter<MACE::Detector::Definition::ECALShield>(fCheckOverlap)};
    auto& mmsField{fWorld->NewDaughter<MACE::Detector::Definition::MMSField>(fCheckOverlap)};
    auto& mmsShield{fWorld->NewDaughter<MACE::Detector::Definition::MMSShield>(fCheckOverlap)};
    auto& shieldingWall{fWorld->NewDaughter<MACE::Detector::Definition::ShieldingWall>(fCheckOverlap)};
    auto& solenoidFieldS1{fWorld->NewDaughter<MACE::Detector::Definition::SolenoidFieldS1>(fCheckOverlap)};
    auto& solenoidFieldS2{fWorld->NewDaughter<MACE::Detector::Definition::SolenoidFieldS2>(fCheckOverlap)};
    auto& solenoidFieldS3{fWorld->NewDaughter<MACE::Detector::Definition::SolenoidFieldS3>(fCheckOverlap)};
    auto& solenoidFieldT1{fWorld->NewDaughter<MACE::Detector::Definition::SolenoidFieldT1>(fCheckOverlap)};
    auto& solenoidFieldT2{fWorld->NewDaughter<MACE::Detector::Definition::SolenoidFieldT2>(fCheckOverlap)};

    // 2

    ecalField.NewDaughter<MACE::Detector::Definition::ECALMagnet>(fCheckOverlap);
    auto& mcpChamber{ecalField.NewDaughter<MACE::Detector::Definition::MCPChamber>(fCheckOverlap)};

    solenoidFieldS1.NewDaughter<MACE::Detector::Definition::SolenoidBeamPipeS1>(fCheckOverlap);
    solenoidFieldS1.NewDaughter<MACE::Detector::Definition::SolenoidS1>(fCheckOverlap);
    auto& solenoidShieldS1{solenoidFieldS1.NewDaughter<MACE::Detector::Definition::SolenoidShieldS1>(fCheckOverlap)};

    auto& solenoidBeamPipeS2{solenoidFieldS2.NewDaughter<MACE::Detector::Definition::SolenoidBeamPipeS2>(fCheckOverlap)};
    solenoidFieldS2.NewDaughter<MACE::Detector::Definition::SolenoidS2>(fCheckOverlap);
    auto& solenoidShieldS2{solenoidFieldS2.NewDaughter<MACE::Detector::Definition::SolenoidShieldS2>(fCheckOverlap)};

    solenoidFieldS3.NewDaughter<MACE::Detector::Definition::SolenoidBeamPipeS3>(fCheckOverlap);
    solenoidFieldS3.NewDaughter<MACE::Detector::Definition::SolenoidS3>(fCheckOverlap);
    auto& solenoidShieldS3{solenoidFieldS3.NewDaughter<MACE::Detector::Definition::SolenoidShieldS3>(fCheckOverlap)};

    solenoidFieldT1.NewDaughter<MACE::Detector::Definition::SolenoidBeamPipeT1>(fCheckOverlap);
    auto& solenoidShieldT1{solenoidFieldT1.NewDaughter<MACE::Detector::Definition::SolenoidShieldT1>(fCheckOverlap)};
    solenoidFieldT1.NewDaughter<MACE::Detector::Definition::SolenoidT1>(fCheckOverlap);

    solenoidFieldT2.NewDaughter<MACE::Detector::Definition::SolenoidBeamPipeT2>(fCheckOverlap);
    auto& solenoidShieldT2{solenoidFieldT2.NewDaughter<MACE::Detector::Definition::SolenoidShieldT2>(fCheckOverlap)};
    solenoidFieldT2.NewDaughter<MACE::Detector::Definition::SolenoidT2>(fCheckOverlap);

    auto& mmsBeamPipe{mmsField.NewDaughter<MACE::Detector::Definition::MMSBeamPipe>(fCheckOverlap)};
    mmsField.NewDaughter<MACE::Detector::Definition::MMSMagnet>(fCheckOverlap);

    // 3

    auto& virtualDetectorD{mcpChamber.NewDaughter<Detector::Definition::VirtualDetectorD>(fCheckOverlap)};

    auto& collimator{solenoidBeamPipeS2.NewDaughter<MACE::Detector::Definition::Collimator>(fCheckOverlap)};
    auto& virtualDetectorB{solenoidBeamPipeS2.NewDaughter<Detector::Definition::VirtualDetectorB>(fCheckOverlap)};
    auto& virtualDetectorC{solenoidBeamPipeS2.NewDaughter<Detector::Definition::VirtualDetectorC>(fCheckOverlap)};

    auto& acceleratorField{mmsBeamPipe.NewDaughter<MACE::Detector::Definition::AcceleratorField>(fCheckOverlap)};
    auto& virtualDetectorA{mmsBeamPipe.NewDaughter<Detector::Definition::VirtualDetectorA>(fCheckOverlap)};

    // 4

    auto& accelerator{acceleratorField.NewDaughter<MACE::Detector::Definition::Accelerator>(fCheckOverlap)};

    ////////////////////////////////////////////////////////////////
    // Set production cuts
    ////////////////////////////////////////////////////////////////
    {
        // Dense-to-thin region

        const auto denseToThinRegionCut{new G4ProductionCuts};
        denseToThinRegionCut->SetProductionCut(0, "e-");
        denseToThinRegionCut->SetProductionCut(0, "e+");
        denseToThinRegionCut->SetProductionCut(0, "proton");
        const auto denseToThinRegion{new G4Region{"DenseToThin"}};
        denseToThinRegion->SetProductionCuts(denseToThinRegionCut);

        accelerator.RegisterRegion(denseToThinRegion);
        collimator.RegisterRegion(denseToThinRegion);

        // Shield region

        const auto shieldRegionCut{new G4ProductionCuts};
        shieldRegionCut->SetProductionCut(2_mm);
        const auto shieldRegion{new G4Region{"Shield"}};
        shieldRegion->SetProductionCuts(shieldRegionCut);

        ecalShield.RegisterRegion(shieldRegion);
        mmsShield.RegisterRegion(shieldRegion);
        solenoidShieldS1.RegisterRegion(shieldRegion);
        solenoidShieldS2.RegisterRegion(shieldRegion);
        solenoidShieldS3.RegisterRegion(shieldRegion);
        solenoidShieldT1.RegisterRegion(shieldRegion);
        solenoidShieldT2.RegisterRegion(shieldRegion);

        // Wall region

        const auto wallRegionCut{new G4ProductionCuts};
        wallRegionCut->SetProductionCut(3_cm);
        const auto wallRegion{new G4Region{"Wall"}};
        wallRegion->SetProductionCuts(wallRegionCut);

        shieldingWall.RegisterRegion(wallRegion);
    }

    ////////////////////////////////////////////////////////////////
    // Register SDs
    ////////////////////////////////////////////////////////////////
    {
        const auto virtualSD{new SD::VirtualSD{"VirtualDetector"}};
        virtualDetectorA.RegisterSD(virtualSD);
        virtualDetectorB.RegisterSD(virtualSD);
        virtualDetectorC.RegisterSD(virtualSD);
        virtualDetectorD.RegisterSD(virtualSD);
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

} // namespace MACE::SimPTS::inline Action
