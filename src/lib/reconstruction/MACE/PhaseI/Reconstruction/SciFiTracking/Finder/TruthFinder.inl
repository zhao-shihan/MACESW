namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiSimHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
TruthFinder<AHit, ATrack>::TruthFinder() :
    Base{},
    fMaxVertexRxy{} {
    const auto& cdc{Detector::Description::CDC::Instance()};
    fMaxVertexRxy = cdc.GasInnerRadius();
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiSimHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
auto TruthFinder<AHit, ATrack>::operator()(const std::vector<AHitPointer>& hitData, int) const -> Base::template Result<AHitPointer> {
    using Result = Base::template Result<AHitPointer>;

    Result r;

    return r;
}

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder
