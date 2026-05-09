namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
GenFitSimpleKalmanFitter<AHit, ATrack>::GenFitSimpleKalmanFitter(double fiberRMS) :
    Base{fiberRMS} {}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto GenFitSimpleKalmanFitter<AHit, ATrack>::operator()(const std::vector<AHitPointer>& hitData, ASeedPointer& seed) -> Base::template Result<AHitPointer> {
    const auto [genfitTrack, measurementHitMap]{this->Initialize(hitData, seed)};
    if (genfitTrack == nullptr) {
        return {};
    }

    try {
        this->GenFitter().processTrack(genfitTrack.get());
    } catch (const genfit::Exception&) {
        return {};
    }

    return this->Finalize(genfitTrack, seed, measurementHitMap);
}
} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter
