namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
FinderBase<ASciFiHit, ATrack>::FinderBase() :
    fMinNHit{} {}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
FinderBase<ASciFiHit, ATrack>::~FinderBase() = default;

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
auto FinderBase<ASciFiHit, ATrack>::GoodHitData(const std::vector<AHitPointer>& hitData) -> bool {
    if (hitData.empty()) [[unlikely]] {
        Mustard::PrintWarning("Empty hit data");
        return false;
    }

    auto good{true};

    muc::flat_hash_set<int> eventID;
    eventID.reserve(hitData.size());
    for (auto&& hit : hitData) {
        eventID.emplace(Get<"EvtID">(*hit));
    }
    if (eventID.size() != 1) [[unlikely]] {
        Mustard::PrintWarning(fmt::format("Hit data include different event IDs {}, skipping", eventID));
        good = false;
    }

    return good;
}
} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder
