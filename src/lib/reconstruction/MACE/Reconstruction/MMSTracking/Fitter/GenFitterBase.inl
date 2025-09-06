namespace MACE::inline Reconstruction::MMSTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
GenFitterBase<AHit, ATrack, AFitter>::GenFitterBase(double driftErrorRMS, double lowestMomentum) :
    Base{},
    fDriftErrorRMS{driftErrorRMS},
    fLowestMomentum{lowestMomentum},
    fEnableEventDisplay{false},
    fEventDisplayTrackStore{},
    fGenFitter{} {
    if (const auto name{"MACEMMS"};
        gGeoManager == nullptr or std::string_view{gGeoManager->GetName()} != name) {
        // geant4 geometry
        Detector::Definition::World world;
        Detector::Assembly::MMS mms{world, false};
        mms.Get<Detector::Definition::CDCGas>().RemoveDaughter<Detector::Definition::CDCSuperLayer>();
        // geant4 -> gdml
        const auto& intraNodeComm{Mustard::Env::MPIEnv::Instance().IntraNodeComm()};
        std::filesystem::path gdmlFSPath;
        std::filesystem::path::string_type gdmlPath;
        if (intraNodeComm.rank() == 0) {
            gdmlFSPath = Mustard::CreateTemporaryFile("mms_temp", ".gdml");
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
        fieldManager->init(new GenFitMMSField);
    }
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto GenFitterBase<AHit, ATrack, AFitter>::Initialize(const std::vector<AHitPointer>& hitData, ASeedPointer seed)
    -> std::pair<std::shared_ptr<genfit::Track>,
                 muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer>> {
    if (Mustard::Math::NormSq(*Get<"p0">(*seed)) < muc::pow(fLowestMomentum, 2)) {
        return {};
    }
    if (TDatabasePDG::Instance()->GetParticle(Get<"PDGID">(*seed)) == nullptr) {
        return {};
    }

    const auto genfitTrack{
        std::make_shared<genfit::Track>(new genfit::RKTrackRep{Get<"PDGID">(*seed)}, // track rep will be deleted when genfit::Track destructs
                                        Mustard::ToG3<"Length">(this->ToTVector3(*Get<"x0">(*seed))),
                                        Mustard::ToG3<"Energy">(this->ToTVector3(*Get<"p0">(*seed))))};

    muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer> measurementHitMap;
    measurementHitMap.reserve(hitData.size());

    const auto& cdc{Detector::Description::CDC::Instance()};
    const auto& cellMap{cdc.CellMap()};
    for (auto&& hit : hitData) {
        const auto& cell{cellMap.at(Get<"CellID">(*hit))};
        const Eigen::Vector3d wirePosition{cell.position.x(), cell.position.y(), 0};
        const Eigen::Vector3d wireStartPoint{wirePosition - cell.senseWireHalfLength * cell.direction};
        const Eigen::Vector3d wireEndPoint{wirePosition + cell.senseWireHalfLength * cell.direction};
        const auto measurement{[&]() -> genfit::AbsMeasurement* {
            if (not fEnableEventDisplay) {
                return new genfit::WireMeasurementNew{Mustard::ToG3<"Length">(*Get<"d">(*hit)),
                                                      Mustard::ToG3<"Length">(this->DriftErrorRMS()),
                                                      this->ToTVector3(Mustard::ToG3<"Length">(wireStartPoint)),
                                                      this->ToTVector3(Mustard::ToG3<"Length">(wireEndPoint)),
                                                      cell.cellID,
                                                      Get<"HitID">(*hit),
                                                      nullptr};
            } else {
                TVectorD rawHitCoords(7);
                rawHitCoords[0] = Mustard::ToG3<"Length">(wireStartPoint.x());
                rawHitCoords[1] = Mustard::ToG3<"Length">(wireStartPoint.y());
                rawHitCoords[2] = Mustard::ToG3<"Length">(wireStartPoint.z());
                rawHitCoords[3] = Mustard::ToG3<"Length">(wireEndPoint.x());
                rawHitCoords[4] = Mustard::ToG3<"Length">(wireEndPoint.y());
                rawHitCoords[5] = Mustard::ToG3<"Length">(wireEndPoint.z());
                rawHitCoords[6] = Mustard::ToG3<"Length">(*Get<"d">(*hit));

                TMatrixDSym rawHitCov(7);
                const auto varD{muc::pow(Mustard::ToG3<"Length">(this->DriftErrorRMS()), 2)};
                rawHitCov(0, 0) = varD;
                rawHitCov(1, 1) = varD;
                rawHitCov(2, 2) = varD;
                rawHitCov(3, 3) = varD;
                rawHitCov(4, 4) = varD;
                rawHitCov(5, 5) = varD;
                rawHitCov(6, 6) = varD;

                return new genfit::WireMeasurement{rawHitCoords, rawHitCov,
                                                   cell.cellID, Get<"HitID">(*hit),
                                                   nullptr};
            }
        }()};
        genfitTrack->insertPoint(new genfit::TrackPoint{measurement, genfitTrack.get()});
        measurementHitMap.emplace(measurement, hit);
    }

    return {genfitTrack, measurementHitMap};
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto GenFitterBase<AHit, ATrack, AFitter>::Finalize(std::shared_ptr<genfit::Track> genfitTrack, ASeedPointer seed,
                                                    const muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer>& measurementHitMap)
    -> Base::template Result<AHitPointer> {
    const auto& status{*genfitTrack->getFitStatus()};
    if (not status.isFitConvergedPartially()) {
        return {};
    }

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

    const auto t0{Get<"t0">(*seed)};
    const auto x0{Mustard::ToG4<"Length">(firstState->getPos())};
    const auto p0{Mustard::ToG4<"Energy">(firstState->getMom())};
    const auto mass{Mustard::ToG4<"Energy">(firstState->getMass())};
    const auto pdgID{firstState->getPDG()};
    const auto ek0{std::sqrt(p0.Mag2() + muc::pow(mass, 2)) - mass};

    auto track{std::make_shared_for_overwrite<Mustard::Data::Tuple<ATrack>>()};
    Get<"EvtID">(*track) = Get<"EvtID">(*seed);
    Get<"TrkID">(*track) = Get<"TrkID">(*seed);
    Get<"HitID">(*track)->resize(fitted.size());
    std::ranges::transform(fitted, Get<"HitID">(*track)->begin(),
                           [](auto&& hit) { return Get<"HitID">(*hit); });
    Get<"chi2">(*track) = status.getChi2() / status.getNdf();
    Get<"t0">(*track) = t0;
    Get<"PDGID">(*track) = pdgID;
    Get<"x0">(*track) = this->template FromTVector3<muc::array3d>(x0);
    Get<"Ek0">(*track) = ek0;
    Get<"p0">(*track) = this->template FromTVector3<muc::array3d>(p0);
    Data::CalculateHelix(*track, Detector::Description::MMSField::Instance().FastField());

    if (fEnableEventDisplay) {
        genfit::EventDisplay::getInstance()->addEvent(genfitTrack.get());
        fEventDisplayTrackStore.emplace_back(std::move(genfitTrack)); // genfitTrack is MOVED here
    }

    return {std::move(track), std::move(fitted), std::move(failed)};
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
auto GenFitterBase<AHit, ATrack, AFitter>::OpenEventDisplay(bool clearUponClose) -> void {
    genfit::EventDisplay::getInstance()->open();
    if (clearUponClose) {
        ClearEventDisplayTrackStore();
    }
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<Mustard::Concept::NumericVector3FloatingPoint T>
MUSTARD_ALWAYS_INLINE auto GenFitterBase<AHit, ATrack, AFitter>::ToTVector3(T src) -> TVector3 {
    TVector3 dest;
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    return dest;
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
template<Mustard::Concept::NumericVector3FloatingPoint T>
MUSTARD_ALWAYS_INLINE auto GenFitterBase<AHit, ATrack, AFitter>::FromTVector3(const TVector3& src) -> T {
    T dest;
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    return dest;
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter
