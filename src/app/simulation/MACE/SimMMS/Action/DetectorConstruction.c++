#include "MACE/Detector/Assembly/MMS.h++"
#include "MACE/Detector/Definition/CDCBody.h++"
#include "MACE/Detector/Definition/CDCCell.h++"
#include "MACE/Detector/Definition/CDCGas.h++"
#include "MACE/Detector/Definition/CDCSenseLayer.h++"
#include "MACE/Detector/Definition/CDCSuperLayer.h++"
#include "MACE/Detector/Definition/MMSBeamPipe.h++"
#include "MACE/Detector/Definition/MMSField.h++"
#include "MACE/Detector/Definition/MMSMagnet.h++"
#include "MACE/Detector/Definition/MMSShield.h++"
#include "MACE/Detector/Definition/TTC.h++"
#include "MACE/Detector/Definition/World.h++"
#include "MACE/Detector/Field/MMSField.h++"
#include "MACE/SimMMS/Action/DetectorConstruction.h++"
#include "MACE/SimMMS/Messenger/DetectorMessenger.h++"
#include "MACE/SimMMS/SD/CDCSD.h++"
#include "MACE/SimMMS/SD/TTCSD.h++"
#include "MACE/SimMMS/SD/TTCSiPMSD.h++"

#include "Mustard/Detector/Definition/DefinitionBase.h++"
#include "Mustard/Detector/Field/AsG4Field.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4ChordFinder.hh"
#include "G4InterpolationDriver.hh"
#include "G4ProductionCuts.hh"
#include "G4TDormandPrince45.hh"
#include "G4TMagFieldEquation.hh"

namespace MACE::SimMMS::inline Action {

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

    fWorld = std::make_unique<Detector::Definition::World>();
    Detector::Assembly::MMS mms{*fWorld, fCheckOverlap};

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

        mms.Get<Detector::Definition::CDCCell>().RegisterRegion("CDCFieldWire", denseToThinRegion);
        mms.Get<Detector::Definition::CDCCell>().RegisterRegion("CDCSenseWire", denseToThinRegion);

        // Shield region

        const auto shieldRegionCut{new G4ProductionCuts};
        shieldRegionCut->SetProductionCut(2_mm);
        const auto shieldRegion{new G4Region{"Shield"}};
        shieldRegion->SetProductionCuts(shieldRegionCut);

        mms.Get<Detector::Definition::MMSShield>().RegisterRegion(shieldRegion);
    }

    ////////////////////////////////////////////////////////////////
    // Register SDs
    ////////////////////////////////////////////////////////////////
    {
        const auto& cdcName{Detector::Description::CDC::Instance().Name()};
        mms.Get<Detector::Definition::CDCCell>().RegisterSD(cdcName + "SensitiveVolume", new SD::CDCSD{cdcName});
        const auto& ttcName{Detector::Description::TTC::Instance().Name()};
        mms.Get<Detector::Definition::TTC>().RegisterSD(ttcName + "Scintillator", new SD::TTCSD{ttcName});
        mms.Get<Detector::Definition::TTC>().RegisterSD(ttcName + "Silicone", new SD::TTCSiPMSD{ttcName + "SiPM"});
    }

    ////////////////////////////////////////////////////////////////
    // Register background fields
    ////////////////////////////////////////////////////////////////
    {
        using Field = Mustard::Detector::Field::AsG4Field<Detector::Field::MMSField>;
        using Equation = G4TMagFieldEquation<Field>;
        using Stepper = G4TDormandPrince45<Equation, 6>;
        using Driver = G4InterpolationDriver<Stepper>;
        const auto field{new Field};
        const auto equation{new Equation{field}}; // clang-format off
        const auto stepper{new Stepper{equation, 6}};
        const auto driver{new Driver{fMinDriverStep, stepper, 6}}; // clang-format on
        const auto chordFinder{new G4ChordFinder{driver}};
        chordFinder->SetDeltaChord(fDeltaChord);
        mms.Get<Detector::Definition::MMSField>().RegisterField(std::make_unique<G4FieldManager>(field, chordFinder), false);
    }

    return fWorld->PhysicalVolume();
}

} // namespace MACE::SimMMS::inline Action
