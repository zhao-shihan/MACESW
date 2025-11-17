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

#include "MACE/Detector/Definition/ECALCrystal.h++"
#include "MACE/Detector/Definition/ECALPhotoSensor.h++"
#include "MACE/Detector/Definition/Target.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/PhaseI/Detector/Definition/CentralBeamPipe.h++"
#include "MACE/PhaseI/Detector/Definition/MRPC.h++"
#include "MACE/PhaseI/Detector/Definition/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Definition/TTC.h++"
#include "MACE/PhaseI/Detector/Definition/World.h++"
#include "MACE/PhaseI/Detector/Description/TTC.h++"
#include "MACE/PhaseI/Detector/Description/UsePhaseIDefault.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Action/DetectorConstruction.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Messenger/DetectorMessenger.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/ECALPMSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/ECALSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/MRPCSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/SciFiSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/SciFiSiPMSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/TTCSD.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/TTCSiPMSD.h++"

#include "Mustard/Detector/Definition/DefinitionBase.h++"
#include "Mustard/Detector/Description/DescriptionIO.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4ProductionCuts.hh"
#include "G4ProductionCutsTable.hh"
#include "G4Region.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include <array>

namespace MACE::PhaseI::SimMACEPhaseI::inline Action {

using namespace Mustard::LiteralUnit;

DetectorConstruction::DetectorConstruction() :
    PassiveSingleton{this},
    G4VUserDetectorConstruction{},
    fCheckOverlap{false},
    fWorld{} {
    DetectorMessenger::EnsureInstantiation();
    Detector::Description::UsePhaseIDefault();
}

auto DetectorConstruction::Construct() -> G4VPhysicalVolume* {
    namespace PhaseI = PhaseI::Detector::Definition;
    using namespace MACE::Detector::Definition;

    fWorld = std::make_unique<PhaseI::World>();
    auto& ecalCrystal{fWorld->NewDaughter<ECALCrystal>(fCheckOverlap)};
    auto& ecalPhotoSensor{fWorld->NewDaughter<ECALPhotoSensor>(fCheckOverlap)};
    auto& centralBeamPipe{fWorld->NewDaughter<PhaseI::CentralBeamPipe>(fCheckOverlap)};
    auto& sciFiTracker{fWorld->NewDaughter<PhaseI::SciFiTracker>(fCheckOverlap)};
    auto& ttc{fWorld->NewDaughter<PhaseI::TTC>(fCheckOverlap)};

    centralBeamPipe.NewDaughter<Target>(fCheckOverlap);

    const auto& ecalName{MACE::Detector::Description::ECAL::Instance().Name()};
    const auto& scifiName{MACE::PhaseI::Detector::Description::SciFiTracker::Instance().Name()};

    const auto fECALPMSD{new SD::ECALPMSD{ecalName + "PM"}};
    ecalPhotoSensor.RegisterSD("ECALPMCathode", fECALPMSD);

    const auto ecalSD(new SD::ECALSD{ecalName, fECALPMSD});
    ecalCrystal.RegisterSD(ecalSD);

    const auto fSciFiSD{new SD::SciFiSD{scifiName}};
    sciFiTracker.RegisterSD(scifiName + "HelicalFiberCore", fSciFiSD);
    sciFiTracker.RegisterSD(scifiName + "TransverseFiberCore", fSciFiSD);

    const auto sciFiSiPMSD{new SD::SciFiSiPMSD{scifiName + "SiPM"}};
    sciFiTracker.RegisterSD(scifiName + "SiPM", sciFiSiPMSD);

    const auto ttcSiPM(new SD::TTCSiPMSD{MACE::PhaseI::Detector::Description::TTC::Instance().Name() + "SiPM", TTCSiPMSD::Type::MACEPhaseI});
    ttc.RegisterSD("TTCScintillator", new SD::TTCSD{MACE::PhaseI::Detector::Description::TTC::Instance().Name(), TTCSD::Type::MACEPhaseI});
    ttc.RegisterSD("TTCSilicone", ttcSiPM);

    return fWorld->PhysicalVolume();
}

} // namespace MACE::PhaseI::SimMACEPhaseI::inline Action
