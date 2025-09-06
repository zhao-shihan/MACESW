#include "MACE/Detector/Description/ECAL.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "CLHEP/Vector/TwoVector.h"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/subdivision.h"
#include "pmp/algorithms/utilities.h"
#include "pmp/surface_mesh.h"

#include "muc/math"

#include <concepts>
#include <queue>
#include <ranges>

namespace MACE::Detector::Description {

namespace {

static_assert(std::same_as<pmp::Scalar, double>, "PMP should be compiled with PMP_SCALAR_TYPE_64");

using namespace Mustard::MathConstant;

class ECALMesh {
public:
    ECALMesh(int n);
    auto Generate() && -> auto;

private:
    auto GenerateIcosahedron() -> void;
    auto GenerateIcosphere() -> void;
    auto GenerateDualMesh() -> void;

private:
    pmp::SurfaceMesh fPMPMesh;
    const int fNSubdivision;
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
        dualMesh.add_face(std::move(vertices));
    }

    // swap old and new meshes, don't copy properties
    fPMPMesh.assign(dualMesh);
}

} // namespace

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::PhysicalConstant;

ECAL::ECAL() : // clang-format off
    DescriptionBase{"ECAL"}, // clang-format on
    fNSubdivision{3},
    fInnerRadius{20_cm},
    fCrystalHypotenuse{10_cm},
    fUpstreamWindowRadius{50_mm},
    fDownstreamWindowRadius{5_mm},
    // BGO optical param.s
    fScintillationEnergyBin{1.945507481_eV, 1.956691365_eV, 1.974526166_eV, 1.992686315_eV, 2.011182111_eV,
                            2.030023135_eV, 2.049218172_eV, 2.068776498_eV, 2.088712977_eV, 2.109036_eV,
                            2.12975713_eV, 2.150887751_eV, 2.171451222_eV, 2.190391881_eV, 2.209663573_eV,
                            2.229274743_eV, 2.247116958_eV, 2.265244608_eV, 2.283666187_eV, 2.301276603_eV,
                            2.318032238_eV, 2.333890214_eV, 2.349966255_eV, 2.368619281_eV, 2.389971082_eV,
                            2.412936123_eV, 2.437596391_eV, 2.464039018_eV, 2.492373025_eV, 2.521380095_eV,
                            2.551089015_eV, 2.581526478_eV, 2.612727511_eV, 2.644717279_eV, 2.677523383_eV,
                            2.708082211_eV, 2.733054836_eV, 2.75369941_eV, 2.773041191_eV, 2.792667214_eV,
                            2.810903696_eV, 2.827691812_eV, 2.844122034_eV, 2.861882767_eV, 2.877806421_eV,
                            2.894720751_eV, 2.911600212_eV, 2.929633378_eV, 2.947486552_eV, 2.965972235_eV,
                            2.984688528_eV, 3.003641613_eV, 3.024116133_eV, 3.04551166_eV, 3.067869957_eV,
                            3.091903493_eV, 3.118356404_eV, 3.147344518_eV, 3.178997019_eV, 3.215638088_eV,
                            3.253129005_eV, 3.281477983_eV, 3.302608153_eV},
    fScintillationComponent1{0.195254826, 0.214876564, 0.247266504, 0.281705118, 0.316502611, 0.351368412, 0.387289114,
                             0.424968103, 0.459795422, 0.494441782, 0.528656318, 0.562776635, 0.595129123, 0.628156693,
                             0.66160206, 0.695627571, 0.726322943, 0.757562051, 0.788164087, 0.818063582, 0.846719321,
                             0.873933468, 0.900421966, 0.925228448, 0.945965637, 0.963539434, 0.978127738, 0.991670468,
                             0.999665211, 1, 0.991116414, 0.973225812, 0.943719198, 0.904954051, 0.858628733, 0.81043525,
                             0.766603628, 0.72769847, 0.690508334, 0.650827878, 0.615158969, 0.582925251, 0.549468551,
                             0.518706558, 0.485613932, 0.454237279, 0.422103766, 0.389336957, 0.357120837, 0.324825964,
                             0.293908848, 0.263908054, 0.234764495, 0.208390636, 0.1828993, 0.156768192, 0.13171465,
                             0.107656381, 0.084704457, 0.061552239, 0.040184081, 0.024471847, 0.012727081},
    fScintillationYield{10000},
    fScintillationTimeConstant1{300_ns},
    fResolutionScale{1.},
    fUseMPPC{false},
    fPMTDimensions{{29.3_mm, 25_mm, 87_mm},    // 9442B Type-HEX01
                   {29.3_mm, 25_mm, 87_mm},    // 9442B Type-PEN
                   {39.9_mm, 32_mm, 98.5_mm},  // 9902B Type-HEX02
                   {39.9_mm, 32_mm, 98.5_mm},  // 9902B Type-HEX03
                   {39.9_mm, 32_mm, 98.5_mm},  // 9902B Type-HEX04
                   {39.9_mm, 32_mm, 98.5_mm},  // 9902B Type-HEX05
                   {39.9_mm, 32_mm, 98.5_mm},  // 9902B Type-HEX06
                   {39.9_mm, 32_mm, 98.5_mm},  // 9902B Type-HEX07
                   {39.9_mm, 32_mm, 98.5_mm},  // 9902B Type-HEX08
                   {39.9_mm, 32_mm, 98.5_mm}}, // 9902B Type-HEX09
    fPMTCouplerThickness{0.1_mm},
    fPMTWindowThickness{1_mm},
    fPMTCathodeThickness{20_nm},
    fPMTEnergyBin{1.732649965_eV, 1.760237951_eV, 1.803307489_eV, 1.848537542_eV, 1.896094863_eV, 1.946160761_eV, 1.997657575_eV,
                  2.045172376_eV, 2.092877718_eV, 2.123492226_eV, 2.145831664_eV, 2.168816794_eV, 2.18850057_eV, 2.204563937_eV,
                  2.221286508_eV, 2.242202221_eV, 2.265173439_eV, 2.290315074_eV, 2.319490762_eV, 2.352989916_eV, 2.387857013_eV,
                  2.422646099_eV, 2.453711564_eV, 2.48323493_eV, 2.511441394_eV, 2.542243582_eV, 2.574809989_eV, 2.617165988_eV,
                  2.665600148_eV, 2.716592531_eV, 2.794890081_eV, 2.902336797_eV, 3.021316555_eV, 3.150468334_eV, 3.291154851_eV,
                  3.444993614_eV, 3.568086447_eV, 3.6351373_eV, 3.69352408_eV, 3.729691757_eV, 3.773692253_eV, 3.806938806_eV,
                  3.836048038_eV, 3.8701779_eV, 3.893115743_eV, 3.918282138_eV, 3.954402073_eV, 3.973712174_eV, 4.006861856_eV,
                  4.035218382_eV, 4.0587919_eV, 4.097020153_eV, 4.111990233_eV, 4.142952897_eV, 4.181888035_eV, 4.215921914_eV,
                  4.248868735_eV, 4.293483423_eV, 4.340214275_eV, 4.411684499_eV},
    fPMTQuantumEfficiency{0.00206, 0.00237, 0.00282, 0.00398, 0.0074, 0.01321, 0.02113, 0.03024, 0.03969, 0.04905, 0.05856, 0.06819,
                          0.07723, 0.08636, 0.0957, 0.10508, 0.11467, 0.12374, 0.13281, 0.14205, 0.15191, 0.16195, 0.17195, 0.18114,
                          0.18987, 0.19886, 0.2083, 0.21794, 0.228, 0.23806, 0.24644, 0.25312, 0.25713, 0.25932, 0.25835, 0.25279,
                          0.24266, 0.23367, 0.22357, 0.2143, 0.20344, 0.19319, 0.18363, 0.17294, 0.16265, 0.15232, 0.14053, 0.12759,
                          0.11486, 0.10345, 0.09229, 0.08193, 0.07198, 0.06108, 0.05136, 0.04241, 0.0337, 0.02403, 0.01447, 0.00466}, // ET 9269B
    fMPPCNPixelRows{4,
                    4,
                    8,
                    8,
                    8,
                    8,
                    8,
                    8,
                    8,
                    8},
    fMPPCPixelSizeSet{3_mm,
                      3_mm,
                      3_mm,
                      3_mm,
                      3_mm,
                      3_mm,
                      3_mm,
                      3_mm,
                      3_mm,
                      3_mm},
    fMPPCPitch{0.2_mm},     // gap between pixels
    fMPPCThickness{0.1_mm}, // cathode
    fMPPCCouplerThickness{0.1_mm},
    fMPPCWindowThickness{0.2_mm},
    // 0.1(epoxy)+0.1(cathode),window change  from epoxy to epoxy&silicon Pixels, may change name "window" later
    // S14161
    fMPPCEnergyBin{1.391655126_eV, 1.413303953_eV, 1.436778788_eV, 1.461046623_eV, 1.486148332_eV, 1.512127645_eV, 1.533451437_eV,
                   1.553243676_eV, 1.579239384_eV, 1.601331725_eV, 1.618380329_eV, 1.644070091_eV, 1.668575932_eV, 1.695332333_eV,
                   1.722960822_eV, 1.750124077_eV, 1.769418592_eV, 1.796139112_eV, 1.823679036_eV, 1.850275906_eV, 1.880443107_eV,
                   1.905389876_eV, 1.927096598_eV, 1.948591293_eV, 1.968750142_eV, 1.989552718_eV, 2.012699006_eV, 2.044509747_eV,
                   2.086746867_eV, 2.108593697_eV, 2.13196381_eV, 2.162065168_eV, 2.196197125_eV, 2.211510324_eV, 2.240146328_eV,
                   2.277682676_eV, 2.31087249_eV, 2.342157708_eV, 2.374301646_eV, 2.410389305_eV, 2.452932479_eV, 2.507137915_eV,
                   2.565089699_eV, 2.6485729_eV, 2.732230683_eV, 2.795153262_eV, 2.842415847_eV, 2.907557927_eV, 2.975755876_eV,
                   3.029226473_eV, 3.073774257_eV, 3.139907148_eV, 3.179961838_eV, 3.207163267_eV, 3.244659512_eV, 3.289219352_eV,
                   3.307290247_eV, 3.327878491_eV, 3.363166032_eV, 3.399209944_eV, 3.435444057_eV, 3.46043183_eV, 3.48852431_eV,
                   3.516457699_eV, 3.550580722_eV, 3.563879348_eV, 3.57404697_eV, 3.587160946_eV, 3.60984247_eV, 3.628195256_eV,
                   3.637441808_eV, 3.651400344_eV, 3.670179231_eV, 3.684390646_eV, 3.696659746_eV, 3.720794081_eV, 3.721383683_eV,
                   3.742354188_eV, 3.755013028_eV, 3.787037996_eV, 3.809782448_eV, 3.840536792_eV},
    fMPPCEfficiency{0.038361565, 0.043881036, 0.050158203, 0.056879188, 0.063896051, 0.071245776, 0.077096023, 0.082806497, 0.09049302,
                    0.096846043, 0.102167517, 0.110439587, 0.118523116, 0.127329437, 0.136497387, 0.144697087, 0.152101728, 0.160383488,
                    0.169784037, 0.17901796, 0.189932435, 0.197788503, 0.2061559, 0.215114701, 0.226013151, 0.23524354, 0.244663368,
                    0.255647156, 0.270165176, 0.278993559, 0.28798576, 0.297864503, 0.310058789, 0.315182084, 0.324696839, 0.336285043,
                    0.347467345, 0.356656635, 0.364883088, 0.373827813, 0.382839555, 0.3902882, 0.395495771, 0.400343653, 0.401560403,
                    0.396238242, 0.390316468, 0.384591232, 0.375317514, 0.365819506, 0.358254968, 0.346811362, 0.335599315, 0.326263057,
                    0.314742636, 0.300694748, 0.29059875, 0.283296713, 0.270049269, 0.257841507, 0.247144881, 0.237727757, 0.22854291,
                    0.217013978, 0.206721701, 0.200096265, 0.191250653, 0.181575856, 0.168947005, 0.159534377, 0.149674853, 0.139503115,
                    0.129670093, 0.120230653, 0.108280609, 0.091831406, 0.098424138, 0.083937488, 0.073056832, 0.060399447, 0.047887957,
                    0.034501313},
    fWaveformIntegralTime{100_ns} {}

auto ECAL::ComputeMesh() const -> MeshInformation {
    auto pmpMesh{ECALMesh{fNSubdivision}.Generate()};
    MeshInformation mesh;
    auto& [vertex, faceList, typeMap, clusterMap]{mesh};
    const auto point{pmpMesh.vertex_property<pmp::Point>("v:point")};

    for (auto&& v : pmpMesh.vertices()) {
        vertex.emplace_back(Mustard::VectorCast<CLHEP::Hep3Vector>(point[v]));
    }

    for (auto&& f : pmpMesh.faces()) {
        const auto centroid{Mustard::VectorCast<CLHEP::Hep3Vector>(pmp::centroid(pmpMesh, f))};
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
        if (std::ranges::any_of(pmpMesh.vertices(f),
                                [&](const auto& v) {
                                    const auto rXY{fInnerRadius * muc::hypot(point[v][0], point[v][1])};
                                    if (point[v][2] < 0) {
                                        return rXY < fUpstreamWindowRadius;
                                    } else {
                                        return rXY < fDownstreamWindowRadius;
                                    }
                                })) {
            continue;
        }

        auto& face{faceList.emplace_back()};
        face.centroid = centroid;
        face.normal = Mustard::VectorCast<CLHEP::Hep3Vector>(pmp::face_normal(pmpMesh, f));

        for (auto&& v : pmpMesh.vertices(f)) {
            face.vertexIndex.emplace_back(v.idx());
        }

        const auto LocalPhi{
            [uHat = (vertex[face.vertexIndex.front()] - face.centroid).unit(),
             vHat = face.normal.cross(vertex[face.vertexIndex.front()] - face.centroid).unit(),
             &localOrigin = face.centroid,
             &vertex = vertex](const auto& i) {
                const auto localPoint = vertex[i] - localOrigin;
                return std::atan2(localPoint.dot(vHat), localPoint.dot(uHat));
            }};
        std::ranges::sort(face.vertexIndex,
                          [&LocalPhi](const auto& i, const auto& j) {
                              return LocalPhi(i) < LocalPhi(j);
                          });
    }

    std::map<std::vector<float>, std::vector<int>> edgeLengthSet;

    for (int moduleID{};
         auto&& [centroid, _, vertexIndex] : std::as_const(faceList)) { // module types and clusters sorting
        // sort by edge length
        std::vector<float> edgeLength; // magic conversion (double to float)
        std::vector<G4ThreeVector> xV{vertexIndex.size()};

        std::ranges::transform(vertexIndex, xV.begin(),
                               [&](const auto& i) { return vertex[i]; });

        for (int i{}; i < std::ssize(xV); ++i) {
            edgeLength.emplace_back(i != std::ssize(xV) - 1 ? (xV[i + 1] - xV[i]).mag() :
                                                              (xV[0] - xV[i]).mag());
        };

        std::ranges::sort(edgeLength);
        edgeLengthSet[edgeLength].emplace_back(moduleID);

        // sort by centroid distance to other faces
        std::pair<float, int> clusterInfo;

        auto cmp = [](
                       const std::pair<float, int>& p,
                       const std::pair<float, int>& q) { return p.first > q.first; };

        std::priority_queue<std::pair<float, int>,
                            std::vector<std::pair<float, int>>,
                            decltype(cmp)>
            centroidPriority(cmp);

        for (int i{}; auto&& [adjacentCentroid, _1, _2] : std::as_const(faceList)) {
            if (centroid == adjacentCentroid) {
                i++;
                continue;
            }
            clusterInfo.first = (centroid - adjacentCentroid).mag();
            clusterInfo.second = i;
            centroidPriority.push(clusterInfo);
            i++;
        }

        for (int i{}; i < std::ssize(vertexIndex); ++i) {
            auto top = centroidPriority.top();
            if (top.first > 0.2) {
                continue;
            }
            clusterMap[moduleID].emplace_back(top.second);
            centroidPriority.pop();
        }
        moduleID++;
    }

    if (Mustard::Env::VerboseLevelReach<'V'>()) {
        std::cout << ">>--->>edgeLengthSet" << "\n";
        for (int type{1}; auto&& pair : edgeLengthSet) {
            std::ranges::sort(pair.second);
            std::cout << "--->>type " << type << " : " << "\n";
            std::cout << ">>lengths: " << "\n";
            for (auto&& value : pair.first) {
                std::cout << value << " ";
            }
            std::cout << "\n>>units(" << pair.second.size() << "units total): " << "\n";
            for (auto&& value : pair.second) {
                std::cout << value << " ";
            }
            std::cout << "\n";
            std::cout << "======================================================\n";
            type++;
        }
    }

    int typeID{};
    for (auto&& pair : edgeLengthSet) {
        for (auto&& value : pair.second) {
            typeMap[value] = typeID; // unitID->typeID
        }
        typeID++;
    }

    return mesh;
}

auto ECAL::ComputeTransformToOuterSurfaceWithOffset(int moduleID, double offsetInNormalDirection) const -> HepGeom::Transform3D {
    const auto& faceList{Mesh().fFaceList};
    auto&& [centroid, normal, vertexIndex]{faceList[moduleID]};

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
    ImportValue(node, fUpstreamWindowRadius, "UpstreamWindowRadius");
    ImportValue(node, fDownstreamWindowRadius, "DownstreamWindowRadius");
    ImportValue(node, fScintillationEnergyBin, "ScintillationEnergyBin");
    ImportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ImportValue(node, fScintillationYield, "ScintillationYield");
    ImportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    ImportValue(node, fResolutionScale, "ResolutionScale");
    ImportValue(node, fUseMPPC, "UseMPPC");
    ImportValue(node, fPMTDimensions, "PMTDimensions");
    ImportValue(node, fPMTCouplerThickness, "PMTCouplerThickness");
    ImportValue(node, fPMTWindowThickness, "PMTWindowThickness");
    ImportValue(node, fPMTCathodeThickness, "PMTCathodeThickness");
    ImportValue(node, fPMTEnergyBin, "PMTEnergyBin");
    ImportValue(node, fPMTQuantumEfficiency, "PMTQuantumEfficiency");
    ImportValue(node, fMPPCNPixelRows, "MPPCNPixelRows");
    ImportValue(node, fMPPCPixelSizeSet, "MPPCPixelSizeSet");
    ImportValue(node, fMPPCPitch, "MPPCPitch");
    ImportValue(node, fMPPCThickness, "MPPCThickness");
    ImportValue(node, fMPPCCouplerThickness, "MPPCCouplerThickness");
    ImportValue(node, fMPPCWindowThickness, "MPPCWindowThickness");
    ImportValue(node, fMPPCEnergyBin, "MPPCEnergyBin");
    ImportValue(node, fMPPCEfficiency, "MPPCEfficiency");
    ImportValue(node, fModuleSelection, "ModuleSelection");
    ImportValue(node, fWaveformIntegralTime, "WaveformIntegralTime");

    SetGeometryOutdated();
}

auto ECAL::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, fNSubdivision, "NSubdivision");
    ExportValue(node, fInnerRadius, "InnerRadius");
    ExportValue(node, fCrystalHypotenuse, "CrystalHypotenuse");
    ExportValue(node, fUpstreamWindowRadius, "UpstreamWindowRadius");
    ExportValue(node, fDownstreamWindowRadius, "DownstreamWindowRadius");
    ExportValue(node, fScintillationEnergyBin, "ScintillationEnergyBin");
    ExportValue(node, fScintillationComponent1, "ScintillationComponent1");
    ExportValue(node, fScintillationYield, "ScintillationYield");
    ExportValue(node, fScintillationTimeConstant1, "ScintillationTimeConstant1");
    ExportValue(node, fResolutionScale, "ResolutionScale");
    ExportValue(node, fUseMPPC, "UseMPPC");
    ExportValue(node, fPMTDimensions, "PMTDimensions");
    ExportValue(node, fPMTCouplerThickness, "PMTCouplerThickness");
    ExportValue(node, fPMTWindowThickness, "PMTWindowThickness");
    ExportValue(node, fPMTCathodeThickness, "PMTCathodeThickness");
    ExportValue(node, fPMTEnergyBin, "PMTEnergyBin");
    ExportValue(node, fPMTQuantumEfficiency, "PMTQuantumEfficiency");
    ExportValue(node, fMPPCNPixelRows, "MPPCNPixelRows");
    ExportValue(node, fMPPCPixelSizeSet, "MPPCPixelSizeSet");
    ExportValue(node, fMPPCPitch, "MPPCPitch");
    ExportValue(node, fMPPCThickness, "MPPCThickness");
    ExportValue(node, fMPPCCouplerThickness, "MPPCCouplerThickness");
    ExportValue(node, fMPPCWindowThickness, "MPPCWindowThickness");
    ExportValue(node, fMPPCEnergyBin, "MPPCEnergyBin");
    ExportValue(node, fMPPCEfficiency, "MPPCEfficiency");
    ExportValue(node, fModuleSelection, "ModuleSelection");
    ExportValue(node, fWaveformIntegralTime, "WaveformIntegralTime");
}

} // namespace MACE::Detector::Description
