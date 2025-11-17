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

#include "MACE/Detector/Assembly/MMS.h++"
#include "MACE/Detector/Definition/MMSBeamPipe.h++"
#include "MACE/Detector/Definition/MMSField.h++"
#include "MACE/Detector/Definition/TTC.h++"
#include "MACE/Detector/Definition/World.h++"
#include "MACE/Detector/Field/MMSField.h++"
#include "MACE/SimTTC/Action/DetectorConstruction.h++"
#include "MACE/SimTTC/Messenger/DetectorMessenger.h++"
#include "MACE/SimTTC/SD/TTCSD.h++"
#include "MACE/SimTTC/SD/TTCSiPMSD.h++"

#include "Mustard/Detector/Definition/DefinitionBase.h++"
#include "Mustard/Detector/Field/AsG4Field.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4ChordFinder.hh"
#include "G4InterpolationDriver.hh"
#include "G4ProductionCuts.hh"
#include "G4TDormandPrince45.hh"
#include "G4TMagFieldEquation.hh"

namespace MACE::SimTTC::inline Action {

using namespace Mustard::LiteralUnit::Length;

DetectorConstruction::DetectorConstruction() :
    PassiveSingleton{this},
    G4VUserDetectorConstruction{},
    fCheckOverlap{},
    fMinDriverStep{500_um},
    fDeltaChord{500_mm},
    fWorld{},
    fNumericMessengerRegister{this} {
    DetectorMessenger::EnsureInstantiation();
}

auto DetectorConstruction::Construct() -> G4VPhysicalVolume* {
    ////////////////////////////////////////////////////////////////
    // Construct volumes
    ////////////////////////////////////////////////////////////////

    fWorld = std::make_unique<Detector::Definition::World>();
    auto& mmsField{fWorld->NewDaughter<Detector::Definition::MMSField>(fCheckOverlap)};
    auto& mmsTTC{mmsField.NewDaughter<Detector::Definition::TTC>(fCheckOverlap)};

    ////////////////////////////////////////////////////////////////
    // Register SDs
    ////////////////////////////////////////////////////////////////

    const auto& ttcName{Detector::Description::TTC::Instance().Name()};
    const auto ttcSiPM(new SD::TTCSiPMSD{ttcName + "SiPM", TTCSiPMSD::Type::MACE});
    mmsTTC.RegisterSD(ttcName + "Scintillator", new SD::TTCSD{ttcName, TTCSD::Type::MACE});
    mmsTTC.RegisterSD(ttcName + "Silicone", ttcSiPM);

    ////////////////////////////////////////////////////////////////
    // Register background fields
    ////////////////////////////////////////////////////////////////

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
    mmsField.RegisterField(std::make_unique<G4FieldManager>(field, chordFinder), false);

    return fWorld->PhysicalVolume();
}

} // namespace MACE::SimTTC::inline Action
