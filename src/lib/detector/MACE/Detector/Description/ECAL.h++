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
#include "Mustard/Math/GeometryRepresentation.h++"
#include "Mustard/Math/Vector.h++"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "gtl/phmap.hpp"

#include "muc/array"

#include "gsl/gsl"

#include <vector>

namespace MACE::Detector::Description {

class ECAL final : public Mustard::Detector::Description::DescriptionWithCacheBase<ECAL> {
    friend Mustard::Env::SingletonFactory;

private:
    ECAL();
    ~ECAL() = default;

public:
    auto NSubdivision() const -> auto { return *fNSubdivision; }
    auto InnerRadius() const -> auto { return *fInnerRadius; }
    auto CrystalHypotenuse() const -> auto { return *fCrystalHypotenuse; }
    auto CrystalPackageThickness() const -> auto { return *fCrystalPackageThickness; }
    auto UpstreamWindowRadius() const -> auto { return *fUpstreamWindowRadius; }
    auto DownstreamWindowRadius() const -> auto { return *fDownstreamWindowRadius; }

    auto Array() const -> const auto& { return *fArray; }
    auto NUnit() const -> auto { return Array().moduleList.size(); }
    auto ComputeTransformToOuterSurfaceWithOffset(int moduleID, double offsetInNormalDirection) const -> HepGeom::Transform3D;
    auto ModuleSelection() const -> const auto& { return *fModuleSelection; }

    auto ScintillationEnergyBin() const -> auto& { return *fScintillationEnergyBin; }
    auto ScintillationComponent1() const -> auto& { return *fScintillationComponent1; }
    auto ScintillationYield() const -> auto { return *fScintillationYield; }
    auto ScintillationTimeConstant1() const -> auto { return *fScintillationTimeConstant1; }
    auto ResolutionScale() const -> auto { return *fResolutionScale; }

    auto MPPCNPixelRowSet() const -> const auto& { return *fMPPCNPixelRowSet; }
    auto MPPCPixelSizeSet() const -> const auto& { return *fMPPCPixelSizeSet; }
    auto MPPCPitch() const -> auto { return *fMPPCPitch; }
    auto MPPCThickness() const -> auto { return *fMPPCThickness; }
    auto MPPCCouplerThickness() const -> auto { return *fMPPCCouplerThickness; }
    auto MPPCWindowThickness() const -> auto { return *fMPPCWindowThickness; }
    auto MPPCEnergyBin() const -> const auto& { return *fMPPCEnergyBin; }
    auto MPPCEfficiency() const -> const auto& { return *fMPPCEfficiency; }

    auto WaveformIntegralTime() const -> auto { return *fWaveformIntegralTime; }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    auto NSubdivision(int val) -> void { fNSubdivision = val; }
    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto CrystalHypotenuse(double val) -> void { fCrystalHypotenuse = val; }
    auto CrystalPackageThickness(double val) -> void { fCrystalPackageThickness = val; }
    auto UpstreamWindowRadius(double val) -> void { fUpstreamWindowRadius = val; }
    auto DownstreamWindowRadius(double val) -> void { fDownstreamWindowRadius = val; }
    auto ModuleSelection(std::vector<int> val) { fModuleSelection = std::move(val); }

    auto ScintillationEnergyBin(std::vector<double> val) -> void { fScintillationEnergyBin = std::move(val); }
    auto ScintillationComponent1(std::vector<double> val) -> void { fScintillationComponent1 = std::move(val); }
    auto ScintillationYield(double val) -> void { fScintillationYield = val; }
    auto ScintillationTimeConstant1(double val) -> void { fScintillationTimeConstant1 = val; }
    auto ResolutionScale(double val) -> void { fResolutionScale = val; }

    auto MPPCNPixelRowSet(std::vector<int> val) -> void { fMPPCNPixelRowSet = std::move(val); }
    auto MPPCPixelSizeSet(std::vector<double> val) -> void { fMPPCPixelSizeSet = std::move(val); }
    auto MPPCPitch(double val) -> void { fMPPCPitch = val; }
    auto MPPCThickness(double val) -> void { fMPPCThickness = val; }
    auto MPPCCouplerThickness(double val) -> void { fMPPCCouplerThickness = val; }
    auto MPPCWindowThickness(double val) -> void { fMPPCWindowThickness = val; }
    auto MPPCEnergyBin(std::vector<double> val) -> void { fMPPCEnergyBin = std::move(val); }
    auto MPPCEfficiency(std::vector<double> val) -> void { fMPPCEfficiency = std::move(val); }

    auto WaveformIntegralTime(double val) { fWaveformIntegralTime = val; }

private:
    struct ArrayInformation {
        struct Module {
            int moduleID{};
            gtl::flat_hash_set<int> neighborModuleID{};
            int typeID{};
            Mustard::Point3D centroid{};
            Mustard::Vector3D normal{};
            std::vector<gsl::index> vertexIndex{};
        };
        std::vector<Mustard::Point3D> vertexList{};
        std::vector<Module> moduleList{};
    };

private:
    auto CalculateArrayInformation() const -> ArrayInformation;

    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    Simple<int> fNSubdivision;
    Simple<double> fInnerRadius;
    Simple<double> fCrystalHypotenuse;
    Simple<double> fCrystalPackageThickness;
    Simple<double> fUpstreamWindowRadius;
    Simple<double> fDownstreamWindowRadius;
    Cached<ArrayInformation> fArray;
    Simple<std::vector<int>> fModuleSelection;

    Simple<std::vector<double>> fScintillationEnergyBin;
    Simple<std::vector<double>> fScintillationComponent1;
    Simple<double> fScintillationYield;
    Simple<double> fScintillationTimeConstant1;
    Simple<double> fResolutionScale;

    Simple<std::vector<int>> fMPPCNPixelRowSet;
    Simple<std::vector<double>> fMPPCPixelSizeSet;
    Simple<double> fMPPCPitch;
    Simple<double> fMPPCThickness;
    Simple<double> fMPPCCouplerThickness;
    Simple<double> fMPPCWindowThickness;
    Simple<std::vector<double>> fMPPCEnergyBin;
    Simple<std::vector<double>> fMPPCEfficiency;

    Simple<double> fWaveformIntegralTime;
};

} // namespace MACE::Detector::Description
