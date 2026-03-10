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

#include "Mustard/Detector/Description/DescriptionWithCacheBase.h++"

#include "Eigen/Core"

#include "muc/array"
#include "muc/hash_map"
#include "muc/math"

#include "gsl/gsl"

#include <bit>
#include <cinttypes>
#include <vector>

class G4Material;

namespace MACE::Detector::Description {

class CDC final : public Mustard::Detector::Description::DescriptionWithCacheBase<CDC> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    CDC();
    ~CDC() override = default;

public:
    ///////////////////////////////////////////////////////////
    // Geometry
    ///////////////////////////////////////////////////////////

    auto EvenSuperLayerIsAxial() const -> auto { return *fEvenSuperLayerIsAxial; }
    auto NSuperLayer() const -> auto { return *fNSuperLayer; }
    auto NSenseLayerPerSuper() const -> auto { return *fNSenseLayerPerSuper; }
    auto GasInnerRadius() const -> auto { return *fGasInnerRadius; }
    auto GasInnerLength() const -> auto { return *fGasInnerLength; }
    auto EndCapSlope() const -> auto { return *fEndCapSlope; }
    auto MinStereoAngle() const -> auto { return *fMinStereoAngle; }
    auto CellWidthLowerBound() const -> auto { return *fCellWidthLowerBound; }
    auto ReferenceCellWidth() const -> auto { return *fReferenceCellWidth; }
    auto CellWidthUpperBound() const -> auto { return *fCellWidthUpperBound; }
    auto FieldWireDiameter() const -> auto { return *fFieldWireDiameter; }
    auto SenseWireDiameter() const -> auto { return *fSenseWireDiameter; }
    auto MinAdjacentSuperLayersDistance() const -> auto { return *fMinAdjacentSuperLayersDistance; }
    auto MinWireAndRadialShellDistance() const -> auto { return *fMinWireAndRadialShellDistance; }
    auto EndCapThickness() const -> auto { return *fEndCapThickness; }
    auto InnerShellAlThickness() const -> auto { return *fInnerShellAlThickness; }
    auto InnerShellMylarThickness() const -> auto { return *fInnerShellMylarThickness; }
    auto OuterShellThickness() const -> auto { return *fOuterShellThickness; }
    auto LayerConfiguration() const -> const auto& { return *fLayerConfiguration; }
    auto GasOuterRadius() const -> auto { return LayerConfiguration().back().outerRadius + fMinWireAndRadialShellDistance; }
    auto GasOuterLength() const -> auto { return fGasInnerLength + 2 * fEndCapSlope * (GasOuterRadius() - fGasInnerRadius); }
    auto CellMap() const -> const auto& { return *fCellMap; }
    auto CellMapFromSenseLayerIDAndLocalCellID() const -> const auto& { return *fCellMapFromSenseLayerIDAndLocalCellID; }

    auto EvenSuperLayerIsAxial(bool val) -> void { fEvenSuperLayerIsAxial = val; }
    auto NSuperLayer(int val) -> void { fNSuperLayer = val; }
    auto NSenseLayerPerSuper(int val) -> void { fNSenseLayerPerSuper = val; }
    auto GasInnerRadius(double val) -> void { fGasInnerRadius = val; }
    auto GasInnerLength(double val) -> void { fGasInnerLength = val; }
    auto EndCapSlope(double val) -> void { fEndCapSlope = val; }
    auto MinStereoAngle(double val) -> void { fMinStereoAngle = val; }
    auto CellWidthLowerBound(double val) -> void { fCellWidthLowerBound = val; }
    auto ReferenceCellWidth(double val) -> void { fReferenceCellWidth = val; }
    auto CellWidthUpperBound(double val) -> void { fCellWidthUpperBound = val; }
    auto FieldWireDiameter(double val) -> void { fFieldWireDiameter = val; }
    auto SenseWireDiameter(double val) -> void { fSenseWireDiameter = val; }
    auto MinAdjacentSuperLayersDistance(double val) -> void { fMinAdjacentSuperLayersDistance = val; }
    auto MinWireAndRadialShellDistance(double val) -> void { fMinWireAndRadialShellDistance = val; }
    auto EndCapThickness(double val) -> void { fEndCapThickness = val; }
    auto InnerShellAlThickness(double val) -> void { fInnerShellAlThickness = val; }
    auto InnerShellMylarThickness(double val) -> void { fInnerShellMylarThickness = val; }
    auto OuterShellThickness(double val) -> void { fOuterShellThickness = val; }

    ///////////////////////////////////////////////////////////
    // Material
    ///////////////////////////////////////////////////////////

    auto GasButaneFraction() const -> auto { return *fGasButaneFraction; }
    auto EndCapMaterialName() const -> const auto& { return *fEndCapMaterialName; }
    auto OuterShellCFRPDensity() const -> auto { return *fOuterShellCFRPDensity; }

    auto GasButaneFraction(double val) -> void { fGasButaneFraction = val; }
    auto EndCapMaterialName(std::string val) -> void { fEndCapMaterialName = std::move(val); }
    auto OuterShellCFRPDensity(double val) -> void { fOuterShellCFRPDensity = val; }

    auto GasMaterial() const -> G4Material*;

    ///////////////////////////////////////////////////////////
    // Detection
    ///////////////////////////////////////////////////////////

    auto MeanDriftVelocity() const -> auto { return *fMeanDriftVelocity; }
    auto TimeResolutionFWHM() const -> auto { return *fTimeResolutionFWHM; }

    auto MeanDriftVelocity(double val) -> void { fMeanDriftVelocity = val; }
    auto TimeResolutionFWHM(double val) -> void { fTimeResolutionFWHM = val; }

public:
    struct SuperLayerConfiguration {
        struct SenseLayerConfiguration {
            struct CellConfiguration {
                int cellID{};
                double centerAzimuth{};
            };
            int senseLayerID{};
            double innerRadius{};
            double outerRadius{};
            double cellWidth{};
            double halfLength{};
            double stereoAzimuthAngle{};
            auto TanStereoZenithAngle(double r) const -> auto { return r * std::tan(stereoAzimuthAngle / 2) / halfLength; }
            auto SecStereoZenithAngle(double r) const -> auto { return std::sqrt(1 + muc::pow(TanStereoZenithAngle(r), 2)); }
            auto CosStereoZenithAngle(double r) const -> auto { return 1 / SecStereoZenithAngle(r); }
            auto SinStereoZenithAngle(double r) const -> auto { return TanStereoZenithAngle(r) / SecStereoZenithAngle(r); }
            auto StereoZenithAngle(double r) const -> auto { return std::atan(TanStereoZenithAngle(r)); }
            std::vector<CellConfiguration> cell{};
        };
        bool isAxial{};
        int superLayerID{};
        int nCellPerSenseLayer{};
        double cellAzimuthWidth{};
        double innerRadius{};
        double innerHalfLength{};
        double outerRadius{};
        double outerHalfLength{};
        std::vector<SenseLayerConfiguration> sense{};
    };

    struct CellInformation {
        int cellID{};
        int cellLocalID{};
        int senseLayerID{};
        int senseLayerLocalID{};
        int superLayerID{};
        Eigen::Vector2d position{};
        Eigen::Vector3d direction{};
        double senseWireHalfLength{};
        double centerAzimuth{};
    };

private:
    struct HashArray2i32 {
        constexpr auto operator()(muc::array2i32 i) const -> std::size_t {
            return std::bit_cast<std::uint64_t>(i);
        }
    };

public:
    using CellMapFromSenseLayerIDAndLocalCellIDType = muc::flat_hash_map<muc::array2i32, CellInformation, HashArray2i32>;

private:
    auto CalculateLayerConfiguration() const -> std::vector<SuperLayerConfiguration>;
    auto CalculateCellMap() const -> std::vector<CellInformation>;
    auto CalculateCellMapFromSenseLayerIDAndLocalCellID() const -> CellMapFromSenseLayerIDAndLocalCellIDType;

    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    ///////////////////////////////////////////////////////////
    // Geometry
    ///////////////////////////////////////////////////////////

    Simple<bool> fEvenSuperLayerIsAxial; // true: AVAUAVAU..., false: VAUAVAUA...
    Simple<int> fNSuperLayer;
    Simple<int> fNSenseLayerPerSuper;
    Simple<double> fGasInnerRadius;
    Simple<double> fGasInnerLength;
    Simple<double> fEndCapSlope;
    Simple<double> fMinStereoAngle;
    Simple<double> fCellWidthLowerBound;
    Simple<double> fReferenceCellWidth;
    Simple<double> fCellWidthUpperBound;
    Simple<double> fFieldWireDiameter;
    Simple<double> fSenseWireDiameter;
    Simple<double> fMinAdjacentSuperLayersDistance;
    Simple<double> fMinWireAndRadialShellDistance;
    Simple<double> fEndCapThickness;
    Simple<double> fInnerShellAlThickness;
    Simple<double> fInnerShellMylarThickness;
    Simple<double> fOuterShellThickness;

    Cached<std::vector<SuperLayerConfiguration>> fLayerConfiguration;
    Cached<std::vector<CellInformation>> fCellMap;
    Cached<CellMapFromSenseLayerIDAndLocalCellIDType> fCellMapFromSenseLayerIDAndLocalCellID;

    ///////////////////////////////////////////////////////////
    // Material
    ///////////////////////////////////////////////////////////

    Simple<double> fGasButaneFraction;
    Simple<std::string> fEndCapMaterialName;
    Simple<double> fOuterShellCFRPDensity;

    ///////////////////////////////////////////////////////////
    // Detection
    ///////////////////////////////////////////////////////////

    Simple<double> fMeanDriftVelocity;
    Simple<double> fTimeResolutionFWHM;
};

} // namespace MACE::Detector::Description
