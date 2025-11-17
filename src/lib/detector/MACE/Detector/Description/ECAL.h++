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

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "muc/array"
#include "muc/hash_map"

#include "gsl/gsl"

#include <unordered_set>
#include <vector>

namespace MACE::Detector::Description {

class ECAL final : public Mustard::Detector::Description::DescriptionWithCacheBase<ECAL> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    ECAL();
    ~ECAL() override = default;

public:
    auto NSubdivision() const -> auto { return *fNSubdivision; }
    auto InnerRadius() const -> auto { return *fInnerRadius; }
    auto CrystalHypotenuse() const -> auto { return *fCrystalHypotenuse; }
    auto CrystalPackageThickness() const -> auto { return *fCrystalPackageThickness; }
    auto UpstreamWindowRadius() const -> auto { return *fUpstreamWindowRadius; }
    auto DownstreamWindowRadius() const -> auto { return *fDownstreamWindowRadius; }
    auto ScintillationEnergyBin() const -> auto& { return *fScintillationEnergyBin; }
    auto ScintillationComponent1() const -> auto& { return *fScintillationComponent1; }
    auto ScintillationYield() const -> auto { return *fScintillationYield; }
    auto ScintillationTimeConstant1() const -> auto { return *fScintillationTimeConstant1; }
    auto ResolutionScale() const -> auto { return *fResolutionScale; }

    auto UseMPPC() const -> auto { return *fUseMPPC; }

    auto PMTDimensions() const -> const auto& { return *fPMTDimensions; }
    auto PMTCouplerThickness() const -> auto { return *fPMTCouplerThickness; }
    auto PMTWindowThickness() const -> auto { return *fPMTWindowThickness; }
    auto PMTCathodeThickness() const -> auto { return *fPMTCathodeThickness; }
    auto PMTEnergyBin() const -> const auto& { return *fPMTEnergyBin; }
    auto PMTQuantumEfficiency() const -> const auto& { return *fPMTQuantumEfficiency; }

    auto MPPCNPixelRow() const -> const auto& { return *fMPPCNPixelRow; }
    auto MPPCPixelSizeSet() const -> const auto& { return *fMPPCPixelSizeSet; }
    auto MPPCPitch() const -> auto { return *fMPPCPitch; }
    auto MPPCThickness() const -> auto { return *fMPPCThickness; }
    auto MPPCCouplerThickness() const -> auto { return *fMPPCCouplerThickness; }
    auto MPPCWindowThickness() const -> auto { return *fMPPCWindowThickness; }
    auto MPPCEnergyBin() const -> const auto& { return *fMPPCEnergyBin; }
    auto MPPCEfficiency() const -> const auto& { return *fMPPCEfficiency; }

    auto Mesh() const -> const auto& { return *fMesh; }
    auto NUnit() const -> auto { return Mesh().faceList.size(); }
    auto ComputeTransformToOuterSurfaceWithOffset(int moduleID, double offsetInNormalDirection) const -> HepGeom::Transform3D;

    auto ModuleSelection() const -> const auto& { return *fModuleSelection; }
    auto WaveformIntegralTime() const -> auto { return *fWaveformIntegralTime; }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    auto NSubdivision(int val) -> void { fNSubdivision = val; }
    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto CrystalHypotenuse(double val) -> void { fCrystalHypotenuse = val; }
    auto CrystalPackageThickness(double val) -> void { fCrystalPackageThickness = val; }
    auto UpstreamWindowRadius(double val) -> void { fUpstreamWindowRadius = val; }
    auto DownstreamWindowRadius(double val) -> void { fDownstreamWindowRadius = val; }
    auto ScintillationEnergyBin(std::vector<double> val) -> void { fScintillationEnergyBin = std::move(val); }
    auto ScintillationComponent1(std::vector<double> val) -> void { fScintillationComponent1 = std::move(val); }
    auto ScintillationYield(double val) -> void { fScintillationYield = val; }
    auto ScintillationTimeConstant1(double val) -> void { fScintillationTimeConstant1 = val; }
    auto ResolutionScale(double val) -> void { fResolutionScale = val; }

    auto UseMPPC(bool val) -> void { fUseMPPC = val; }

    auto PMTDimensions(std::vector<muc::array3d> val) -> void { fPMTDimensions = std::move(val); }
    auto PMTCouplerThickness(double val) -> void { fPMTCouplerThickness = val; }
    auto PMTWindowThickness(double val) -> void { fPMTWindowThickness = val; }
    auto PMTCathodeThickness(double val) -> void { fPMTCathodeThickness = val; }
    auto PMTEnergyBin(std::vector<double> val) -> void { fPMTEnergyBin = std::move(val); }
    auto PMTQuantumEfficiency(std::vector<double> val) -> void { fPMTQuantumEfficiency = std::move(val); }

    auto MPPCNPixelRow(std::vector<int> val) -> void { fMPPCNPixelRow = std::move(val); }
    auto MPPCPixelSizeSet(std::vector<double> val) -> void { fMPPCPixelSizeSet = std::move(val); }
    auto MPPCPitch(double val) -> void { fMPPCPitch = val; }
    auto MPPCThickness(double val) -> void { fMPPCThickness = val; }
    auto MPPCCouplerThickness(double val) -> void { fMPPCCouplerThickness = val; }
    auto MPPCWindowThickness(double val) -> void { fMPPCWindowThickness = val; }
    auto MPPCEnergyBin(std::vector<double> val) -> void { fMPPCEnergyBin = std::move(val); }
    auto MPPCEfficiency(std::vector<double> val) -> void { fMPPCEfficiency = std::move(val); }

    auto ModuleSelection(std::vector<int> val) { fModuleSelection = std::move(val); }
    auto WaveformIntegralTime(double val) { fWaveformIntegralTime = val; }

    struct MeshInformation {
        struct Module {
            CLHEP::Hep3Vector centroid{};
            CLHEP::Hep3Vector normal{};
            std::vector<gsl::index> vertexIndex{};
            int typeID{};
            std::unordered_set<int> neighborModuleID{};
        };
        std::vector<HepGeom::Point3D<double>> vertexList{};
        std::vector<Module> faceList{};
    };

private:
    auto CalculateMeshInformation() const -> MeshInformation;

    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    Simple<int> fNSubdivision;
    Simple<double> fInnerRadius;
    Simple<double> fCrystalHypotenuse;
    Simple<double> fCrystalPackageThickness;
    Simple<double> fUpstreamWindowRadius;
    Simple<double> fDownstreamWindowRadius;
    Simple<std::vector<double>> fScintillationEnergyBin;
    Simple<std::vector<double>> fScintillationComponent1;
    Simple<double> fScintillationYield;
    Simple<double> fScintillationTimeConstant1;
    Simple<double> fResolutionScale;

    Simple<bool> fUseMPPC;

    Simple<std::vector<muc::array3d>> fPMTDimensions;
    Simple<double> fPMTCouplerThickness;
    Simple<double> fPMTWindowThickness;
    Simple<double> fPMTCathodeThickness;
    Simple<std::vector<double>> fPMTEnergyBin;
    Simple<std::vector<double>> fPMTQuantumEfficiency;

    Simple<std::vector<int>> fMPPCNPixelRow;
    Simple<std::vector<double>> fMPPCPixelSizeSet;
    Simple<double> fMPPCPitch;
    Simple<double> fMPPCThickness;
    Simple<double> fMPPCCouplerThickness;
    Simple<double> fMPPCWindowThickness;
    Simple<std::vector<double>> fMPPCEnergyBin;
    Simple<std::vector<double>> fMPPCEfficiency;

    Cached<MeshInformation> fMesh;
    Simple<std::vector<int>> fModuleSelection;
    Simple<double> fWaveformIntegralTime;
};

} // namespace MACE::Detector::Description
