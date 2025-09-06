namespace MACE::inline Reconstruction::MMSTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
FinderBase<AHit, ATrack>::FinderBase() :
    fMinNHit{} {
    const auto& cdc{Detector::Description::CDC::Instance()};
    fMinNHit = cdc.NSenseLayerPerSuper() * cdc.NSuperLayer();
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
FinderBase<AHit, ATrack>::~FinderBase() = default;

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
auto FinderBase<AHit, ATrack>::GoodHitData(const std::vector<AHitPointer>& hitData) -> bool {
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

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder
