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

#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/Simulation/Analysis/MMSTruthTracker.h++"

#include "gsl/gsl"

#include <utility>

namespace MACE::inline Simulation::Analysis {

MMSTruthTracker::MMSTruthTracker() :
    fMinNTTCHitForQualifiedTrack{2},
    fTrackFinder{},
    fMessengerRegister{this} {
    const auto& cdc{Detector::Description::CDC::Instance()};
    fTrackFinder.MinNHit(cdc.NSenseLayerPerSuper() * cdc.NSuperLayer());
    fTrackFinder.MaxVertexRxy(cdc.GasInnerRadius());
}

auto MMSTruthTracker::operator()(const std::vector<gsl::owner<CDCHit*>>& cdcHitHC,
                                 const std::vector<gsl::owner<TTCHit*>>& ttcHitHC) -> muc::shared_ptrvec<Mustard::Data::Tuple<Data::MMSSimTrack>> {
    if (ssize(cdcHitHC) < fTrackFinder.MinNHit() or
        ssize(ttcHitHC) < fMinNTTCHitForQualifiedTrack) {
        return {};
    }

    constexpr auto byTrackID{
        [](const auto& hit1, const auto& hit2) {
            return Get<"TrkID">(*hit1) < Get<"TrkID">(*hit2);
        }};

    Expects(std::ranges::is_sorted(cdcHitHC, byTrackID));
    Expects(std::ranges::is_sorted(ttcHitHC, byTrackID));

    // find CDC hits coincidence with TTC hits

    muc::shared_ptrvec<Mustard::Data::Tuple<Data::MMSSimTrack>> mmsTrackData;
    mmsTrackData.reserve(cdcHitHC.size() / fTrackFinder.MinNHit());

    std::ranges::subrange trackTTCHit{ttcHitHC.cbegin(), ttcHitHC.cbegin()};
    const auto trackCDCHitFirst{std::ranges::lower_bound(cdcHitHC, ttcHitHC.front(), byTrackID)};
    std::ranges::subrange trackCDCHit{trackCDCHitFirst, trackCDCHitFirst};

    muc::flat_hash_set<short> tileHit;
    tileHit.reserve(2 * fMinNTTCHitForQualifiedTrack);

    while (trackTTCHit.end() != ttcHitHC.cend() and
           trackCDCHit.end() != cdcHitHC.cend()) {
        trackTTCHit = {trackTTCHit.end(), std::ranges::upper_bound(trackTTCHit.end(), ttcHitHC.cend(), *trackTTCHit.end(), byTrackID)};
        trackCDCHit = {trackCDCHit.end(), std::ranges::upper_bound(trackCDCHit.end(), cdcHitHC.cend(), trackTTCHit.front(), byTrackID)};

        if (std::ranges::ssize(trackTTCHit) < fMinNTTCHitForQualifiedTrack or
            std::ranges::ssize(trackCDCHit) < fTrackFinder.MinNHit() or
            GetAs<"x0", G4ThreeVector>(**trackCDCHit.begin()).perp2() > muc::pow(fTrackFinder.MaxVertexRxy(), 2)) {
            continue;
        }

        tileHit.clear();
        for (auto&& hit : trackTTCHit) {
            tileHit.emplace(Get<"TileID">(*hit));
        }
        if (ssize(tileHit) < fMinNTTCHitForQualifiedTrack) {
            continue;
        }

        auto mmsTrack{fTrackFinder(std::vector(trackCDCHit.begin(), trackCDCHit.end())).good};
        if (mmsTrack.empty()) {
            continue;
        }

        for (auto&& hit : trackTTCHit) {
            Get<"Good">(*hit) = true;
        }
        for (auto&& [_, track] : mmsTrack) {
            for (auto&& hit : track.hitData) {
                Get<"Good">(*hit) = true;
            }
            mmsTrackData.emplace_back(std::move(track.seed));
        }
    }

    return mmsTrackData;
}

} // namespace MACE::inline Simulation::Analysis
