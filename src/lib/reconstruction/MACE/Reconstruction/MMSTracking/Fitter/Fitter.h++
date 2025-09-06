#pragma once

#include "MACE/Data/Hit.h++"
#include "MACE/Data/MMSTrack.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"

#include "muc/ptrvec"

#include <concepts>
#include <iterator>
#include <utility>

namespace MACE::inline Reconstruction::MMSTracking::inline Fitter {

template<typename T>
concept Fitter =
    requires {
        typename T::Hit;
        typename T::Track;
        requires Mustard::Data::SuperTupleModel<typename T::Hit, Data::CDCHit>;
        requires Mustard::Data::SuperTupleModel<typename T::Track, Data::MMSTrack>;
    } and
    requires(T fitter, const std::vector<Mustard::Data::Tuple<typename T::Hit>*> hitData, const Mustard::Data::Tuple<typename T::Track>* seed) {
        { fitter(hitData, seed) };
        { fitter(hitData, seed).track } -> std::same_as<std::shared_ptr<Mustard::Data::Tuple<typename T::Track>>>;
        { fitter(hitData, seed).fitted } -> std::same_as<std::vector<Mustard::Data::Tuple<typename T::Hit>*>>;
        { fitter(hitData, seed).failed } -> std::same_as<std::vector<Mustard::Data::Tuple<typename T::Hit>*>>;
    } and
    requires(T fitter, const muc::shared_ptrvec<Mustard::Data::Tuple<typename T::Hit>> hitData, const std::shared_ptr<Mustard::Data::Tuple<typename T::Track>> seed) {
        { fitter(hitData, seed) };
        { fitter(hitData, seed).track } -> std::same_as<std::shared_ptr<Mustard::Data::Tuple<typename T::Track>>>;
        { fitter(hitData, seed).fitted } -> std::same_as<muc::shared_ptrvec<Mustard::Data::Tuple<typename T::Hit>>>;
        { fitter(hitData, seed).failed } -> std::same_as<muc::shared_ptrvec<Mustard::Data::Tuple<typename T::Hit>>>;
    };

template<typename T>
concept SimFitter =
    requires {
        requires Fitter<T>;
        requires Mustard::Data::SuperTupleModel<typename T::Hit, Data::CDCSimHit>;
        requires Mustard::Data::SuperTupleModel<typename T::Track, Data::MMSSimTrack>;
    };

} // namespace MACE::inline Reconstruction::MMSTracking::inline Fitter
