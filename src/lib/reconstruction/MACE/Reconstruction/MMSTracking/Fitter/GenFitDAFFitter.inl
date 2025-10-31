namespace MACE::inline Reconstruction::MMSTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
GenFitDAFFitter<AHit, ATrack>::GenFitDAFFitter(double driftErrorRMS) :
    Base{driftErrorRMS} {}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto GenFitDAFFitter<AHit, ATrack>::operator()(const std::vector<AHitPointer>& hitData, const ASeedPointer& seed) -> Base::template Result<AHitPointer> {
    const auto [genfitTrack, measurementHitMap]{this->Initialize(hitData, seed)};
    if (genfitTrack == nullptr) {
        return {};
    }

    try {
        this->GenFitter().processTrack(genfitTrack.get(), true);
    } catch (const genfit::Exception&) {
        return {};
    }

    return this->Finalize(genfitTrack, seed, measurementHitMap);
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter
