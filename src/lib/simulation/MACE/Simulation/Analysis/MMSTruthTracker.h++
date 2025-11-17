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
#include "MACE/Reconstruction/MMSTracking/Finder/TruthFinder.h++"
#include "MACE/Simulation/Analysis/MMSTruthTrackerMessenger.h++"
#include "MACE/Simulation/Hit/CDCHit.h++"
#include "MACE/Simulation/Hit/TTCHit.h++"

#include "Mustard/Data/Tuple.h++"

#include "G4ThreeVector.hh"

#include "muc/math"
#include "muc/ptrvec"

#include "gsl/gsl"

#include <algorithm>
#include <iterator>
#include <ranges>

namespace MACE::inline Simulation::Analysis {

class MMSTruthTracker {
public:
    MMSTruthTracker();

    auto MinNCDCHitForQualifiedTrack(int n) -> void { fTrackFinder.MinNHit(std::max(1, n)); }
    auto MinNTTCHitForQualifiedTrack(int n) -> void { fMinNTTCHitForQualifiedTrack = std::max(1, n); }

    auto operator()(const std::vector<gsl::owner<CDCHit*>>& cdcHitHC,
                    const std::vector<gsl::owner<TTCHit*>>& ttcHitHC) -> muc::shared_ptrvec<Mustard::Data::Tuple<Data::MMSSimTrack>>;

private:
    int fMinNTTCHitForQualifiedTrack;

    MMSTracking::TruthFinder<> fTrackFinder;

    MMSTruthTrackerMessenger::Register<MMSTruthTracker> fMessengerRegister;
};

} // namespace MACE::inline Simulation::Analysis
