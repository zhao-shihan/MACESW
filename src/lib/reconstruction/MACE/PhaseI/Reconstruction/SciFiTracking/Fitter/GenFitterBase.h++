#pragma once

#include "MACE/Detector/Definition/World.h++"
#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Detector/Assembly/TrackerSystem.h++"
#include "MACE/PhaseI/Detector/Definition/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Definition/World.h++"
#include "MACE/PhaseI/Reconstruction/GenFitExtension/HelixMeasurement.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/FitterBase.h++"

#include "Mustard/Concept/NumericVector.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/IO/CreateTemporaryFile.h++"
#include "Mustard/Utility/ConvertG3G4Unit.h++"
#include "Mustard/Utility/FunctionAttribute.h++"

#include "AbsFitter.h"
#include "AbsMeasurement.h"
#include "EventDisplay.h"
#include "FieldManager.h"
#include "FitStatus.h"
#include "MaterialEffects.h"
#include "MeasuredStateOnPlane.h"
#include "RKTrackRep.h"
#include "StateOnPlane.h"
#include "TGeoMaterialInterface.h"
#include "Track.h"
#include "TrackPoint.h"
#include "WireMeasurement.h"
#include "WireMeasurementNew.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TDatabasePDG.h"
#include "TGeoManager.h"
#include "TMatrixDSymfwd.h"
#include "TMatrixTSym.h"
#include "TVector3.h"
#include "TVectorD.h"

#include "mplr/mplr.hpp"

#include "muc/hash_map"
#include "muc/math"
#include "muc/numeric"
#include "muc/ptrvec"

#include "gsl/gsl"

#include <ConstField.h>
#include <FieldManager.h>
#include <cstddef>
#include <iterator>
#include <string_view>
#include <utility>

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> AHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack,
         std::derived_from<genfit::AbsFitter> AFitter>
class GenFitterBase : public FitterBase<AHit, ATrack> {

private:
    using Base = FitterBase<AHit, ATrack>;

protected:
    GenFitterBase(double fiberRMS, double lowestMomentum = 1 * CLHEP::MeV);
    virtual ~GenFitterBase() = default;

public:
    auto FiberRMS() const -> auto { return fFiberRMS; }
    auto LowestMomentum() const -> auto { return fLowestMomentum; }
    auto EnableEventDisplay() const -> auto { return fEnableEventDisplay; }

    auto FiberRMS(double val) -> void { fFiberRMS = val; }
    auto LowestMomentum(double val) -> void { fLowestMomentum = val; }
    auto EnableEventDisplay(bool val) -> void { fEnableEventDisplay = val; }

    auto GenFitter() const -> const auto& { return fGenFitter; }
    auto GenFitter() -> auto& { return fGenFitter; }

    auto OpenEventDisplay(bool clearUponClose = true) -> void;
    auto ClearEventDisplayTrackStore() -> void { fEventDisplayTrackStore.clear(); }

protected:
    template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
        requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
                 Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
    auto Initialize(const std::vector<AHitPointer>& hitData, ASeedPointer seed)
        -> std::pair<std::shared_ptr<genfit::Track>,
                     muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer>>;
    template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
        requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
                 Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
    auto Finalize(std::shared_ptr<genfit::Track> genfitTrack, ASeedPointer seed,
                  const muc::flat_hash_map<const genfit::AbsMeasurement*, AHitPointer>& measurementHitMap)
        -> Base::template Result<AHitPointer>;

    template<Mustard::Concept::NumericVector3FloatingPoint T>
    MUSTARD_ALWAYS_INLINE static auto ToTVector3(T src) -> TVector3;
    template<Mustard::Concept::NumericVector3FloatingPoint T>
    MUSTARD_ALWAYS_INLINE static auto FromTVector3(const TVector3& src) -> T;

private:
    double fFiberRMS;
    double fLowestMomentum;
    bool fEnableEventDisplay;

    muc::shared_ptrvec<genfit::Track> fEventDisplayTrackStore;

    AFitter fGenFitter;
};

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Fitter
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitterBase.inl"
