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

namespace MACE::inline Reconstruction::MMSTracking::inline Finder {

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
FinderBase<AHit, ATrack>::FinderBase() :
    fMinNHit{} {
    const auto& cdc{Detector::Description::CDC::Instance()};
    fMinNHit = cdc.NSenseLayerPerSuper() * cdc.NSuperLayer();
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
FinderBase<AHit, ATrack>::~FinderBase() = default;

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
auto FinderBase<AHit, ATrack>::GoodHitData(const std::vector<AHitPointer>& hitData) -> bool {
    if (hitData.empty()) [[unlikely]] {
        Mustard::PrintWarning("Empty hit data");
        return false;
    }

    auto good{true};

    muc::flat_hash_set<int> eventID;
    eventID.reserve(hitData.size());
    for (auto&& hit : hitData) {
        eventID.emplace(Get<"EvtID">(*hit));
    }
    if (eventID.size() != 1) [[unlikely]] {
        Mustard::PrintWarning(fmt::format("Hit data include different event IDs {}, skipping", eventID));
        good = false;
    }

    return good;
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder
