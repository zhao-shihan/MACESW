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

#include "MACE/PhaseI/SimMACEPhaseI/SD/MRPCSDMessenger.h++"
#include "MACE/PhaseI/Simulation/Hit/MRPCHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/hash_map"
#include "muc/ptrvec"

#include <algorithm>

class G4DataInterpolation;

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

class MRPCSD : public G4VSensitiveDetector {
public:
    explicit MRPCSD(const G4String& sdName);
    ~MRPCSD() override;

    auto IonizingEnergyDepositionThreshold(double e) -> void { fIonizingEnergyDepositionThreshold = std::max(0., e); }

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    double fIonizingEnergyDepositionThreshold;

    muc::flat_hash_map<int, muc::unique_ptrvec<MRPCHit>> fSplitHit;
    MRPCHitCollection* fHitsCollection;

    MRPCSDMessenger::Register<MRPCSD> fMessengerRegister;
};

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
