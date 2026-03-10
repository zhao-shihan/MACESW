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

#include "MACE/Detector/Description/CDC.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "Eigen/Geometry"

#include "muc/math"
#include "muc/numeric"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <tuple>
#include <utility>

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit;
using namespace Mustard::PhysicalConstant;

CDC::CDC() :
    DescriptionWithCacheBase{"CDC"},
    // Geometry
    fEvenSuperLayerIsAxial{this},
    fNSuperLayer{this, 7},
    fNSenseLayerPerSuper{this, 3},
    fGasInnerRadius{this, 15_cm},
    fGasInnerLength{this, 120_cm},
    fEndCapSlope{this, 0.75},
    fMinStereoAngle{this, 6_deg},
    fCellWidthLowerBound{this, 8_mm},
    fReferenceCellWidth{this, 10_mm},
    fCellWidthUpperBound{this, 12_mm},
    fFieldWireDiameter{this, 80_um},
    fSenseWireDiameter{this, 20_um},
    fMinAdjacentSuperLayersDistance{this, 1_mm},
    fMinWireAndRadialShellDistance{this, 2_mm},
    fEndCapThickness{this, 15_mm},
    fInnerShellAlThickness{this, 25_um},
    fInnerShellMylarThickness{this, 25_um},
    fOuterShellThickness{this, 10_mm},
    fLayerConfiguration{this, [this] { return CalculateLayerConfiguration(); }},
    fCellMap{this, [this] { return CalculateCellMap(); }},
    fCellMapFromSenseLayerIDAndLocalCellID{this, [this] { return CalculateCellMapFromSenseLayerIDAndLocalCellID(); }},
    // Material
    fGasButaneFraction{this, 0.15},
    fEndCapMaterialName{this, "G4_Al"},
    fOuterShellCFRPDensity{this, 1.95_g_cm3},
    // Detection
    fMeanDriftVelocity{this, 3.5_cm_us},
    fTimeResolutionFWHM{this, 30_ns} {}

auto CDC::GasMaterial() const -> G4Material* {
    constexpr auto materialName{"CDCGas"};
    const auto nist{G4NistManager::Instance()};

    auto gas{nist->FindMaterial(materialName)};
    if (gas) {
        return gas;
    }

    const auto heFraction{1 - fGasButaneFraction};
    const auto he{nist->FindOrBuildMaterial("G4_He")};
    const auto butane{nist->FindOrBuildMaterial("G4_BUTANE")};

    gas = new G4Material{materialName,
                         heFraction * he->GetDensity() + fGasButaneFraction * butane->GetDensity(),
                         2,
                         kStateGas};
    gas->AddMaterial(he, heFraction);
    gas->AddMaterial(butane, fGasButaneFraction);

    return gas;
}

auto CDC::CalculateLayerConfiguration() const -> std::vector<SuperLayerConfiguration> {
    std::vector<SuperLayerConfiguration> layerConfig;

    layerConfig.reserve(fNSuperLayer);
    for (int superLayerID{}; superLayerID < fNSuperLayer; ++superLayerID) {
        const auto notFirstSuperLayer{superLayerID > 0};
        auto& super{layerConfig.emplace_back()};
        const auto& lastSuper{notFirstSuperLayer ?
                                  layerConfig[superLayerID - 1] :
                                  layerConfig.front()};

        super.isAxial = fEvenSuperLayerIsAxial ?
                            muc::even(superLayerID) :
                            muc::odd(superLayerID);
        super.superLayerID = superLayerID;
        super.innerRadius = superLayerID > 0 ?
                                lastSuper.outerRadius + fMinAdjacentSuperLayersDistance :
                                fGasInnerRadius + fMinWireAndRadialShellDistance;
        super.nCellPerSenseLayer =
            4 * std::lround(0.5_pi / [this, &super] {
                return std::clamp(
                    fReferenceCellWidth /
                        (fGasInnerRadius + fMinWireAndRadialShellDistance +
                         0.5 * (fNSuperLayer * fNSenseLayerPerSuper * fReferenceCellWidth +
                                (fNSuperLayer - 1) * fMinAdjacentSuperLayersDistance)),
                    fCellWidthLowerBound /
                        (super.innerRadius + fReferenceCellWidth / 2),
                    fCellWidthUpperBound /
                        (super.innerRadius + fNSenseLayerPerSuper * fReferenceCellWidth - fReferenceCellWidth / 2));
            }());
        super.cellAzimuthWidth = 2_pi / super.nCellPerSenseLayer;
        const auto halfPhiCell{super.cellAzimuthWidth / 2};

        super.innerHalfLength =
            notFirstSuperLayer ?
                lastSuper.outerHalfLength + fEndCapSlope * fMinAdjacentSuperLayersDistance :
                fGasInnerLength / 2 + fEndCapSlope * fMinWireAndRadialShellDistance;

        const auto firstInnerStereoZenithAngle{
            [this,
             isAxial = super.isAxial,
             superLayerID] {
                if (isAxial) {
                    return 0.0;
                }
                if ((fEvenSuperLayerIsAxial ? superLayerID + 3 : superLayerID) % 4 == 0) {
                    return +fMinStereoAngle;
                }
                return -fMinStereoAngle;
            }()};
        const auto firstSenseLayerID{static_cast<int>(superLayerID * fNSenseLayerPerSuper)};
        super.sense.reserve(fNSenseLayerPerSuper);
        for (int senseLayerLocalID{}; senseLayerLocalID < fNSenseLayerPerSuper; ++senseLayerLocalID) {
            const auto notFirstSenseLayerOfThisSuperLayer{senseLayerLocalID > 0};
            auto& sense{super.sense.emplace_back()};
            const auto& lastSense{notFirstSenseLayerOfThisSuperLayer ?
                                      super.sense[senseLayerLocalID - 1] :
                                      super.sense.front()};

            sense.senseLayerID = firstSenseLayerID + senseLayerLocalID;
            sense.innerRadius =
                notFirstSenseLayerOfThisSuperLayer ?
                    lastSense.outerRadius :
                    super.innerRadius;
            sense.outerRadius =
                [&rIn = sense.innerRadius,
                 &dFW = fFieldWireDiameter,
                 &halfPhiCell] {
                    return (rIn + (rIn + dFW) * halfPhiCell) / (1 - halfPhiCell);
                }();
            sense.cellWidth = sense.outerRadius - sense.innerRadius;
            const auto tanInnerStereoZenithAngle{notFirstSenseLayerOfThisSuperLayer ?
                                                     lastSense.TanStereoZenithAngle(lastSense.outerRadius) :
                                                     std::tan(firstInnerStereoZenithAngle)};
            sense.halfLength =
                [&eta = fEndCapSlope,
                 lastHL = notFirstSenseLayerOfThisSuperLayer ?
                              lastSense.halfLength :
                              super.innerHalfLength,
                 rIn = sense.innerRadius,
                 lastRIn = notFirstSenseLayerOfThisSuperLayer ?
                               lastSense.innerRadius / std::cos(lastSense.stereoAzimuthAngle / 2) :
                               super.innerRadius,
                 tan2ThetaS = muc::pow(tanInnerStereoZenithAngle, 2)] {
                    return (lastHL +
                            eta * (std::sqrt(
                                       muc::pow(rIn, 2) +
                                       (lastHL + eta * (rIn - lastRIn)) * (lastHL - eta * (rIn + lastRIn)) * tan2ThetaS) -
                                   lastRIn)) /
                           (1 - muc::pow(*eta, 2) * tan2ThetaS);
                }();
            sense.stereoAzimuthAngle = 2 * std::atan(sense.halfLength / sense.innerRadius * tanInnerStereoZenithAngle);

            const auto firstCellID{((notFirstSuperLayer ?
                                         lastSuper.sense.back().cell.back().cellID + 1 :
                                         0) +
                                    senseLayerLocalID * super.nCellPerSenseLayer)};
            const auto firstCellAzimuth{muc::even(sense.senseLayerID) ?
                                            0 :
                                            halfPhiCell};
            sense.cell.reserve(super.nCellPerSenseLayer);
            for (int cellLocalID{}; cellLocalID < super.nCellPerSenseLayer; ++cellLocalID) {
                auto& cell{sense.cell.emplace_back()};
                cell.cellID = firstCellID + cellLocalID;
                cell.centerAzimuth = firstCellAzimuth + cellLocalID * super.cellAzimuthWidth;
            }
            sense.cell.shrink_to_fit();
        }
        super.sense.shrink_to_fit();

        super.outerRadius =
            [&dFW = fFieldWireDiameter,
             &lastSense = std::as_const(super).sense.back()] {
                return (lastSense.outerRadius + dFW) / std::cos(lastSense.stereoAzimuthAngle / 2);
            }();
        super.outerHalfLength = super.innerHalfLength + fEndCapSlope * (super.outerRadius - super.innerRadius);
    }
    layerConfig.shrink_to_fit();

    return layerConfig;
}

auto CDC::CalculateCellMap() const -> std::vector<CellInformation> {
    std::vector<CellInformation> cellMap;

    const auto rFieldWire{fFieldWireDiameter / 2};

    const auto& layerConfig{LayerConfiguration()};
    cellMap.reserve(muc::ranges::transform_reduce(layerConfig, 0ULL, std::plus{},
                                                  [this](const auto& super) {
                                                      return super.nCellPerSenseLayer * fNSenseLayerPerSuper;
                                                  }));

    for (int superLayerID{};
         auto&& super : layerConfig) {
        for (int senseLayerLocalID{};
             auto&& sense : super.sense) {
            const auto wireRadialPosition{muc::midpoint(sense.innerRadius, sense.outerRadius) + rFieldWire}; // clang-format off
            const Eigen::AngleAxisd stereoRotation{-sense.StereoZenithAngle(wireRadialPosition), Eigen::Vector3d{1, 0, 0}};
            const auto senseWireHalfLength{sense.halfLength * sense.SecStereoZenithAngle(sense.innerRadius + fFieldWireDiameter / 2 + sense.cellWidth / 2)};
            for (int cellLocalID{};
                 auto&& cell : sense.cell) {
                cellMap.push_back({cell.cellID,
                                   cellLocalID,
                                   sense.senseLayerID,
                                   senseLayerLocalID,
                                   superLayerID,
                                   Eigen::Rotation2Dd{cell.centerAzimuth} *
                                       Eigen::Vector2d{wireRadialPosition, 0},
                                   Eigen::AngleAxisd{cell.centerAzimuth, Eigen::Vector3d{0, 0, 1}} *
                                       (stereoRotation * Eigen::Vector3d{0, 0, 1}),
                                   senseWireHalfLength,
                                   cell.centerAzimuth}); // clang-format on
                /* const auto& x0{cellMap.back().position};
                const auto& t0{cellMap.back().direction};
                const auto oldCoutPrecision{std::cout.precision(std::numeric_limits<double>::max_digits10)};
                std::cout << cell.cellID << ',' << x0.x() << ',' << x0.y() << ',' << t0.x() << ',' << t0.y() << ',' << t0.z() << ',' << 2 * senseWireHalfLength << ',' << '\n';
                std::cout.precision(oldCoutPrecision); */
                ++cellLocalID;
            }
            /* fmt::println("{},{},{},{},{},{},{},{},{}",
                         superLayerID, sense.senseLayerID, sense.cell.front().cellID, sense.cell.back().cellID, muc::midpoint(sense.innerRadius, sense.outerRadius) + fFieldWireDiameter / 2, sense.stereoAzimuthAngle, sense.cell.front().centerAzimuth, sense.cellWidth, 2 * sense.halfLength); */
            ++senseLayerLocalID;
        }
        ++superLayerID;
    }
    cellMap.shrink_to_fit();

    return cellMap;
}

auto CDC::CalculateCellMapFromSenseLayerIDAndLocalCellID() const -> CellMapFromSenseLayerIDAndLocalCellIDType {
    CellMapFromSenseLayerIDAndLocalCellIDType cellMapFromSenseLayerIDAndLocalCellID;
    const auto& cellMap{CellMap()};
    cellMapFromSenseLayerIDAndLocalCellID.reserve(cellMap.size());
    for (auto&& cellInfo : cellMap) {
        cellMapFromSenseLayerIDAndLocalCellID[{cellInfo.senseLayerID, cellInfo.cellLocalID}] = cellInfo;
    }
    return cellMapFromSenseLayerIDAndLocalCellID;
}

auto CDC::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fEvenSuperLayerIsAxial, "EvenSuperLayerIsAxial");
    ImportValue(node, fNSuperLayer, "NSuperLayer");
    ImportValue(node, fNSenseLayerPerSuper, "NSenseLayerPerSuper");
    ImportValue(node, fGasInnerRadius, "GasInnerRadius");
    ImportValue(node, fGasInnerLength, "GasInnerLength");
    ImportValue(node, fEndCapSlope, "EndCapSlope");
    ImportValue(node, fMinStereoAngle, "MinStereoAngle");
    ImportValue(node, fCellWidthLowerBound, "CellWidthLowerBound");
    ImportValue(node, fReferenceCellWidth, "ReferenceCellWidth");
    ImportValue(node, fCellWidthUpperBound, "CellWidthUpperBound");
    ImportValue(node, fFieldWireDiameter, "FieldWireDiameter");
    ImportValue(node, fSenseWireDiameter, "SenseWireDiameter");
    ImportValue(node, fMinAdjacentSuperLayersDistance, "MinAdjacentSuperLayersDistance");
    ImportValue(node, fMinWireAndRadialShellDistance, "MinWireAndRadialShellDistance");
    ImportValue(node, fEndCapThickness, "EndCapThickness");
    ImportValue(node, fInnerShellAlThickness, "InnerShellAlThickness");
    ImportValue(node, fInnerShellMylarThickness, "InnerShellMylarThickness");
    ImportValue(node, fOuterShellThickness, "OuterShellThickness");
    // Material
    ImportValue(node, fGasButaneFraction, "GasButaneFraction");
    ImportValue(node, fEndCapMaterialName, "EndCapMaterialName");
    ImportValue(node, fOuterShellCFRPDensity, "OuterShellCFRPDensity");
    // Detection
    ImportValue(node, fMeanDriftVelocity, "MeanDriftVelocity");
    ImportValue(node, fTimeResolutionFWHM, "TimeResolutionFWHM");
}

auto CDC::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fEvenSuperLayerIsAxial, "EvenSuperLayerIsAxial");
    ExportValue(node, fNSuperLayer, "NSuperLayer");
    ExportValue(node, fNSenseLayerPerSuper, "NSenseLayerPerSuper");
    ExportValue(node, fGasInnerRadius, "GasInnerRadius");
    ExportValue(node, fGasInnerLength, "GasInnerLength");
    ExportValue(node, fEndCapSlope, "EndCapSlope");
    ExportValue(node, fMinStereoAngle, "MinStereoAngle");
    ExportValue(node, fCellWidthLowerBound, "CellWidthLowerBound");
    ExportValue(node, fReferenceCellWidth, "ReferenceCellWidth");
    ExportValue(node, fCellWidthUpperBound, "CellWidthUpperBound");
    ExportValue(node, fFieldWireDiameter, "FieldWireDiameter");
    ExportValue(node, fSenseWireDiameter, "SenseWireDiameter");
    ExportValue(node, fMinAdjacentSuperLayersDistance, "MinAdjacentSuperLayersDistance");
    ExportValue(node, fMinWireAndRadialShellDistance, "MinWireAndRadialShellDistance");
    ExportValue(node, fEndCapThickness, "EndCapThickness");
    ExportValue(node, fInnerShellAlThickness, "InnerShellAlThickness");
    ExportValue(node, fInnerShellMylarThickness, "InnerShellMylarThickness");
    ExportValue(node, fOuterShellThickness, "OuterShellThickness");
    // Material
    ExportValue(node, fGasButaneFraction, "GasButaneFraction");
    ExportValue(node, fEndCapMaterialName, "EndCapMaterialName");
    ExportValue(node, fOuterShellCFRPDensity, "OuterShellCFRPDensity");
    // Detection
    ExportValue(node, fMeanDriftVelocity, "MeanDriftVelocity");
    ExportValue(node, fTimeResolutionFWHM, "TimeResolutionFWHM");
}

} // namespace MACE::Detector::Description
