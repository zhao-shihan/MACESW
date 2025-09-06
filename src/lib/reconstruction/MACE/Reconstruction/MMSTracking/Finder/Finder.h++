#pragma once

#include "MACE/Data/Hit.h++"
#include "MACE/Data/MMSTrack.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"

#include "muc/ptrvec"

#include <concepts>
#include <iterator>

namespace MACE::inline Reconstruction::MMSTracking::inline Finder {

template<typename T>
concept Finder =
    requires {
        typename T::Hit;
        typename T::Track;
        requires Mustard::Data::SuperTupleModel<typename T::Hit, Data::CDCHit>;
        requires Mustard::Data::SuperTupleModel<typename T::Track, Data::MMSTrack>;
    } and
    requires(T finder, const int nextTrackID, const int trackID, const std::vector<Mustard::Data::Tuple<typename T::Hit>*> hitData) {
        { finder(hitData, nextTrackID) };
        { finder(hitData, nextTrackID).good[trackID].hitData } -> std::same_as<std::vector<Mustard::Data::Tuple<typename T::Hit>*>>;
        { finder(hitData, nextTrackID).good[trackID].seed } -> std::same_as<std::shared_ptr<Mustard::Data::Tuple<typename T::Track>>>;
        { finder(hitData, nextTrackID).garbage } -> std::same_as<std::vector<Mustard::Data::Tuple<typename T::Hit>*>>;
    } and
    requires(T finder, const int nextTrackID, const int trackID, const muc::shared_ptrvec<Mustard::Data::Tuple<typename T::Hit>> hitData) {
        { finder(hitData, nextTrackID) };
        { finder(hitData, nextTrackID).good[trackID].hitData } -> std::same_as<muc::shared_ptrvec<Mustard::Data::Tuple<typename T::Hit>>>;
        { finder(hitData, nextTrackID).good[trackID].seed } -> std::same_as<std::shared_ptr<Mustard::Data::Tuple<typename T::Track>>>;
        { finder(hitData, nextTrackID).garbage } -> std::same_as<muc::shared_ptrvec<Mustard::Data::Tuple<typename T::Hit>>>;
    };

template<typename T>
concept SimFinder =
    requires {
        requires Finder<T>;
        requires Mustard::Data::SuperTupleModel<typename T::Hit, Data::CDCSimHit>;
        requires Mustard::Data::SuperTupleModel<typename T::Track, Data::MMSSimTrack>;
    };

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder
