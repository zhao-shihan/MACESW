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

#include "MACE/Simulation/Hit/MCPHit.h++"

#include "G4VSensitiveDetector.hh"

#include "muc/ptrvec"

#include <algorithm>

class G4DataInterpolation;

namespace MACE::inline Simulation::inline SD {

class MCPSD : public G4VSensitiveDetector {
public:
    explicit MCPSD(const G4String& sdName);
    ~MCPSD() override;

    auto Initialize(G4HCofThisEvent* hitsCollection) -> void override;
    auto ProcessHits(G4Step* theStep, G4TouchableHistory*) -> G4bool override;
    auto EndOfEvent(G4HCofThisEvent*) -> void override;

protected:
    std::unique_ptr<G4DataInterpolation> fEfficiency;

    muc::unique_ptrvec<MCPHit> fSplitHit;
    MCPHitCollection* fHitsCollection;
};

} // namespace MACE::inline Simulation::inline SD
