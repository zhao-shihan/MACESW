namespace MACE::inline Reconstruction::MMSTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<Data::CDCSimHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSSimTrack> ATrack>
TruthFinder<AHit, ATrack>::TruthFinder() :
    Base{},
    fMaxVertexRxy{} {
    const auto& cdc{Detector::Description::CDC::Instance()};
    fMaxVertexRxy = cdc.GasInnerRadius();
}

template<Mustard::Data::SuperTupleModel<Data::CDCSimHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSSimTrack> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
auto TruthFinder<AHit, ATrack>::operator()(const std::vector<AHitPointer>& hitData, int) const -> Base::template Result<AHitPointer> {
    using Result = Base::template Result<AHitPointer>;

    if (not this->GoodHitData(hitData) or
        ssize(hitData) < this->MinNHit() or
        GetAs<"x0", CLHEP::Hep3Vector>(*hitData.front()).perp2() > muc::pow(fMaxVertexRxy, 2)) {
        return {.good = {}, .garbage = hitData};
    }

    Result r;
    r.good.reserve(hitData.size() / this->MinNHit());
    r.garbage.reserve(hitData.size());

    std::ranges::subrange track{hitData.cbegin(), hitData.cbegin()};
    const auto CollectGarbage{
        [&] { r.garbage.insert(r.garbage.end(), track.begin(), track.end()); }};

    muc::flat_hash_set<short> cellHit;
    cellHit.reserve(this->MinNHit());

    while (track.end() != hitData.end()) {
        track = {track.end(), std::ranges::upper_bound(track.end(), hitData.end(), *track.end(),
                                                       [](auto&& hit1, auto&& hit2) {
                                                           return Get<"TrkID">(*hit1) < Get<"TrkID">(*hit2);
                                                       })};

        if (std::ranges::ssize(track) < this->MinNHit()) {
            CollectGarbage();
            continue;
        }

        cellHit.clear();
        for (auto&& hit : track) {
            cellHit.emplace(Get<"CellID">(*hit));
        }
        if (ssize(cellHit) < this->MinNHit()) {
            CollectGarbage();
            continue;
        }

        const auto& firstHit{**track.begin()};

        auto outputTrackID{*Get<"TrkID">(firstHit)};
        auto [iGoodTrack, inserted]{r.good.try_emplace(outputTrackID, typename Result::GoodTrack{})};
        while (not inserted) {
            Mustard::PrintWarning(fmt::format("Disordered dataset (track {} has appeared before), attempting to assign track ID {}", outputTrackID, outputTrackID + 1));
            std::tie(iGoodTrack, inserted) = r.good.try_emplace(++outputTrackID, typename Result::GoodTrack{});
        }
        auto& [trackHitData, seed]{iGoodTrack->second};

        trackHitData.reserve(track.size());
        for (auto&& hit : track) {
            trackHitData.emplace_back(hit);
        }

        seed = std::make_shared_for_overwrite<Mustard::Data::Tuple<Data::MMSSimTrack>>();
        Get<"EvtID">(*seed) = Get<"EvtID">(firstHit);
        Get<"TrkID">(*seed) = outputTrackID;
        Get<"HitID">(*seed)->reserve(track.size());
        for (auto&& hit : track) {
            Get<"HitID">(*seed)->emplace_back(Get<"HitID">(*hit));
        }
        Get<"chi2">(*seed) = 0;
        Get<"t0">(*seed) = Get<"t0">(firstHit);
        Get<"PDGID">(*seed) = Get<"PDGID">(firstHit);
        Get<"x0">(*seed) = Get<"x0">(firstHit);
        Get<"Ek0">(*seed) = Get<"Ek0">(firstHit);
        Get<"p0">(*seed) = Get<"p0">(firstHit);
        Data::CalculateHelix(*seed, Detector::Description::MMSField::Instance().FastField());
        Get<"CreatProc">(*seed) = Get<"CreatProc">(firstHit);
    }

    return r;
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder
