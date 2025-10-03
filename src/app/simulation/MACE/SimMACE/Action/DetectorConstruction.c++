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
#include "MACE/Detector/Definition/PDSVeto.h++"
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
#include "MACE/SimMACE/Action/DetectorConstruction.h++"
#include "MACE/SimMACE/Messenger/DetectorMessenger.h++"
#include "MACE/SimMACE/PhysicsList.h++"
#include "MACE/SimMACE/SD/CDCSD.h++"
#include "MACE/SimMACE/SD/ECALSD.h++"
#include "MACE/SimMACE/SD/MCPSD.h++"
#include "MACE/SimMACE/SD/TTCSD.h++"

#include "Mustard/Detector/Definition/DefinitionBase.h++"
#include "Mustard/Detector/Field/AsG4Field.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4InterpolationDriver.hh"
#include "G4ProductionCuts.hh"
#include "G4ProductionCutsTable.hh"
#include "G4RegionStore.hh"
#include "G4TDormandPrince45.hh"
#include "G4TMagFieldEquation.hh"

#include "gsl/gsl"

namespace MACE::SimMACE::inline Action {

using namespace Mustard::LiteralUnit::Length;

DetectorConstruction::DetectorConstruction() :
    PassiveSingleton{this},
    G4VUserDetectorConstruction{},
    fCheckOverlap{},
    fMinDriverStep{2_um},
    fDeltaChord{2_um},
    fWorld{},
    fPhysicsMessengerRegister{this},
    fNumericMessengerRegister{this} {
    DetectorMessenger::EnsureInstantiation();
}

auto DetectorConstruction::ApplyProductionCutNearTarget(bool apply) const -> void {
    const auto cut{G4RegionStore::GetInstance()->GetRegion("NearTargetDenseToThin")->GetProductionCuts()};
    if (apply) {
        auto defaultCut{G4ProductionCutsTable::GetProductionCutsTable()->GetDefaultProductionCuts()->GetProductionCuts()};
        cut->SetProductionCuts(defaultCut);
    } else {
        cut->SetProductionCut(0, "e-");
        cut->SetProductionCut(0, "e+");
        cut->SetProductionCut(0, "proton");
    }
}

auto DetectorConstruction::Construct() -> G4VPhysicalVolume* {
    ////////////////////////////////////////////////////////////////
    // Construct volumes
    ////////////////////////////////////////////////////////////////

    // 0

    fWorld = std::make_unique<Detector::Definition::World>();

    // 1

    Detector::Assembly::MMS mms{*fWorld, fCheckOverlap};

    auto& ecalField{fWorld->NewDaughter<Detector::Definition::ECALField>(fCheckOverlap)};
    auto& ecalShield{fWorld->NewDaughter<Detector::Definition::ECALShield>(fCheckOverlap)};
    auto& shieldingWall{fWorld->NewDaughter<Detector::Definition::ShieldingWall>(fCheckOverlap)};
    auto& solenoidFieldS1{fWorld->NewDaughter<Detector::Definition::SolenoidFieldS1>(fCheckOverlap)};
    auto& solenoidFieldS2{fWorld->NewDaughter<Detector::Definition::SolenoidFieldS2>(fCheckOverlap)};
    auto& solenoidFieldS3{fWorld->NewDaughter<Detector::Definition::SolenoidFieldS3>(fCheckOverlap)};
    auto& solenoidFieldT1{fWorld->NewDaughter<Detector::Definition::SolenoidFieldT1>(fCheckOverlap)};
    auto& solenoidFieldT2{fWorld->NewDaughter<Detector::Definition::SolenoidFieldT2>(fCheckOverlap)};
    // auto& veto{fWorld->NewDaughter<Detector::Definition::PDSVeto>(fCheckOverlap)};

    // 2

    auto& ecalCrystal{ecalField.NewDaughter<Detector::Definition::ECALCrystal>(fCheckOverlap)};
    ecalField.NewDaughter<Detector::Definition::ECALMagnet>(fCheckOverlap);
    ecalField.NewDaughter<Detector::Definition::ECALPhotoSensor>(fCheckOverlap);
    auto& mcpChamber{ecalField.NewDaughter<Detector::Definition::MCPChamber>(fCheckOverlap)};

    solenoidFieldS1.NewDaughter<Detector::Definition::SolenoidBeamPipeS1>(fCheckOverlap);
    solenoidFieldS1.NewDaughter<Detector::Definition::SolenoidS1>(fCheckOverlap);
    auto& solenoidShieldS1{solenoidFieldS1.NewDaughter<Detector::Definition::SolenoidShieldS1>(fCheckOverlap)};

    auto& solenoidBeamPipeS2{solenoidFieldS2.NewDaughter<Detector::Definition::SolenoidBeamPipeS2>(fCheckOverlap)};
    solenoidFieldS2.NewDaughter<Detector::Definition::SolenoidS2>(fCheckOverlap);
    auto& solenoidShieldS2{solenoidFieldS2.NewDaughter<Detector::Definition::SolenoidShieldS2>(fCheckOverlap)};

    solenoidFieldS3.NewDaughter<Detector::Definition::SolenoidBeamPipeS3>(fCheckOverlap);
    solenoidFieldS3.NewDaughter<Detector::Definition::SolenoidS3>(fCheckOverlap);
    auto& solenoidShieldS3{solenoidFieldS3.NewDaughter<Detector::Definition::SolenoidShieldS3>(fCheckOverlap)};

    solenoidFieldT1.NewDaughter<Detector::Definition::SolenoidBeamPipeT1>(fCheckOverlap);
    auto& solenoidShieldT1{solenoidFieldT1.NewDaughter<Detector::Definition::SolenoidShieldT1>(fCheckOverlap)};
    solenoidFieldT1.NewDaughter<Detector::Definition::SolenoidT1>(fCheckOverlap);

    solenoidFieldT2.NewDaughter<Detector::Definition::SolenoidBeamPipeT2>(fCheckOverlap);
    auto& solenoidShieldT2{solenoidFieldT2.NewDaughter<Detector::Definition::SolenoidShieldT2>(fCheckOverlap)};
    solenoidFieldT2.NewDaughter<Detector::Definition::SolenoidT2>(fCheckOverlap);

    // 3

    auto& mcp{mcpChamber.NewDaughter<Detector::Definition::MCP>(fCheckOverlap)};

    auto& acceleratorField{mms.Get<Detector::Definition::MMSBeamPipe>().NewDaughter<Detector::Definition::AcceleratorField>(fCheckOverlap)};

    auto& collimator{solenoidBeamPipeS2.NewDaughter<Detector::Definition::Collimator>(fCheckOverlap)};

    // 4

    auto& accelerator{acceleratorField.NewDaughter<Detector::Definition::Accelerator>(fCheckOverlap)};
    auto& beamDegrader{acceleratorField.NewDaughter<Detector::Definition::BeamDegrader>(fCheckOverlap)};
    auto& beamMonitor{acceleratorField.NewDaughter<Detector::Definition::BeamMonitor>(fCheckOverlap)};
    auto& target{acceleratorField.NewDaughter<Detector::Definition::Target>(fCheckOverlap)};

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

        collimator.RegisterRegion(denseToThinRegion);
        mms.Get<Detector::Definition::CDCCell>().RegisterRegion("CDCFieldWire", denseToThinRegion);
        mms.Get<Detector::Definition::CDCCell>().RegisterRegion("CDCSenseWire", denseToThinRegion);

        // Near-target-dense-to-thin region

        const auto nearTargetDenseToThinRegionCut{new G4ProductionCuts};
        nearTargetDenseToThinRegionCut->SetProductionCut(0, "e-");
        nearTargetDenseToThinRegionCut->SetProductionCut(0, "e+");
        nearTargetDenseToThinRegionCut->SetProductionCut(0, "proton");
        const auto nearTargetDenseToThinRegion{new G4Region{"NearTargetDenseToThin"}};
        nearTargetDenseToThinRegion->SetProductionCuts(nearTargetDenseToThinRegionCut);

        accelerator.RegisterRegion(nearTargetDenseToThinRegion);
        beamDegrader.RegisterRegion(nearTargetDenseToThinRegion);
        beamMonitor.RegisterRegion(nearTargetDenseToThinRegion);
        target.RegisterRegion(nearTargetDenseToThinRegion);

        // Shield region

        const auto shieldRegionCut{new G4ProductionCuts};
        shieldRegionCut->SetProductionCut(2_mm);
        const auto shieldRegion{new G4Region{"Shield"}};
        shieldRegion->SetProductionCuts(shieldRegionCut);

        ecalShield.RegisterRegion(shieldRegion);
        mms.Get<Detector::Definition::MMSShield>().RegisterRegion(shieldRegion);
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

        /* // Beam dump region

        const auto beamDumpRegionCut{new G4ProductionCuts};
        beamDumpRegionCut->SetProductionCut(10_cm); */
    }

    ////////////////////////////////////////////////////////////////
    // Register SDs
    ////////////////////////////////////////////////////////////////
    {
        const auto& cdcName{Detector::Description::CDC::Instance().Name()};
        mms.Get<Detector::Definition::CDCCell>().RegisterSD(cdcName + "SensitiveVolume", new SD::CDCSD{cdcName});
        const auto& ttcName{Detector::Description::TTC::Instance().Name()};
        mms.Get<Detector::Definition::TTC>().RegisterSD(ttcName + "Scintillator", new SD::TTCSD{ttcName});
        const auto& mcpName{Detector::Description::MCP::Instance().Name()};
        mcp.RegisterSD(mcpName + "Anode", new SD::MCPSD{mcpName});
        ecalCrystal.RegisterSD(new SD::ECALSD{Detector::Description::ECAL::Instance().Name()});
    }

    ////////////////////////////////////////////////////////////////
    // Register background fields
    ////////////////////////////////////////////////////////////////
    {
        { // Magnetic fields
            const auto RegisterField{
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
            RegisterField(mms.Get<Detector::Definition::MMSField>(), new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::MMSField>, false);
            RegisterField(solenoidFieldS1, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldS1>, false);
            RegisterField(solenoidFieldT1, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldT1>, false);
            RegisterField(solenoidFieldS2, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldS2>, false);
            RegisterField(solenoidFieldT2, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldT2>, false);
            RegisterField(solenoidFieldS3, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::SolenoidFieldS3>, false);
            RegisterField(ecalField, new Mustard::Detector::Field::AsG4Field<MACE::Detector::Field::ECALField>, false);
        }
        { // Accelerator EM field, must be registered after MMS magnetic field
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

} // namespace MACE::SimMACE::inline Action
