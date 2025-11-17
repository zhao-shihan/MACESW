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

#include "MACE/Simulation/Hit/ECALHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

namespace MACE::inline Simulation::inline SD {

class ECALPMSD;

class ECALSD : public G4VSensitiveDetector {
public:
    explicit ECALSD(const G4String& sdName, const ECALPMSD* ecalPMSD = {});

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    const ECALPMSD* fECALPMSD;

    double fEnergyDepositionThreshold;

    muc::flat_hash_map<int, muc::unique_ptrvec<ECALHit>> fSplitHit;
    ECALHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
