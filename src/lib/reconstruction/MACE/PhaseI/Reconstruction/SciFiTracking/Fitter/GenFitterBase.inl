// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

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
        MACE::PhaseI::Detector::Definition::World world{};
        MACE::PhaseI::Detector::Assembly::TrackerSystem trackersystem{world, false};
        // geant4 -> gdml
        const auto& intraNodeComm{Mustard::Env::MPIEnv::Instance().IntraNodeComm()};
        std::filesystem::path gdmlFSPath{};
        std::filesystem::path::string_type gdmlPath{};
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

    TVector3 mom{Get<"p">(*seed)[0] / 20, Get<"p">(*seed)[1] / 20, Get<"p">(*seed)[2] / 20};
    const int pid{11};

    muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer> measurementHitMap;
    measurementHitMap.reserve(hitData.size());
    const auto genfitTrack{
        std::make_shared<genfit::Track>(new genfit::RKTrackRep{pid}, // track rep will be deleted when genfit::Track destructs
                                        Mustard::ToG3<"Length">(this->ToTVector3(*Get<"x">(*seed))), mom)};
    struct BySiPMID {
        auto operator()(const AHitPointer& hit1, const AHitPointer& hit2) const -> bool {
            return *Get<"FiberID">(*hit1) < *Get<"FiberID">(*hit2);
        }
    };
    std::set<AHitPointer, BySiPMID> dataSet{};
    for (auto&& hit : std::as_const(hitData)) {
        dataSet.insert(hit);
    }
    for (auto&& hit : std::as_const(dataSet)) {
        const auto measurement{
            [&]() -> genfit::AbsMeasurement* {
                if (fiberMap[*Get<"FiberID">(*hit)].layerType == "Axial") {
                    double x{fiberMap[*Get<"FiberID">(*hit)].radius * std::cos(fiberMap[*Get<"FiberID">(*hit)].rotationAngle)};
                    double y{fiberMap[*Get<"FiberID">(*hit)].radius * std::sin(fiberMap[*Get<"FiberID">(*hit)].rotationAngle)};
                    double z{sciFiTracker.FiberLength() / 2};
                    TVector3 startPoint{x, y, -z};
                    TVector3 endPoint{x, y, z};
                    TVectorD rawHitCoords{7};
                    rawHitCoords[0] = Mustard::ToG3<"Length">(startPoint.x());
                    rawHitCoords[1] = Mustard::ToG3<"Length">(startPoint.y());
                    rawHitCoords[2] = Mustard::ToG3<"Length">(startPoint.z());
                    rawHitCoords[3] = Mustard::ToG3<"Length">(endPoint.x());
                    rawHitCoords[4] = Mustard::ToG3<"Length">(endPoint.y());
                    rawHitCoords[5] = Mustard::ToG3<"Length">(endPoint.z());
                    rawHitCoords[6] = Mustard::ToG3<"Length">(0);

                    TMatrixDSym rawHitCov{7};
                    rawHitCov.UnitMatrix();
                    rawHitCov(0, 0) = 0.0;
                    rawHitCov(1, 1) = 0.0;
                    rawHitCov(2, 2) = 0.0;
                    rawHitCov(3, 3) = 0.0;
                    rawHitCov(4, 4) = 0.0;
                    rawHitCov(5, 5) = 0.0;
                    rawHitCov(6, 6) = 0.0289 * 0.0289;

                    return new genfit::WireMeasurement{rawHitCoords, rawHitCov,
                                                       *Get<"FiberID">(*hit), Get<"HitID">(*hit),
                                                       nullptr};
                } else {
                    TVectorD rawHitCoords{8};
                    rawHitCoords[0] = 0.;                                                              // x
                    rawHitCoords[1] = 0.;                                                              // y
                    rawHitCoords[2] = 0.;                                                              // z
                    rawHitCoords[3] = Mustard::ToG3<"Length">(fiberMap[*Get<"FiberID">(*hit)].radius); // r
                    rawHitCoords[4] = fiberMap[*Get<"FiberID">(*hit)].pitch;                           // pitch
                    rawHitCoords[5] = fiberMap[*Get<"FiberID">(*hit)].rotationAngle;                   // rotationAngle
                    rawHitCoords[6] = 2 * std::numbers::pi;
                    rawHitCoords[7] = 0; //

                    TMatrixDSym rawHitCov{8};
                    rawHitCov.UnitMatrix();
                    rawHitCov(0, 0) = 0;
                    rawHitCov(1, 1) = 0;
                    rawHitCov(2, 2) = 0;
                    rawHitCov(3, 3) = 0.0;
                    rawHitCov(4, 4) = 0.0;
                    rawHitCov(5, 5) = 0.0;
                    rawHitCov(6, 6) = 0.0;
                    rawHitCov(7, 7) = 0.0289 * 0.0289;
                    return new MACE::PhaseI::Reconstruction::GenFitX::HelixMeasurement{rawHitCoords, rawHitCov, *Get<"FiberID">(*hit), Get<"HitID">(*hit), nullptr};
                }
            }()};
        genfitTrack->insertPoint(new genfit::TrackPoint{measurement, genfitTrack.get()});
        measurementHitMap.emplace(measurement, hit);
    }
    genfitTrack->sort();
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
    const auto& status{*genfitTrack->getFitStatus()};
    const auto* firstState{static_cast<const genfit::MeasuredStateOnPlane*>(nullptr)};
    try {
        firstState = &genfitTrack->getFittedState();
    } catch (const genfit::Exception&) {
        return {};
    }
    try {
        firstState = &genfitTrack->getFittedState();
    } catch (const genfit::Exception&) {
        return {};
    }
    const auto cardinalRep{genfitTrack->getCardinalRep()};
    const auto& allPoint{genfitTrack->getPointsWithMeasurement()};
    std::vector<AHitPointer> fitted{};
    fitted.reserve(allPoint.size());
    std::vector<AHitPointer> failed{};
    failed.reserve(allPoint.size());
    for (gsl::index i{}; i < ssize(allPoint); ++i) {
        const auto fit{allPoint[i]->hasFitterInfo(cardinalRep)};
        (fit ? fitted : failed)
            .emplace_back(measurementHitMap.at(allPoint[i]->getRawMeasurement()));
    }
    const auto t0{Get<"t">(*seed)};
    const auto x0{Mustard::ToG4<"Length">(firstState->getPos())};
    const auto p0{Mustard::ToG4<"Energy">(firstState->getMom()).Unit()};

    auto track{std::make_shared_for_overwrite<Mustard::Data::Tuple<ATrack>>()};
    Get<"EvtID">(*track) = Get<"EvtID">(*seed);
    Get<"x">(*track) = this->template FromTVector3<muc::array3d>(x0);
    auto p{this->template FromTVector3<muc::array3d>(p0)};
    const auto& seedX{Get<"x">(*seed)};
    if (p[0] * seedX[0] + p[1] * seedX[1] + p[2] * seedX[2] < 0) {
        p[0] *= -1;
        p[1] *= -1;
        p[2] *= -1;
    }
    Get<"p">(*track) = p;
    Get<"t">(*track) = t0;
    Get<"chi2">(*track) = status.getChi2() / status.getNdf();
    if (fEnableEventDisplay) {
        genfit::EventDisplay::getInstance()->addEvent(genfitTrack.get());
        fEventDisplayTrackStore.emplace_back(std::move(genfitTrack)); // genfitTrack is MOVED here
    }
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
    TVector3 dest{};
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
    T dest{};
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    return dest;
}

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter
