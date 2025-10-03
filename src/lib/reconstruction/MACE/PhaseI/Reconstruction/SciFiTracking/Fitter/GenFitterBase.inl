namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter {
template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
GenFitterBase<AHit, ATrack, AFitter>::GenFitterBase(double fiberRMS, double lowestMomentum) :
    Base{},
    fFiberRMS{fiberRMS},
    fLowestMomentum{lowestMomentum},
    fEnableEventDisplay{false},
    fEventDisplayTrackStore{},
    fGenFitter{} {
    if (const auto name{"TrackerSystem"};
        gGeoManager == nullptr or std::string_view{gGeoManager->GetName()} != name) {
        MACE::PhaseI::Detector::Definition::World world;
        MACE::PhaseI::Detector::Assembly::TrackerSystem trackersystem{world, false};
        // geant4 -> gdml
        const auto& intraNodeComm{Mustard::Env::MPIEnv::Instance().IntraNodeComm()};
        std::filesystem::path gdmlFSPath;
        std::filesystem::path::string_type gdmlPath;
        if (intraNodeComm.rank() == 0) {
            gdmlFSPath = Mustard::CreateTemporaryFile("trackersystem_temp", ".gdml");
            world.Export(gdmlFSPath);
            gdmlPath = gdmlFSPath;
        }
        auto gdmlPathLength{gdmlPath.length()};
        intraNodeComm.bcast(0, gdmlPathLength);
        gdmlPath.resize(gdmlPathLength);
        intraNodeComm.bcast(0, gdmlPath.data(), mplr::vector_layout<std::filesystem::path::value_type>{gdmlPathLength});
        // gdml -> root
        TGeoManager::Import(gdmlPath.c_str());
        gGeoManager->SetName(name);
        gGeoManager->GetTopVolume()->SetInvisible();
        // remove gdml
        intraNodeComm.barrier();
        if (intraNodeComm.rank() == 0) {
            std::error_code ec;
            std::filesystem::remove(gdmlFSPath, ec);
        }
    }

    // setup genfit
    if (const auto materialEffects{genfit::MaterialEffects::getInstance()};
        not materialEffects->isInitialized()) {
        materialEffects->init(new genfit::TGeoMaterialInterface);
    }
    if (const auto fieldManager{genfit::FieldManager::getInstance()};
        not fieldManager->isInitialized()) {
        fieldManager->init(new genfit::ConstField(0., 0., 0.));
    }
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto GenFitterBase<AHit, ATrack, AFitter>::Initialize(const std::vector<AHitPointer>& hitData, ASeedPointer seed)
    -> std::pair<std::shared_ptr<genfit::Track>,
                 muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer>> {
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();

    TVector3 pos(Get<"x">(*seed)[0] * 0.1, Get<"x">(*seed)[1] * 0.1, Get<"x">(*seed)[2] * 0.1);
    TVector3 mom(Get<"p">(*seed)[0] / 20, Get<"p">(*seed)[1] / 20, Get<"p">(*seed)[2] / 20);
    int pid = 11;

    muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer> measurementHitMap;
    measurementHitMap.reserve(hitData.size());
    const auto genfitTrack{
        std::make_shared<genfit::Track>(new genfit::RKTrackRep{pid}, // track rep will be deleted when genfit::Track destructs
                                        Mustard::ToG3<"Length">(this->ToTVector3(*Get<"x">(*seed))),
                                        mom)};

    for (auto&& hit : hitData) {
        const auto measurement{
            [&]() -> genfit::AbsMeasurement* {
                if (fiberMap[*Get<"SiPMID">(*hit)].layerType == "Transverse") {
                    double x = fiberMap[*Get<"SiPMID">(*hit)].radius * std::cos(fiberMap[*Get<"SiPMID">(*hit)].rotationAngle);
                    double y = fiberMap[*Get<"SiPMID">(*hit)].radius * std::sin(fiberMap[*Get<"SiPMID">(*hit)].rotationAngle);
                    double z = sciFiTracker.FiberLength() / 2;
                    TVector3 startPoint(x, y, -z);
                    TVector3 endPoint(x, y, z);
                    TVectorD rawHitCoords(7);
                    rawHitCoords[0] = Mustard::ToG3<"Length">(startPoint.x());
                    rawHitCoords[1] = Mustard::ToG3<"Length">(startPoint.y());
                    rawHitCoords[2] = Mustard::ToG3<"Length">(startPoint.z());
                    rawHitCoords[3] = Mustard::ToG3<"Length">(endPoint.x());
                    rawHitCoords[4] = Mustard::ToG3<"Length">(endPoint.y());
                    rawHitCoords[5] = Mustard::ToG3<"Length">(endPoint.z());
                    rawHitCoords[6] = Mustard::ToG3<"Length">(0);

                    TMatrixDSym rawHitCov(7);
                    rawHitCov.UnitMatrix();
                    rawHitCov(6, 6) = 0.0289 * 0.0289;

                    return new genfit::WireMeasurement{rawHitCoords, rawHitCov,
                                                       *Get<"SiPMID">(*hit), Get<"HitID">(*hit),
                                                       nullptr};
                } else {
                    TMatrixDSym hitCov(8);
                    hitCov.UnitMatrix();
                    hitCov(7, 7) = 0.0289 * 0.0289;
                    TVectorD hitCoords(8);
                    hitCoords[0] = 0.;                                                                                                       // x
                    hitCoords[1] = 0.;                                                                                                       // y
                    hitCoords[2] = 0.;                                                                                                       // z
                    hitCoords[3] = Mustard::ToG3<"Length">(fiberMap[*Get<"SiPMID">(*hit)].radius);                                           // r
                    hitCoords[4] = std::copysign(Mustard::ToG3<"Length">(sciFiTracker.FiberLength()), fiberMap[*Get<"SiPMID">(*hit)].pitch); // pitch
                    hitCoords[5] = fiberMap[*Get<"SiPMID">(*hit)].rotationAngle;                                                             // phi0
                    hitCoords[6] = 2 * std::numbers::pi;                                                                                     // phiTotal
                    hitCoords[7] = 0;                                                                                                        //

                    return new genfit::HelixMeasurement(hitCoords, hitCov, *Get<"SiPMID">(*hit), Get<"HitID">(*hit), nullptr);
                }
            }()};
        genfitTrack->insertPoint(new genfit::TrackPoint{measurement, genfitTrack.get()});
        measurementHitMap.emplace(measurement, hit);
    }
    // init fitter
    // auto fitter = new genfit::KalmanFitterRefTrack();

    // particle pdg code;
    // const int pdg = -11;

    // // trackrep
    // genfit::AbsTrackRep* rep = new genfit::RKTrackRep(pdg);

    // // create track
    // const auto genfitTrack{std::make_shared<genfit::Track>(rep, pos, mom)};
    // int detID = 0;
    // int hitID = 0;

    // for (auto&& hit : hitData) {
    //     if (fiberMap[*Get<"SiPMID">(*hit)].layerType == "Transverse") {
    //         double x = fiberMap[*Get<"SiPMID">(*hit)].radius * std::cos(fiberMap[*Get<"SiPMID">(*hit)].rotationAngle);
    //         double y = fiberMap[*Get<"SiPMID">(*hit)].radius * std::sin(fiberMap[*Get<"SiPMID">(*hit)].rotationAngle);
    //         double z = sciFiTracker.FiberLength() / 2;
    //         TVector3 startPoint(x, y, -z);
    //         TVector3 endPoint(x, y, z);
    //         TVectorD rawHitCoords(7);
    //         rawHitCoords[0] = Mustard::ToG3<"Length">(startPoint.x());
    //         rawHitCoords[1] = Mustard::ToG3<"Length">(startPoint.y());
    //         rawHitCoords[2] = Mustard::ToG3<"Length">(startPoint.z());
    //         rawHitCoords[3] = Mustard::ToG3<"Length">(endPoint.x());
    //         rawHitCoords[4] = Mustard::ToG3<"Length">(endPoint.y());
    //         rawHitCoords[5] = Mustard::ToG3<"Length">(endPoint.z());
    //         rawHitCoords[6] = Mustard::ToG3<"Length">(0);

    // TMatrixDSym rawHitCov(7);
    // rawHitCov.UnitMatrix();
    // rawHitCov(6, 6) = 0.0289 * 0.0289;

    // genfit::WireMeasurement* measurement = new genfit::WireMeasurement(rawHitCoords, rawHitCov, detID++, hitID++, nullptr);
    // measurementHitMap.emplace(measurement, hit);
    // genfitTrack->insertPoint(new genfit::TrackPoint(measurement, genfitTrack.get()));
    // } else {
    // TMatrixDSym hitCov(8);
    // hitCov.UnitMatrix();
    // hitCov(7, 7) = 0.0289 * 0.0289; // 漂移距离的方差
    // TVectorD hitCoords(8);
    // hitCoords[0] = 0.;                                                                                    // x
    // hitCoords[1] = 0.;                                                                                    // y
    // hitCoords[2] = 0.;                                                                                    // z
    // hitCoords[3] = fiberMap[*Get<"SiPMID">(*hit)].radius * 0.1;                                           // r
    // hitCoords[4] = std::copysign(sciFiTracker.FiberLength() * 0.1, fiberMap[*Get<"SiPMID">(*hit)].pitch); // pitch
    // hitCoords[5] = fiberMap[*Get<"SiPMID">(*hit)].rotationAngle;                                          // phi0
    // hitCoords[6] = 2 * std::numbers::pi;                                                                  // phiTotal
    // hitCoords[7] = 0;                                                                                     //

    // genfit::HelixMeasurement* measurement = new genfit::HelixMeasurement(hitCoords, hitCov, detID++, hitID++, nullptr);
    // measurementHitMap.emplace(measurement, hit);
    // genfitTrack->insertPoint(new genfit::TrackPoint(measurement, genfitTrack.get()));
    // }
    // }
    return {genfitTrack, measurementHitMap};
}
template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto GenFitterBase<AHit, ATrack, AFitter>::Finalize(std::shared_ptr<genfit::Track> genfitTrack, ASeedPointer seed,
                                                    const muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer>& measurementHitMap)
    -> Base::template Result<AHitPointer> {

    const genfit::MeasuredStateOnPlane* firstState;
    try {
        firstState = &genfitTrack->getFittedState();
    } catch (const genfit::Exception&) {
        return {};
    }
    const auto cardinalRep{genfitTrack->getCardinalRep()};
    const auto& allPoint{genfitTrack->getPointsWithMeasurement()};
    std::vector<AHitPointer> fitted;
    fitted.reserve(allPoint.size());
    std::vector<AHitPointer> failed;
    failed.reserve(allPoint.size());
    for (gsl::index i{}; i < ssize(allPoint); ++i) {
        const auto fit{allPoint[i]->hasFitterInfo(cardinalRep)};
        (fit ? fitted : failed)
            .emplace_back(measurementHitMap.at(allPoint[i]->getRawMeasurement()));
    }
    const auto t0{Get<"t">(*seed)};
    const auto x0{Mustard::ToG4<"Length">(firstState->getPos())};
    const auto p0{Mustard::ToG4<"Energy">(firstState->getMom()).Unit()};
    // double mag = p0.Mag();
    // p0[0] /= mag;
    // p0[1] /= mag;
    // p0[2] /= mag;

    auto track{std::make_shared_for_overwrite<Mustard::Data::Tuple<ATrack>>()};
    Get<"EvtID">(*track) = Get<"EvtID">(*seed);
    Get<"x">(*track) = this->template FromTVector3<muc::array3d>(x0);
    Get<"p">(*track) = this->template FromTVector3<muc::array3d>(p0);
    Get<"t">(*track) = t0;
    return {std::move(track), std::move(fitted), std::move(failed)};
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
auto GenFitterBase<AHit, ATrack, AFitter>::OpenEventDisplay(bool clearUponClose) -> void {
    genfit::EventDisplay::getInstance()->open();
    if (clearUponClose) {
        ClearEventDisplayTrackStore();
    }
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<Mustard::Concept::NumericVector3FloatingPoint T>
MUSTARD_ALWAYS_INLINE auto GenFitterBase<AHit, ATrack, AFitter>::ToTVector3(T src) -> TVector3 {
    TVector3 dest;
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    return dest;
}

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<Mustard::Concept::NumericVector3FloatingPoint T>
MUSTARD_ALWAYS_INLINE auto GenFitterBase<AHit, ATrack, AFitter>::FromTVector3(const TVector3& src) -> T {
    T dest;
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    return dest;
}
} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter
