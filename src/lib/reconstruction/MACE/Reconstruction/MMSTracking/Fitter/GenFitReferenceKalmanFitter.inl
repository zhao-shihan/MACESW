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

namespace MACE::inline Reconstruction::MMSTracking::inline Fitter {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
GenFitReferenceKalmanFitter<AHit, ATrack>::GenFitReferenceKalmanFitter(double driftErrorRMS) :
    Base{driftErrorRMS} {}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
template<std::indirectly_readable AHitPointer, std::indirectly_readable ASeedPointer>
    requires(Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit> and
             Mustard::Data::SuperTupleModel<typename std::iter_value_t<ASeedPointer>::Model, ATrack>)
auto GenFitReferenceKalmanFitter<AHit, ATrack>::operator()(const std::vector<AHitPointer>& hitData, ASeedPointer seed) -> Base::template Result<AHitPointer> {
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
