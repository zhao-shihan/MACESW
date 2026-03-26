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

#include "MACE/Detector/Description/ECAL.h++"

#include "Mustard/IO/Print.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/utilities.h"
#include "pmp/surface_mesh.h"

#include "muc/math"

#include "fmt/std.h"

#include <concepts>
#include <queue>
#include <ranges>

namespace MACE::Detector::Description {

namespace {

static_assert(std::same_as<pmp::Scalar, double>, "PMP should be compiled with PMP_SCALAR_TYPE_64");

using namespace Mustard::MathConstant;

class ECALMesh {
public:
    explicit ECALMesh(int n);
    auto Generate() && -> auto;

private:
    auto GenerateIcosahedron() -> void;
    auto GenerateIcosphere() -> void;
    auto GenerateDualMesh() -> void;

private:
    pmp::SurfaceMesh fPMPMesh;
    int fNSubdivision;
};

ECALMesh::ECALMesh(int n) :
    fPMPMesh{},
    fNSubdivision{n} {}

auto ECALMesh::Generate() && -> auto {
    GenerateIcosahedron();
    GenerateIcosphere();
    GenerateDualMesh();
    return fPMPMesh;
}

auto ECALMesh::GenerateIcosahedron() -> void {
    // vertices coordinates (not normalized)
    constexpr auto a0 = 1.0;
    constexpr auto b0 = 1.0 / phi;
    // normalized vertices coordinates
    const auto a = a0 / muc::hypot(a0, b0);
    const auto b = b0 / muc::hypot(a0, b0);

    // add normalized vertices
    const auto v1 = fPMPMesh.add_vertex(pmp::Point{0, b, -a});
    const auto v2 = fPMPMesh.add_vertex(pmp::Point{b, a, 0});
    const auto v3 = fPMPMesh.add_vertex(pmp::Point{-b, a, 0});
    const auto v4 = fPMPMesh.add_vertex(pmp::Point{0, b, a});
    const auto v5 = fPMPMesh.add_vertex(pmp::Point{0, -b, a});
    const auto v6 = fPMPMesh.add_vertex(pmp::Point{-a, 0, b});
    const auto v7 = fPMPMesh.add_vertex(pmp::Point{0, -b, -a});
    const auto v8 = fPMPMesh.add_vertex(pmp::Point{a, 0, -b});
    const auto v9 = fPMPMesh.add_vertex(pmp::Point{a, 0, b});
    const auto v10 = fPMPMesh.add_vertex(pmp::Point{-a, 0, -b});
    const auto v11 = fPMPMesh.add_vertex(pmp::Point{b, -a, 0});
    const auto v12 = fPMPMesh.add_vertex(pmp::Point{-b, -a, 0});

    // add triangles
    fPMPMesh.add_triangle(v3, v2, v1);
    fPMPMesh.add_triangle(v2, v3, v4);
    fPMPMesh.add_triangle(v6, v5, v4);
    fPMPMesh.add_triangle(v5, v9, v4);
    fPMPMesh.add_triangle(v8, v7, v1);
    fPMPMesh.add_triangle(v7, v10, v1);
    fPMPMesh.add_triangle(v12, v11, v5);
    fPMPMesh.add_triangle(v11, v12, v7);
    fPMPMesh.add_triangle(v10, v6, v3);
    fPMPMesh.add_triangle(v6, v10, v12);
    fPMPMesh.add_triangle(v9, v8, v2);
    fPMPMesh.add_triangle(v8, v9, v11);
    fPMPMesh.add_triangle(v3, v6, v4);
    fPMPMesh.add_triangle(v9, v2, v4);
    fPMPMesh.add_triangle(v10, v3, v1);
    fPMPMesh.add_triangle(v2, v8, v1);
    fPMPMesh.add_triangle(v12, v10, v7);
    fPMPMesh.add_triangle(v8, v11, v7);
    fPMPMesh.add_triangle(v6, v12, v5);
    fPMPMesh.add_triangle(v11, v9, v5);
}

auto ECALMesh::GenerateIcosphere() -> void {
    // do division
    for (auto i = 0; i < fNSubdivision; ++i) {
        // do sub-division
        pmp::loop_subdivision(fPMPMesh);
        // project to unit sphere
        for (auto&& v : fPMPMesh.vertices()) {
            fPMPMesh.position(v).normalize();
        }
    }
}

auto ECALMesh::GenerateDualMesh() -> void {
    // the new dual mesh
    pmp::SurfaceMesh dualMesh;

    // a property to remember new vertices per face
    auto faceVertex = fPMPMesh.add_face_property<pmp::Vertex>("f:vertex");

    // for each face add the centroid to the dual mesh
    for (auto&& f : fPMPMesh.faces()) {
        pmp::Point centroid{0, 0, 0};
        pmp::Scalar n{0};
        for (auto&& v : fPMPMesh.vertices(f)) {
            centroid += fPMPMesh.position(v);
            ++n;
        }
        centroid /= n;
        faceVertex[f] = dualMesh.add_vertex(centroid);
    }

    // add new face for each vertex
    for (auto&& v : fPMPMesh.vertices()) {
        std::vector<pmp::Vertex> vertices;
        vertices.reserve(6);
        for (auto&& f : fPMPMesh.faces(v)) {
            vertices.emplace_back(faceVertex[f]);
        }
        dualMesh.add_face(vertices);
    }

    // swap old and new meshes, don't copy properties
    fPMPMesh.assign(dualMesh);
}

} // namespace

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::PhysicalConstant;

ECAL::ECAL() :
    DescriptionWithCacheBase{"ECAL"},
    // geometry param.s
    fNSubdivision{this, 3},
    fInnerRadius{this, 200_mm},
    fCrystalHypotenuse{this, 100_mm},
    fCrystalPackageThickness{this, 0.2_mm},
    fUpstreamWindowRadius{this, 50_mm},
    fDownstreamWindowRadius{this, 5_mm},
    fArray{this, [this] { return CalculateArrayInformation(); }},
    fModuleSelection{this, {}},
    // crystal param.s
    fScintillationEnergyBin{this, {}},
    fScintillationComponent1{this, {}},
    fScintillationYield{this, 3500.},
    fScintillationTimeConstant1{this, 30_ns},
    fResolutionScale{this, 1.},
    // sensor param.s
    fMPPCNPixelRowSet{this, {}},
    fMPPCPixelSizeSet{this, {}},
    fMPPCPitch{this, 0.2_mm},     // gap between pixels
    fMPPCThickness{this, 0.1_mm}, // cathode
    fMPPCCouplerThickness{this, 0.1_mm},
    fMPPCWindowThickness{this, 0.2_mm},
    fMPPCEnergyBin{this, {}},
    fMPPCEfficiency{this, {}},
    // 0.1(epoxy)+0.1(cathode),window change  from epoxy to epoxy&silicon Pixels, may change name "window" later
    fWaveformIntegralTime{this, fScintillationTimeConstant1 * 7} {
    fScintillationEnergyBin = {3.300891_eV, 3.394291_eV, 3.459551_eV, 3.515883_eV, 3.557591_eV, 3.591915_eV,
                               3.622042_eV, 3.644458_eV, 3.678815_eV, 3.690132_eV, 3.715531_eV, 3.728362_eV,
                               3.747776_eV, 3.768708_eV, 3.787216_eV, 3.80725_eV, 3.820723_eV, 3.847958_eV,
                               3.871416_eV, 3.9022_eV, 3.932045_eV, 4.042466_eV, 4.157667_eV, 4.193192_eV,
                               4.224366_eV, 4.24096_eV, 4.257685_eV, 4.274542_eV, 4.291534_eV, 4.306942_eV,
                               4.343328_eV, 4.359111_eV, 4.396388_eV, 4.437953_eV, 4.471035_eV, 4.511202_eV,
                               4.546346_eV, 4.602552_eV, 4.662178_eV, 4.733725_eV, 4.883613_eV, 5.114986_eV};
    fScintillationComponent1 = {0.152652, 0.190301, 0.233538, 0.282997, 0.328661, 0.379536,
                                0.430119, 0.47672, 0.531663, 0.57447, 0.617499, 0.656175,
                                0.695466, 0.729048, 0.768509, 0.803861, 0.841795, 0.88913,
                                0.937399, 0.977291, 1.0, 0.972946, 0.880537, 0.821459,
                                0.754517, 0.691979, 0.640116, 0.591668, 0.540922, 0.501151,
                                0.452269, 0.413595, 0.35813, 0.307095, 0.264469, 0.218419,
                                0.182053, 0.129528, 0.081253, 0.047185, 0.017148, 0.0};
    fMPPCNPixelRowSet = {12, 8, 12, 12, 12, 12, 12, 12, 12, 12};
    fMPPCPixelSizeSet = {3_mm, 3_mm, 3_mm, 3_mm, 3_mm, 3_mm, 3_mm, 3_mm, 3_mm, 3_mm};
    fMPPCEnergyBin = {1.771068_eV, 2.101763_eV, 2.478823_eV, 2.66882_eV, 2.756137_eV, 2.849362_eV, 2.949113_eV, 3.02425_eV, 3.099632_eV, 3.178868_eV, 3.3981_eV, 3.649811_eV, 3.877416_eV, 4.128759_eV, 4.348776_eV};
    fMPPCEfficiency = {0.26319, 0.395706, 0.503681, 0.525767, 0.555215, 0.570552, 0.597546, 0.59816, 0.599387, 0.593252, 0.521472, 0.466258, 0.458896, 0.408589, 0.196933};
}

auto ECAL::CalculateArrayInformation() const -> ArrayInformation {
    auto pmpMesh{ECALMesh{fNSubdivision}.Generate()};
    ArrayInformation outputArrayInfo;
    auto& [vertexList, moduleList]{outputArrayInfo};
    const auto point{pmpMesh.vertex_property<pmp::Point>("v:point")};
    // construct vertexList
    for (auto&& v : pmpMesh.vertices()) {
        vertexList.emplace_back(Mustard::VectorCast<Mustard::Point3D>(point[v]));
    }

    // map from pmp face index to moduleID
    using ModuleID = int;
    muc::flat_hash_map<pmp::IndexType, ModuleID> indexMap;
    for (int moduleID{};
         auto&& pmpFace : pmpMesh.faces()) {
        const auto centroid{Mustard::VectorCast<Mustard::Point3D>(pmp::centroid(pmpMesh, pmpFace))};
        if (const auto rXY{fInnerRadius * centroid.perp()};
            centroid.z() < 0) {
            if (rXY < fUpstreamWindowRadius) {
                continue;
            }
        } else {
            if (rXY < fDownstreamWindowRadius) {
                continue;
            }
        }
        if (std::ranges::any_of(pmpMesh.vertices(pmpFace),
                                [&](const auto& v) {
                                    const auto rXY{fInnerRadius * muc::hypot(point[v][0], point[v][1])};
                                    if (point[v][2] < 0) {
                                        return rXY < fUpstreamWindowRadius;
                                    }
                                    return rXY < fDownstreamWindowRadius;
                                })) {
            continue;
        }
        indexMap[pmpFace.idx()] = moduleID++;
    }

    // construct moduleList
    for (auto&& pmpFace : pmpMesh.faces()) {
        if (not indexMap.contains(pmpFace.idx())) {
            continue;
        }

        auto& face{moduleList.emplace_back()};
        face.moduleID = indexMap.at(pmpFace.idx());
        face.centroid = Mustard::VectorCast<Mustard::Point3D>(pmp::centroid(pmpMesh, pmpFace));
        face.normal = Mustard::VectorCast<Mustard::Vector3D>(pmp::face_normal(pmpMesh, pmpFace));
        for (auto&& pmpFaceVertex : pmpMesh.vertices(pmpFace)) {
            for (auto&& pmpVertexFace : pmpMesh.faces(pmpFaceVertex)) {
                if (pmpVertexFace != pmpFace and indexMap.contains(pmpVertexFace.idx())) {
                    face.neighborModuleID.insert(indexMap.at(pmpVertexFace.idx()));
                }
            }
        }

        for (auto&& v : pmpMesh.vertices(pmpFace)) {
            face.vertexIndex.emplace_back(v.idx());
        }
        // vertex ordering of a face
        const auto localPhi{
            [uHat = (vertexList[face.vertexIndex.front()] - face.centroid).unit(),
             vHat = face.normal.cross(vertexList[face.vertexIndex.front()] - face.centroid).unit(),
             &localOrigin = face.centroid,
             &vertex = vertexList](const auto& i) {
                const auto localPoint = vertex[i] - localOrigin;
                return std::atan2(localPoint.dot(vHat), localPoint.dot(uHat));
            }};
        std::ranges::sort(face.vertexIndex,
                          [&localPhi](const auto& i, const auto& j) {
                              return localPhi(i) < localPhi(j);
                          });
    }

    // construct type mapping
    using PolygonEdges = std::vector<double>;
    std::multimap<PolygonEdges, ModuleID> edgeLengthsMap;

    for (auto&& [moduleID, _1, _2, centroid, _3, vertexIndex] : std::as_const(moduleList)) {
        // edge lengths for type identifying
        std::vector<G4ThreeVector> vertexCoordinates{vertexIndex.size()};
        std::ranges::transform(vertexIndex, vertexCoordinates.begin(),
                               [&](auto&& aIndex) { return vertexList[aIndex]; });

        PolygonEdges edges;
        constexpr int reservedDigit{std::numeric_limits<double>::digits10 / 2};
        for (int i{}; i < std::ssize(vertexCoordinates); ++i) {
            const auto& current{vertexCoordinates[i]};
            const auto& next{vertexCoordinates[(i + 1) % vertexCoordinates.size()]};
            edges.emplace_back(muc::round_to((next - current).mag(), reservedDigit));
        }
        std::ranges::sort(edges);
        edgeLengthsMap.insert({edges, moduleID});
    }

    int typeID{};
    for (auto it{edgeLengthsMap.begin()}; it != edgeLengthsMap.end();) {
        auto range{edgeLengthsMap.equal_range(it->first)};
        for (auto aPolygon{range.first}; aPolygon != range.second; aPolygon = std::next(aPolygon)) {
            moduleList[aPolygon->second].typeID = typeID;
        }
        ++typeID;
        it = range.second;
    }

    Mustard::MasterPrintLn<'I'>("\n======================================================");
    Mustard::MasterPrintLn<'I'>("#Information for ECAL");
    Mustard::MasterPrintLn<'I'>("## ECAL Module Sorting\n");

    typeID = 0;
    auto it{edgeLengthsMap.begin()};

    while (it != edgeLengthsMap.end()) {
        auto currentEdgeLengths{it->first};
        const auto range{edgeLengthsMap.equal_range(currentEdgeLengths)};
        const std::ranges::subrange equalRange{range.first, range.second};
        Mustard::MasterPrintLn<'I'>("### Type {}: \n", typeID);
        Mustard::MasterPrintLn<'I'>("- lengths: ");
        Mustard::MasterPrintLn<'I'>("{}, ", currentEdgeLengths);
        Mustard::MasterPrintLn<'I'>("\n- modules ({} in total):", std::ranges::distance(equalRange));

        for (auto&& [_, moduleID] : equalRange) {
            Mustard::MasterPrint<'I'>("{}, ", moduleID);
        }
        Mustard::MasterPrintLn<'I'>("\n===========================");

        ++typeID;
        it = range.second;
    }
    if (not fModuleSelection->empty()) {
        std::unordered_set<int> neighborModuleSet;
        Mustard::MasterPrintLn<'I'>("### Selected Module Clustering ");
        for (auto&& m : *fModuleSelection) {
            neighborModuleSet.insert(m);
            Mustard::MasterPrintLn<'I'>("\n- Module {}", m);
            Mustard::MasterPrint<'I'>("{},", m);
            for (auto&& n : moduleList.at(m).neighborModuleID) {
                neighborModuleSet.insert(n);
                neighborModuleSet.insert(moduleList.at(n).neighborModuleID.begin(), moduleList.at(n).neighborModuleID.end());
                Mustard::MasterPrint<'I'>("{},", n);
            }
        }
        Mustard::MasterPrint<'I'>("\n");
        Mustard::MasterPrint<'I'>("\n- Summary: ");
        for (auto&& m : neighborModuleSet) {
            Mustard::MasterPrint<'I'>("{},", m);
        }
    }
    Mustard::MasterPrintLn<'I'>("\n======================================================");

    return outputArrayInfo;
}

auto ECAL::ComputeTransformToOuterSurfaceWithOffset(int moduleID, double offsetInNormalDirection) const -> HepGeom::Transform3D {
    const auto& moduleList{Array().moduleList};
    auto&& [_1, _2, _3, centroid, normal, vertexIndex]{moduleList[moduleID]};

    const auto centroidMagnitude{centroid.mag()};
    const auto crystalOuterRadius{(fInnerRadius + fCrystalHypotenuse) * centroidMagnitude};

    const auto crystalOuterCentroid{crystalOuterRadius * centroid / centroidMagnitude};
    const G4Rotate3D rotation{normal.theta(), CLHEP::HepZHat.cross(normal)};

    return G4Translate3D{crystalOuterCentroid + offsetInNormalDirection * normal} * rotation;
}

auto ECAL::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue(node, fNSubdivision, "NSubdivision");
    ImportValue(node, fInnerRadius, "InnerRadius");
    ImportValue(node, fCrystalHypotenuse, "CrystalHypotenuse");
    ImportValue(node, fCrystalPackageThickness, "CrystalPackageThickness");
    ImportValue(node, fUpstreamWindowRadius, "UpstreamWindowRadius");
    ImportValue(node, fDownstreamWindowRadius, "DownstreamWindowRadius");
    ImportValue(node, fModuleSelection, "ModuleSelection");
    ImportValue(node, fScintillationEnergyBin, "ScintillationEnergyBin");
    ImportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ImportValue(node, fScintillationYield, "ScintillationYield");
    ImportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    ImportValue(node, fResolutionScale, "ResolutionScale");
    ImportValue(node, fMPPCNPixelRowSet, "MPPCNPixelRowSet");
    ImportValue(node, fMPPCPixelSizeSet, "MPPCPixelSizeSet");
    ImportValue(node, fMPPCPitch, "MPPCPitch");
    ImportValue(node, fMPPCThickness, "MPPCThickness");
    ImportValue(node, fMPPCCouplerThickness, "MPPCCouplerThickness");
    ImportValue(node, fMPPCWindowThickness, "MPPCWindowThickness");
    ImportValue(node, fMPPCEnergyBin, "MPPCEnergyBin");
    ImportValue(node, fMPPCEfficiency, "MPPCEfficiency");
    ImportValue(node, fWaveformIntegralTime, "WaveformIntegralTime");
}

auto ECAL::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fNSubdivision, "NSubdivision");
    ExportValue(node, fInnerRadius, "InnerRadius");
    ExportValue(node, fCrystalHypotenuse, "CrystalHypotenuse");
    ExportValue(node, fCrystalPackageThickness, "CrystalPackageThickness");
    ExportValue(node, fUpstreamWindowRadius, "UpstreamWindowRadius");
    ExportValue(node, fDownstreamWindowRadius, "DownstreamWindowRadius");
    ExportValue(node, fModuleSelection, "ModuleSelection");
    ExportValue(node, fScintillationEnergyBin, "ScintillationEnergyBin");
    ExportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ExportValue(node, fScintillationYield, "ScintillationYield");
    ExportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    ExportValue(node, fResolutionScale, "ResolutionScale");
    ExportValue(node, fMPPCNPixelRowSet, "MPPCNPixelRowSet");
    ExportValue(node, fMPPCPixelSizeSet, "MPPCPixelSizeSet");
    ExportValue(node, fMPPCPitch, "MPPCPitch");
    ExportValue(node, fMPPCThickness, "MPPCThickness");
    ExportValue(node, fMPPCCouplerThickness, "MPPCCouplerThickness");
    ExportValue(node, fMPPCWindowThickness, "MPPCWindowThickness");
    ExportValue(node, fMPPCEnergyBin, "MPPCEnergyBin");
    ExportValue(node, fMPPCEfficiency, "MPPCEfficiency");
    ExportValue(node, fWaveformIntegralTime, "WaveformIntegralTime");
}

} // namespace MACE::Detector::Description
