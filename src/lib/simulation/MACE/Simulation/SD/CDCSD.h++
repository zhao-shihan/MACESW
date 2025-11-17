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

#include "MACE/Data/MMSTrack.h++"
#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Simulation/Hit/CDCHit.h++"
#include "MACE/Simulation/SD/CDCSDMessenger.h++"

#include "Mustard/Data/Tuple.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

#include <algorithm>
#include <vector>

namespace MACE::inline Simulation::inline SD {

class CDCSD : public G4VSensitiveDetector {
public:
    explicit CDCSD(const G4String& sdName);

    auto IonizingEnergyDepositionThreshold(double e) -> void { fIonizingEnergyDepositionThreshold = std::max(0., e); }

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    double fIonizingEnergyDepositionThreshold;

    double fMeanDriftVelocity;
    const std::vector<Detector::Description::CDC::CellInformation>* fCellMap;

    muc::flat_hash_map<int, muc::unique_ptrvec<CDCHit>> fSplitHit;
    CDCHitCollection* fHitsCollection;

    CDCSDMessenger::Register<CDCSD> fMessengerRegister;
};

} // namespace MACE::inline Simulation::inline SD
