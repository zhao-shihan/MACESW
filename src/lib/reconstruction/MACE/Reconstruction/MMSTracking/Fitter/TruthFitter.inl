namespace MACE::inline Reconstruction::MMSTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<Data::CDCSimHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSSimTrack> ATrack>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto TruthFitter<AHit, ATrack>::operator()(const std::vector<AHitPointer>& hitData, ASeedPointer seed) -> std::shared_ptr<Mustard::Data::Tuple<ATrack>> {
    const auto track{std::make_shared_for_overwrite<Mustard::Data::Tuple<ATrack>>()};

    const auto& firstHit{*hitData.front()};
    Get<"EvtID">(*track) = Get<"EvtID">(firstHit);
    Get<"TrkID">(*track) = Get<"TrkID">(*seed);
    Get<"HitID">(*track)->reserve(hitData.size());
    for (auto&& hit : hitData) {
        Get<"HitID">(*track)->emplace_back(Get<"HitID">(*hit));
    }
    Get<"chi2">(*track) = 0;
    Get<"t0">(*track) = Get<"t0">(firstHit);
    Get<"PDGID">(*track) = Get<"PDGID">(firstHit);
    Get<"x0">(*track) = Get<"x0">(firstHit);
    Get<"Ek0">(*track) = Get<"Ek0">(firstHit);
    Get<"p0">(*track) = Get<"p0">(firstHit);
    Data::CalculateHelix(*track, Detector::Description::MMSField::Instance().FastField());
    Get<"CreatProc">(*track) = Get<"CreatProc">(firstHit);

    if (not fCheckHitDataConsistency) {
        return track;
    }

#define MACE_RECONSTRUCTION_MMSTRACKING_FITTER_TRUTHFITTER_HIT_DATA_CONSISTENCY_CHECK(cond) \
    if (cond) {                                                                             \
        Mustard::Throw<std::invalid_argument>(#cond);                                       \
    }
    for (auto&& hit : hitData) {
        MACE_RECONSTRUCTION_MMSTRACKING_FITTER_TRUTHFITTER_HIT_DATA_CONSISTENCY_CHECK(Get<"EvtID">(*hit) != Get<"EvtID">(firstHit))
        MACE_RECONSTRUCTION_MMSTRACKING_FITTER_TRUTHFITTER_HIT_DATA_CONSISTENCY_CHECK(Get<"TrkID">(*hit) != Get<"TrkID">(firstHit))
        MACE_RECONSTRUCTION_MMSTRACKING_FITTER_TRUTHFITTER_HIT_DATA_CONSISTENCY_CHECK(Get<"PDGID">(*hit) != Get<"PDGID">(firstHit))
        MACE_RECONSTRUCTION_MMSTRACKING_FITTER_TRUTHFITTER_HIT_DATA_CONSISTENCY_CHECK(Get<"CreatProc">(*hit) != Get<"CreatProc">(firstHit))
    }
#undef MACE_RECONSTRUCTION_MMSTRACKING_FITTER_TRUTHFITTER_HIT_DATA_CONSISTENCY_CHECK

    return track;
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter
