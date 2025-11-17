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
GenFitDAFFinder<AHit, ATrack>::GenFitDAFFinder(double driftErrorRMS) :
    Base{},
    fFirstSegmentMaxDeltaPhi{},
    fFirstSegmentMinNHit{},
    fMomentumSeed{0.7 * (Mustard::PhysicalConstant::muon_mass_c2 / 2)}, // mean Michel momentum
    fDAFFitter{driftErrorRMS} {
    const auto& cdc{Detector::Description::CDC::Instance()};
    fFirstSegmentMaxDeltaPhi = 1.25 * cdc.LayerConfiguration().front().cellAzimuthWidth;
    fFirstSegmentMinNHit = cdc.NSenseLayerPerSuper();
}

template<Mustard::Data::SuperTupleModel<Data::CDCHit> AHit,
         Mustard::Data::SuperTupleModel<Data::MMSTrack> ATrack>
template<std::indirectly_readable AHitPointer>
    requires Mustard::Data::SuperTupleModel<typename std::iter_value_t<AHitPointer>::Model, AHit>
auto GenFitDAFFinder<AHit, ATrack>::operator()(const std::vector<AHitPointer>& hitData, int nextTrackID) -> Base::template Result<AHitPointer> {
    using Result = Base::template Result<AHitPointer>;

    if (not this->GoodHitData(hitData) or
        ssize(hitData) < this->MinNHit()) {
        return {.garbage = hitData};
    }

    const auto nTrackForReserve{hitData.size() / this->MinNHit()};

    const auto& cdc{Detector::Description::CDC::Instance()};
    const auto& cell{cdc.CellMap()};

    // all hit in first super layer, sort by phi
    std::vector<AHitPointer> hitInFirstSuperLayer;
    hitInFirstSuperLayer.reserve(cdc.NSenseLayerPerSuper() * nTrackForReserve);
    for (auto&& hit : hitData) {
        if (cell[Get<"CellID">(*hit)].superLayerID == 0) {
            hitInFirstSuperLayer.emplace_back(hit);
        }
    }
    if (hitInFirstSuperLayer.size() < fFirstSegmentMinNHit) {
        return {.garbage = hitData};
    }
    muc::timsort(hitInFirstSuperLayer,
                 [&](auto&& hit1, auto&& hit2) { // sort by phi
                     return cell[Get<"CellID">(*hit1)].centerAzimuth < cell[Get<"CellID">(*hit2)].centerAzimuth;
                 });

    // hit segment in first super layer
    std::vector<std::ranges::subrange<typename std::vector<AHitPointer>::const_iterator>> segmentInFirstSuperLayer;
    segmentInFirstSuperLayer.reserve(nTrackForReserve);
    auto segmentBegin{hitInFirstSuperLayer.cbegin()};
    for (auto hit{std::next(hitInFirstSuperLayer.cbegin())};; ++hit) {
        const auto deltaPhi{cell[Get<"CellID">(**hit)].centerAzimuth -
                            cell[Get<"CellID">(**std::prev(hit))].centerAzimuth};
        const auto AddSegment{[&] { segmentInFirstSuperLayer.emplace_back(segmentBegin, hit); }};
        if (deltaPhi > fFirstSegmentMaxDeltaPhi) {
            if (std::distance(segmentBegin, hit) >= fFirstSegmentMinNHit) {
                AddSegment();
            }
            segmentBegin = hit;
        }
        if (hit == std::prev(hitInFirstSuperLayer.cend())) {
            AddSegment();
            break;
        }
    }

    Result r;
    r.good.reserve(nTrackForReserve);
    r.garbage.reserve(hitData.size());

    muc::flat_hash_set<AHitPointer> goodHit;
    goodHit.reserve(hitData.size());
    for (auto&& segment : segmentInFirstSuperLayer) {
        const auto seed{std::make_shared_for_overwrite<Mustard::Data::Tuple<ATrack>>()};
        Get<"EvtID">(*seed) = Get<"EvtID">(*hitData.front());
        Get<"TrkID">(*seed) = nextTrackID;
        Get<"x0">(*seed) = {};
        Get<"p0">(*seed) = fMomentumSeed *
                           std::transform_reduce(
                               segment.begin(), segment.end(), Eigen::Vector2d{}, std::plus<>{},
                               [&](auto&& hit) -> const Eigen::Vector2d& {
                                   return cell[Get<"CellID">(*hit)].position;
                               })
                               .normalized();
        // try electron and positron
        Get<"PDGID">(*seed) = -11;
        const auto [emTrack, emFitted, emFailed]{fDAFFitter(hitData, seed)};
        Get<"PDGID">(*seed) = 11;
        const auto [epTrack, epFitted, epFailed]{fDAFFitter(hitData, seed)};
        if ((emTrack == nullptr or Get<"HitID">(*emTrack)->size() < this->MinNHit()) and
            (epTrack == nullptr or Get<"HitID">(*epTrack)->size() < this->MinNHit())) {
            continue;
        }
        // select particle
        int pdgID;
        if ((emTrack == nullptr or Get<"HitID">(*emTrack)->size() < this->MinNHit()) and
            epTrack != nullptr) {
            pdgID = -11;
        } else if (emTrack != nullptr and
                   (epTrack == nullptr or Get<"HitID">(*epTrack)->size() < this->MinNHit())) {
            pdgID = 11;
        } else if (Get<"chi2">(*emTrack) < Get<"chi2">(*epTrack)) {
            pdgID = 11;
        } else {
            pdgID = -11;
        }
        const auto& track{pdgID == 11 ? emTrack : epTrack};
        const auto& fitted{pdgID == 11 ? emFitted : epFitted}; // clang-format off
        r.good.insert({nextTrackID, {fitted, track}}); // clang-format on
        for (auto&& hit : fitted) {
            goodHit.emplace(hit);
        }
        ++nextTrackID;
    }

    // collect garbage hit
    for (auto&& hit : hitData) {
        if (not goodHit.contains(hit)) {
            r.garbage.emplace_back(hit);
        }
    }

    return r;
}

} // namespace MACE::inline Reconstruction::MMSTracking::inline Finder
