#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/Simulation/Analysis/MMSTruthTracker.h++"

#include "muc/algorithm"
#include "muc/hash_set"

#include <cassert>
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

    constexpr auto ByTrackID{
        [](const auto& hit1, const auto& hit2) {
            return Get<"TrkID">(*hit1) < Get<"TrkID">(*hit2);
        }};

    assert(std::ranges::is_sorted(cdcHitHC, ByTrackID));
    assert(std::ranges::is_sorted(ttcHitHC, ByTrackID));

    // find CDC hits coincidence with TTC hits

    muc::shared_ptrvec<Mustard::Data::Tuple<Data::MMSSimTrack>> mmsTrackData;
    mmsTrackData.reserve(cdcHitHC.size() / fTrackFinder.MinNHit());

    std::ranges::subrange trackTTCHit{ttcHitHC.cbegin(), ttcHitHC.cbegin()};
    const auto trackCDCHitFirst{std::ranges::lower_bound(cdcHitHC, ttcHitHC.front(), ByTrackID)};
    std::ranges::subrange trackCDCHit{trackCDCHitFirst, trackCDCHitFirst};

    muc::flat_hash_set<short> tileHit;
    tileHit.reserve(2 * fMinNTTCHitForQualifiedTrack);

    while (trackTTCHit.end() != ttcHitHC.cend() and
           trackCDCHit.end() != cdcHitHC.cend()) {
        trackTTCHit = {trackTTCHit.end(), std::ranges::upper_bound(trackTTCHit.end(), ttcHitHC.cend(), *trackTTCHit.end(), ByTrackID)};
        trackCDCHit = {trackCDCHit.end(), std::ranges::upper_bound(trackCDCHit.end(), cdcHitHC.cend(), trackTTCHit.front(), ByTrackID)};

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
