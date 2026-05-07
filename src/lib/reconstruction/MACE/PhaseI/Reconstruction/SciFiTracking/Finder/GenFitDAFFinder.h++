#pragma once

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/FinderBase.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitDAFFitter.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Math/GeometryRepresentation.h++"
#include "Mustard/Math/POCA.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Minuit2/Minuit2Minimizer.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "Eigen/Core"

#include "muc/algorithm"
#include "muc/array"
#include "muc/hash_set"

#include <Eigen/Dense>
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <ranges>
#include <tuple>

namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::SciFiHit> ASciFiHit,
         Mustard::Data::SuperTupleModel<MACE::PhaseI::Data::Track> ATrack>
class GenFitDAFFinder : public FinderBase<ASciFiHit, ATrack> {
private:
    using Base = FinderBase<ASciFiHit, ATrack>;

public:
    GenFitDAFFinder();
    virtual ~GenFitDAFFinder() override = default;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto operator()(std::vector<AHitPointer>& hitData, int nextTrackID) -> Base::template Result<AHitPointer>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto ClusterHits(std::vector<AHitPointer>& hitData) -> const std::vector<std::vector<AHitPointer>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto DivideHits(const std::vector<std::vector<AHitPointer>>& hitData)
        -> const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto FindCompatibleClusterCombinations(const std::tuple<std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>, std::vector<std::vector<AHitPointer>>>& hitData)
        -> const std::set<std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto CalCoordinates(const std::set<std::vector<std::vector<AHitPointer>>>& hitData, const muc::array3d& direction)
        -> const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto DividePoints(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
        -> const std::vector<std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto EstimateInitialDirection(const std::map<muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
        -> std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>>;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto AddStraightFibersByTheta(
        const std::vector<std::vector<AHitPointer>>& currentClusters,
        const std::vector<std::vector<AHitPointer>>& axialClusters,
        double angleThreshold,
        double timeThreshold) const -> std::vector<std::vector<AHitPointer>>;

    auto FindHLMinDistanceSquare(
        double HelixR, double HelixB, double rotationAngle,
        const muc::array3d line_p0, const muc::array3d line_dir,
        double initialT, double initialTheta) -> std::tuple<double, double, double>;

    auto FindLLMinDistanceSquare(
        const muc::array3d line1_point, const muc::array3d line1_dir,
        const muc::array3d line2_point, const muc::array3d line2_dir) -> double;

    template<std::indirectly_readable AHitPointer>
        requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, ASciFiHit>
    auto TrackFit(std::tuple<muc::array3d, muc::array3d, std::vector<std::vector<AHitPointer>>>& hitData)
        -> std::shared_ptr<Mustard::Data::Tuple<ATrack>>;

private:
    double fFirstSegmentMaxDeltaPhi;
    double fFirstSegmentMinNHit;
    double fMomentumSeed;
};

} // namespace MACE::PhaseI::inline Reconstruction::SciFiTracking::inline Finder

#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/GenFitDAFFinder.inl"
