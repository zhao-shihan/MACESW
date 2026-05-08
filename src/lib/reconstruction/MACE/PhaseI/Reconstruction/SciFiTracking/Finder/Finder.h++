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

#pragma once

#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"

#include "muc/ptrvec"

#include <concepts>
#include <iterator>

namespace MACE::PhaseI::inline Reconstuction::SciFiTracking::inline Finder {

template<typename T>
concept Finder =
    requires {
        typename T::SciFiHit;
        typename T::Track;
        requires Mustard::Data::SuperTupleModel<typename T::SciFiHit, MACE::PhaseI::Data::SciFiHit>;
        requires Mustard::Data::SuperTupleModel<typename T::Track, MACE::PhaseI::Data::Track>;
    } and
    requires(T finder, const int nextTrackID, const int trackID, const std::vector<Mustard::Data::Tuple<typename T::SciFiHit>*> hitData) {
        { finder(hitData, nextTrackID) };
        { finder(hitData, nextTrackID).good[trackID].hitData } -> std::same_as<std::vector<Mustard::Data::Tuple<typename T::SciFiHit>*>>;
        { finder(hitData, nextTrackID).good[trackID].seed } -> std::same_as<std::shared_ptr<Mustard::Data::Tuple<typename T::Track>>>;
        { finder(hitData, nextTrackID).garbage } -> std::same_as<std::vector<Mustard::Data::Tuple<typename T::SciFiHit>*>>;
    } and
    requires(T finder, const int nextTrackID, const int trackID, const muc::shared_ptrvec<Mustard::Data::Tuple<typename T::SciFiHit>> hitData) {
        { finder(hitData, nextTrackID) };
        { finder(hitData, nextTrackID).good[trackID].hitData } -> std::same_as<muc::shared_ptrvec<Mustard::Data::Tuple<typename T::SciFiHit>>>;
        { finder(hitData, nextTrackID).good[trackID].seed } -> std::same_as<std::shared_ptr<Mustard::Data::Tuple<typename T::Track>>>;
        { finder(hitData, nextTrackID).garbage } -> std::same_as<muc::shared_ptrvec<Mustard::Data::Tuple<typename T::SciFiHit>>>;
    };

template<typename T>
concept SimFinder =
    requires {
        requires Finder<T>;
        requires Mustard::Data::SuperTupleModel<typename T::SciFiHit, MACE::PhaseI::Data::SciFiSimHit>;
        requires Mustard::Data::SuperTupleModel<typename T::Track, MACE::PhaseI::Data::Track>;
    };
} // namespace MACE::PhaseI::inline Reconstuction::SciFiTracking::inline Finder
