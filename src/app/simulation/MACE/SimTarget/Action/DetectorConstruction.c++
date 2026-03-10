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

#include "MACE/Detector/Definition/BeamDegrader.h++"
#include "MACE/Detector/Definition/BeamMonitor.h++"
#include "MACE/Detector/Definition/Target.h++"
#include "MACE/Detector/Definition/World.h++"
#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/SimTarget/Action/DetectorConstruction.h++"
#include "MACE/SimTarget/Messenger/DetectorMessenger.h++"

#include "Mustard/Detector/Description/DescriptionIO.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4NistManager.hh"

namespace MACE::SimTarget::inline Action {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Density;
using namespace Mustard::LiteralUnit::Temperature;

DetectorConstruction::DetectorConstruction() :
    PassiveSingleton{this},
    G4VUserDetectorConstruction{},
    fCheckOverlap{true},
    fWorld{} {
    DetectorMessenger::EnsureInstantiation();
    auto& world{Detector::Description::World::Instance()};
    world.HalfXExtent(15_cm);
    world.HalfYExtent(15_cm);
    world.HalfZExtent(15_cm);
}

auto DetectorConstruction::Construct() -> G4VPhysicalVolume* {
    using namespace MACE::Detector::Definition;

    // AcceleratorField is target's mother by default, modified it to adapt global frame
    auto& accelerator{Detector::Description::Accelerator::Instance()};
    accelerator.DecelerateFieldLength(0);
    accelerator.AccelerateFieldLength(0);
    accelerator.MaxPotentialPosition(0);

    fWorld = std::make_unique<World>();
    fWorld->NewDaughter<BeamMonitor>(fCheckOverlap);
    fWorld->NewDaughter<BeamDegrader>(fCheckOverlap);
    fWorld->NewDaughter<Target>(fCheckOverlap);

    fWorld->LogicalVolume()->SetMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"));

    return fWorld->PhysicalVolume();
}

} // namespace MACE::SimTarget::inline Action
