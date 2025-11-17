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
