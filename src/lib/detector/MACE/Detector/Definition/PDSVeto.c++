#include "MACE/Detector/Definition/PDSVeto.h++"
#include "MACE/Detector/Description/ECALField.h++"
#include "MACE/Detector/Description/PDSVeto.h++"

#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Torus.hh"
#include "G4Transform3D.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit;
using namespace Mustard::MathConstant;

auto PDSVeto::Construct(G4bool checkOverlaps) -> void {
    //////////////////////////////////////////////////
    ///////////////////Define Material////////////////
    //////////////////////////////////////////////////
    const auto nistManager{G4NistManager::Instance()};
    const auto hydrogenElement{nistManager->FindOrBuildElement("H")};
    const auto carbonElement{nistManager->FindOrBuildElement("C")};
    const auto oxygenElement{nistManager->FindOrBuildElement("O")};
    const auto siliconElement{nistManager->FindOrBuildElement("Si")};

    const auto silicon = nistManager->FindOrBuildMaterial("G4_Si");

    const auto siliconeGrease{new G4Material("siliconeGrease", 1.06_g_cm3, 4, kStateLiquid)};
    siliconeGrease->AddElement(carbonElement, 2);
    siliconeGrease->AddElement(hydrogenElement, 6);
    siliconeGrease->AddElement(oxygenElement, 1);
    siliconeGrease->AddElement(siliconElement, 1);

    const auto plasticScinllator{nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE")};
    const auto air{nistManager->FindOrBuildMaterial("G4_AIR")};

    const auto epoxy = new G4Material("epoxy", 1.18_g_cm3, 3, kStateSolid);
    epoxy->AddElement(carbonElement, 0.7362);
    epoxy->AddElement(hydrogenElement, 0.0675);
    epoxy->AddElement(oxygenElement, 0.1963);

    // const auto windowPropertiesTable{new G4MaterialPropertiesTable};
    // windowPropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, {1.57, 1.57});
    // epoxy->SetMaterialPropertiesTable(windowPropertiesTable);

    const auto cap1PSaLayerCount{5};
    const auto cap2PSaLayerCount{8};
    const auto topPSaLayerCount{12};
    const auto sidePSaLayerCount{16};

    const auto solenoidWindowRadius{14.0_cm};

    const auto& veto{Description::PDSVeto::Instance()};
    const auto sideModuleThickness{(veto.PSThickness() + veto.AlAbsorberThickness()) * veto.SideLayer() - veto.AlAbsorberThickness()};
    const auto capModuleThickness{(veto.PSThickness() + veto.AlAbsorberThickness()) * veto.CapLayer() - veto.AlAbsorberThickness()}; // both cap1 and cap2
    const auto topModuleThickness{(veto.PSThickness() + veto.AlAbsorberThickness()) * veto.TopLayer() - veto.AlAbsorberThickness()};

    const auto cap1ModuleWidth{(veto.PSWidth() + veto.InterPSGap()) * cap1PSaLayerCount - veto.InterPSGap() + veto.ModuleOffset() * (veto.CapLayer() - 1)}; // both cap1 and cap2
    const auto cap2ModuleWidth{(veto.PSWidth() + veto.InterPSGap()) * cap2PSaLayerCount - veto.InterPSGap() + veto.ModuleOffset() * (veto.CapLayer() - 1)}; // both cap1 and cap2
    const auto topModuleWidth{(veto.PSWidth() + veto.InterPSGap()) * topPSaLayerCount - veto.InterPSGap() + veto.ModuleOffset() * (veto.TopLayer() - 1)};
    const auto sideModuleWidth{(veto.PSWidth() + veto.InterPSGap()) * sidePSaLayerCount - veto.InterPSGap() + veto.ModuleOffset() * (veto.SideLayer() - 1)};

    const auto sideModuleXShift{solenoidWindowRadius + cap1ModuleWidth + sideModuleThickness / 2 + veto.InterModuleGap()};
    const G4ThreeVector side0ModuleCenter{sideModuleXShift, 0, 0};
    const G4ThreeVector side1ModuleCenter{-sideModuleXShift, 0, 0};

    const auto cap2ModuleYShift{solenoidWindowRadius + cap2ModuleWidth / 2};
    const auto cap2ModuleZShift{topModuleWidth / 2 + veto.InterModuleGap() + capModuleThickness + veto.Cap12Gap() + capModuleThickness / 2};
    const G4ThreeVector cap2Module0Center{0, cap2ModuleYShift, cap2ModuleZShift};
    const G4ThreeVector cap2Module1Center{0, -cap2ModuleYShift, cap2ModuleZShift};
    const auto cap2Module2Center{-cap2Module0Center};
    const auto cap2Module3Center{-cap2Module1Center};

    const auto cap1ModuleXShift{solenoidWindowRadius + cap1ModuleWidth / 2};
    const auto cap1ModuleZShift(topModuleWidth / 2 + veto.InterModuleGap() + capModuleThickness / 2);
    const G4ThreeVector cap1Module0Center{cap1ModuleXShift, 0, cap1ModuleZShift};
    const G4ThreeVector cap1Module1Center{-cap1ModuleXShift, 0, cap1ModuleZShift};
    const auto cap1Module2Center{-cap1Module0Center};
    const auto cap1Module3Center{-cap1Module1Center};

    const auto topModuleYShift{sideModuleWidth / 2 + veto.InterModuleGap() + topModuleThickness / 2};
    const G4ThreeVector topModuleCenter{0, topModuleYShift, 0};

    const auto topModuleRotation{G4RotateY3D(pi / 2).getRotation()};

    const auto side0ModuleRotation{G4RotateZ3D(pi / 2).getRotation()};
    const auto side1ModuleRotation{G4RotateZ3D(-pi / 2).getRotation()};

    const auto cap1ModuleRotation{G4RotateX3D(pi / 2).getRotation()};
    const auto cap2ModuleRotation{(G4RotateZ3D(pi / 2) * G4RotateX3D(pi / 2)).getRotation()};

    const auto& ecalField{Description::ECALField::Instance()};
    const auto x0{Mustard::VectorCast<G4ThreeVector>(ecalField.Center())};

    const G4Transform3D topModuleTransform{topModuleRotation, topModuleCenter + x0};

    const G4Transform3D side0ModuleTransform{side0ModuleRotation, side0ModuleCenter + x0};
    const G4Transform3D side1ModuleTransform{side1ModuleRotation, side1ModuleCenter + x0};

    const G4Transform3D cap1Module0Transform{cap1ModuleRotation, cap1Module0Center + x0};
    const G4Transform3D cap1Module1Transform{cap1ModuleRotation, cap1Module1Center + x0};
    const G4Transform3D cap1Module2Transform{cap1ModuleRotation, cap1Module2Center + x0};
    const G4Transform3D cap1Module3Transform{cap1ModuleRotation, cap1Module3Center + x0};

    const G4Transform3D cap2Module0Transform{cap2ModuleRotation, cap2Module0Center + x0};
    const G4Transform3D cap2Module1Transform{cap2ModuleRotation, cap2Module1Center + x0};
    const G4Transform3D cap2Module2Transform{cap2ModuleRotation, cap2Module2Center + x0};
    const G4Transform3D cap2Module3Transform{cap2ModuleRotation, cap2Module3Center + x0};

    const auto solidTopModule{Make<G4Box>("temp",
                                          topModuleWidth / 2,
                                          topModuleThickness / 2,
                                          (veto.TopPSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};
    const auto solidSideModule{Make<G4Box>("temp",
                                           sideModuleWidth / 2,
                                           sideModuleThickness / 2,
                                           (veto.SidePSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};
    const auto solidCap1Module{Make<G4Box>("temp",
                                           cap1ModuleWidth / 2,
                                           capModuleThickness / 2,
                                           (veto.Cap1PSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};
    const auto solidCap2Module{Make<G4Box>("temp",
                                           cap2ModuleWidth / 2,
                                           capModuleThickness / 2,
                                           (veto.Cap2PSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};

    const auto logicTopModule{Make<G4LogicalVolume>(solidTopModule,
                                                    air,
                                                    "topModuleBox")};
    const auto logicSideModule{Make<G4LogicalVolume>(solidSideModule,
                                                     air,
                                                     "sideModuleBox")};
    const auto logicCap1Module{Make<G4LogicalVolume>(solidCap1Module,
                                                     air,
                                                     "cap1ModuleBox")};
    const auto logicCap2Module{Make<G4LogicalVolume>(solidCap2Module,
                                                     air,
                                                     "cap2ModuleBox")};

    Make<G4PVPlacement>(topModuleTransform, logicTopModule, "topModuleBox", Mother().LogicalVolume(), false, 0, checkOverlaps);

    Make<G4PVPlacement>(side0ModuleTransform, logicSideModule, "side0ModuleBox", Mother().LogicalVolume(), false, 0, checkOverlaps);
    Make<G4PVPlacement>(side1ModuleTransform, logicSideModule, "side1ModuleBox", Mother().LogicalVolume(), false, 1, checkOverlaps);

    Make<G4PVPlacement>(cap1Module0Transform, logicCap1Module, "cap1Module0Box", Mother().LogicalVolume(), false, 0, checkOverlaps);
    Make<G4PVPlacement>(cap1Module1Transform, logicCap1Module, "cap1Module1Box", Mother().LogicalVolume(), false, 1, checkOverlaps);
    Make<G4PVPlacement>(cap1Module2Transform, logicCap1Module, "cap1Module2Box", Mother().LogicalVolume(), false, 2, checkOverlaps);
    Make<G4PVPlacement>(cap1Module3Transform, logicCap1Module, "cap1Module3Box", Mother().LogicalVolume(), false, 3, checkOverlaps);

    Make<G4PVPlacement>(cap2Module0Transform, logicCap2Module, "cap2Module0Box", Mother().LogicalVolume(), false, 0, checkOverlaps);
    Make<G4PVPlacement>(cap2Module1Transform, logicCap2Module, "cap2Module1Box", Mother().LogicalVolume(), false, 1, checkOverlaps);
    Make<G4PVPlacement>(cap2Module2Transform, logicCap2Module, "cap2Module2Box", Mother().LogicalVolume(), false, 2, checkOverlaps);
    Make<G4PVPlacement>(cap2Module3Transform, logicCap2Module, "cap2Module3Box", Mother().LogicalVolume(), false, 3, checkOverlaps);
    // ModuleBox are virtual, do not exist actually

    const auto AssembleStrip{[&](G4String name,
                                 int totalLayer,
                                 int totalStripALayer,
                                 double moduleThickness,
                                 double moduleWidth,
                                 double offset,
                                 G4LogicalVolume* logicStripBox,
                                 G4LogicalVolume* logicModule) -> void {
        // clang-format off
        const int fiberNum{4}; // must be even
        const auto stripBoxHalfLength{dynamic_cast<const G4Box*>(logicStripBox->GetSolid())->GetZHalfLength()};
        const auto psStripLength{(stripBoxHalfLength - (veto.SiPMThickness() + veto.SiPMCouplerThickness())) * 2};
        const auto fiberPairSpacing { (veto.PSWidth()) / 2 }; // ignore radius
        const auto straightFiberLength { psStripLength
                                        - 4 * sqrt(
                                          pow(veto.PSFiberCurvatureRadius(), 2) - pow(veto.PSFiberCurvatureRadius() -   fiberPairSpacing / 4, 2)
                                          ) };
        const auto fiberSpacing { veto.PSWidth() / (fiberNum) }; // ignore radius
        const auto fiberPlaneTiltAngle { asin(veto.PSHoleRadius() * 2 / fiberSpacing) };
        const auto pairBoxWidth { fiberPairSpacing + veto.PSHoleRadius() * 4 };
        const auto fiberCurvaturePhi { acos(
                                            (veto.PSFiberCurvatureRadius() - fiberPairSpacing / 4)
                                            / veto.PSFiberCurvatureRadius()) };
        // clang-format on

        // material wls
        // TODO
        const auto wls{nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE")};

        // pre define solids and logical volumes
        const auto solidStrip{Make<G4Box>("temp",
                                          veto.PSWidth() / 2,
                                          veto.PSThickness() / 2,
                                          psStripLength / 2)};
        const auto logicStrip{Make<G4LogicalVolume>(solidStrip, plasticScinllator, name + "Strip")};

        const auto solidStraightFiberHole{Make<G4Tubs>("temp", 0, veto.PSHoleRadius(), straightFiberLength / 2, 0, 2 * M_PI)};
        const auto logicStraightFiberHole{Make<G4LogicalVolume>(solidStraightFiberHole, air, "StraightFiberHole")};

        const auto solidCurvedFiberHole{Make<G4Torus>("temp", 0, veto.PSHoleRadius(), veto.PSFiberCurvatureRadius(), 0, fiberCurvaturePhi)};
        const auto logicCurvedFiberHole{Make<G4LogicalVolume>(solidCurvedFiberHole, air, "CurvedFiberHole")};

        const auto solidStraightFiber{Make<G4Tubs>("temp", 0, veto.PSFiberRadius(), straightFiberLength / 2, 0, 2 * M_PI)};
        const auto logicStraightFiber{Make<G4LogicalVolume>(solidStraightFiber, wls, "StraightFiber")};

        const auto solidCurvedFiber{Make<G4Torus>("temp", 0, veto.PSFiberRadius(), veto.PSFiberCurvatureRadius(), 0, fiberCurvaturePhi)};
        const auto logicCurvedFiber{Make<G4LogicalVolume>(solidCurvedFiber, wls, "CurvedFiber")};

        const auto solidSiPMCoupuler{Make<G4Box>("temp",
                                                 veto.SiPMSize() / 2,
                                                 veto.SiPMCouplerThickness() / 2,
                                                 veto.SiPMSize() / 2)};
        const auto solidSiPM{Make<G4Box>("temp",
                                         veto.SiPMSize() / 2,
                                         veto.SiPMThickness() / 2,
                                         veto.SiPMSize() / 2)};
        const auto solidSiPMCathode{Make<G4Box>("temp",
                                                veto.SiPMSize() / 2,
                                                veto.SiPMCathodeThickness() / 2,
                                                veto.SiPMSize() / 2)};
        const auto logicSiPMCoupler{Make<G4LogicalVolume>(solidSiPMCoupuler, siliconeGrease, "SiPMCoupler")};
        const auto logicSiPM{Make<G4LogicalVolume>(solidSiPM, epoxy, "SiPM")};
        const auto logicCathode{Make<G4LogicalVolume>(solidSiPMCathode, silicon, "SiPMCathode")};

        // construct stripBoxes (virtual do not exist actually)
        for (int layerIndex{}; // start from 0
             layerIndex < totalLayer;
             layerIndex++) {
            // clang-format off
            auto stripPosY{
                           -(moduleThickness / 2 - veto.PSThickness() / 2)
                           + (layerIndex) * (veto.PSThickness() + veto.AlAbsorberThickness())
                          };
            // clang-format on
            for (int widthIndex{}; // start from 0
                 widthIndex < totalStripALayer;
                 widthIndex++) {
                // clang-format off
                auto stripPosX{
                               -(moduleWidth / 2 - veto.PSWidth() / 2)
                               + (widthIndex) * (veto.PSWidth() + veto.InterPSGap())
                               + (layerIndex) * offset
                              };
                // clang-format on
                const auto stripTransform{G4Transform3D(G4RotationMatrix::IDENTITY, G4ThreeVector(stripPosX, stripPosY, 0))};
                const auto copyNum{layerIndex * totalStripALayer + widthIndex};
                Make<G4PVPlacement>(stripTransform, logicStripBox, name + "StripBox", logicModule, false, copyNum, checkOverlaps);
            }
        }
        // strips
        Make<G4PVPlacement>(G4Transform3D::Identity, logicStrip, name + "Strip", logicStripBox, false, 0, checkOverlaps);

        ///////////////////////////////////////////////////////////////////////////////////////
        // fiber(hole)s
        //  construct shape

        // clang-format off
        auto solidVirtualPairBox { Make <G4Box>("temp",
                                             pairBoxWidth / 2,
                                             veto.PSHoleRadius(),
                                             psStripLength / 2
                                             ) };
        auto logicVirtualPairBox { Make <G4LogicalVolume>(solidVirtualPairBox, plasticScinllator, "virtualPairBox") };
        // clang-format on

        // construct pair box, SiPM, coupler

        for (int fiberPairCount{}; fiberPairCount < fiberNum / 2; ++fiberPairCount) { // loop by fiber pair
            // m pairs form NO.1 to NO.n(2m) ,first pair: 1, m+1
            const auto pairBoxCenterX{-veto.PSWidth() / 2 + ((fiberNum + 2.) / 4. + fiberPairCount) * fiberSpacing};
            const G4ThreeVector pairBoxTranslate{pairBoxCenterX, 0, 0};
            const G4RotateZ3D pairBoxRotation{fiberPlaneTiltAngle};
            const G4Transform3D pairBoxTransform{pairBoxRotation.getRotation(), pairBoxTranslate};
            Make<G4PVPlacement>(pairBoxTransform, logicVirtualPairBox, "virtualPairBox", logicStrip, false, fiberPairCount, checkOverlaps);

            for (int sideCount{}; sideCount < 2; ++sideCount) {                         // nest loop by one pairs' 2 sides
                const auto readBoxRotation{G4RotateX3D(pow(-1., sideCount) * pi / 2.)}; // SiPM&coupler

                const auto SiPMPosZ{(stripBoxHalfLength - veto.SiPMThickness() / 2)};
                const auto couplerPosZ{stripBoxHalfLength - veto.SiPMThickness() - veto.SiPMCouplerThickness() / 2};

                G4Transform3D couplerTransform{readBoxRotation.getRotation(), G4ThreeVector(pairBoxCenterX, 0, couplerPosZ * pow(-1., sideCount))};
                G4Transform3D SiPMTransform{readBoxRotation.getRotation(), G4ThreeVector(pairBoxCenterX, 0, SiPMPosZ * pow(-1., sideCount))};

                Make<G4PVPlacement>(couplerTransform, logicSiPMCoupler, name + "SiPMCoupler", logicStripBox, false, sideCount, checkOverlaps);
                Make<G4PVPlacement>(SiPMTransform, logicSiPM, name + "SiPM", logicStripBox, false, sideCount, checkOverlaps);
            }
        }
        // fiber positions, most unsafe?
        const auto cathodeRelativePosY{veto.SiPMThickness() / 2 - veto.SiPMCathodeThickness() / 2};
        G4Transform3D cathodeRelativeTransform{G4RotationMatrix::IDENTITY, G4ThreeVector(0, cathodeRelativePosY, 0)};
        Make<G4PVPlacement>(cathodeRelativeTransform, logicCathode, name + "SiPMCathode", logicSiPM, false, 0, checkOverlaps);

        const auto upStraightFiberCenterX{pairBoxWidth / 2 - veto.PSHoleRadius()};
        const auto downStraightFiberCenterX{-pairBoxWidth / 2 + veto.PSHoleRadius()};

        const auto downStraightFiberTranslate{G4ThreeVector(downStraightFiberCenterX, 0, 0)};
        const auto upStraightFiberTranslate{G4ThreeVector(upStraightFiberCenterX, 0, 0)};

        const auto downStraightFiberRotation{G4RotateY3D(0).getRotation()};
        const auto upStraightFiberRotation{G4RotateY3D(0).getRotation()};

        const auto downStraightFiberTransform{G4Transform3D(downStraightFiberRotation, downStraightFiberTranslate)};
        const auto upStraightFiberTransform{G4Transform3D(upStraightFiberRotation, upStraightFiberTranslate)};
        // straight FiberHoles
        Make<G4PVPlacement>(upStraightFiberTransform,
                            logicStraightFiberHole,
                            "upStraightFiberHole",
                            logicVirtualPairBox,
                            false,
                            0,
                            checkOverlaps);
        Make<G4PVPlacement>(downStraightFiberTransform,
                            logicStraightFiberHole,
                            "downStraightFiberHole",
                            logicVirtualPairBox,
                            false,
                            0,
                            checkOverlaps);
        // curved fiber Holes
        for (int sideCount{}; sideCount < 2; ++sideCount) { // loop by strip side, left first, 4 curved fibers constructed in one loop
            auto upCurvedFiber1CenterZ{straightFiberLength / 2 * pow(-1, sideCount)};
            auto upCurvedFiber1CenterX{upStraightFiberCenterX - veto.PSFiberCurvatureRadius()};
            auto upCurvedFiber2CenterZ{psStripLength / 2 * pow(-1, sideCount)};
            auto upCurvedFiber2CenterX{upCurvedFiber1CenterX + 2 * (veto.PSFiberCurvatureRadius() - fiberPairSpacing / 4)};
            auto downCurvedFiber1CenterZ{upCurvedFiber1CenterZ};
            auto downCurvedFiber1CenterX{downStraightFiberCenterX + veto.PSFiberCurvatureRadius()};
            auto downCurvedFiber2CenterZ{upCurvedFiber2CenterZ};
            auto downCurvedFiber2CenterX{downCurvedFiber1CenterX - 2 * (veto.PSFiberCurvatureRadius() - fiberPairSpacing / 4)};

            auto upCurvedFiber1Rotation{(G4RotateX3D(M_PI / 2 * pow(-1, sideCount))).getRotation()};
            auto upCurvedFiber2Rotation{(G4RotateX3D(M_PI / 2 * pow(-1, sideCount)) * G4RotateZ3D(M_PI)).getRotation()};
            auto downCurvedFiber1Rotation{((G4RotateX3D(M_PI / 2 * pow(-1, sideCount))) * G4RotateZ3D(M_PI - fiberCurvaturePhi)).getRotation()};
            auto downCurvedFiber2Rotation{((G4RotateX3D(M_PI / 2 * pow(-1, sideCount))) * G4RotateZ3D(2 * M_PI - fiberCurvaturePhi)).getRotation()};

            auto upCurvedFiber1Translate{G4ThreeVector(upCurvedFiber1CenterX, 0, upCurvedFiber1CenterZ)};
            auto upCurvedFiber2Translate{G4ThreeVector(upCurvedFiber2CenterX, 0, upCurvedFiber2CenterZ)};
            auto downCurvedFiber1Translate{G4ThreeVector(downCurvedFiber1CenterX, 0, downCurvedFiber1CenterZ)};
            auto downCurvedFiber2Translate{G4ThreeVector(downCurvedFiber2CenterX, 0, downCurvedFiber2CenterZ)};

            auto upCurvedFiber1Transform{G4Transform3D(upCurvedFiber1Rotation, upCurvedFiber1Translate)};
            auto upCurvedFiber2Transform{G4Transform3D(upCurvedFiber2Rotation, upCurvedFiber2Translate)};
            auto downCurvedFiber1Transform{G4Transform3D(downCurvedFiber1Rotation, downCurvedFiber1Translate)};
            auto downCurvedFiber2Transform{G4Transform3D(downCurvedFiber2Rotation, downCurvedFiber2Translate)};

            Make<G4PVPlacement>(upCurvedFiber1Transform, logicCurvedFiberHole, "UpCurveHole1", logicVirtualPairBox, false, sideCount, checkOverlaps);
            Make<G4PVPlacement>(upCurvedFiber2Transform, logicCurvedFiberHole, "UpCurveHole2", logicVirtualPairBox, false, sideCount, checkOverlaps);
            Make<G4PVPlacement>(downCurvedFiber1Transform, logicCurvedFiberHole, "DownCurveHole1", logicVirtualPairBox, false, sideCount, checkOverlaps);
            Make<G4PVPlacement>(downCurvedFiber2Transform, logicCurvedFiberHole, "DownCurveHole2", logicVirtualPairBox, false, sideCount, checkOverlaps);
        }
        Make<G4PVPlacement>(G4Transform3D::Identity, logicStraightFiber, "StraightFiber", logicStraightFiberHole, false, 0, checkOverlaps);
        Make<G4PVPlacement>(G4Transform3D::Identity, logicCurvedFiber, "CurvedFiber", logicCurvedFiberHole, false, 0, checkOverlaps);
    }};

    const auto solidTopStripBox{Make<G4Box>("temp",
                                            veto.PSWidth() / 2,
                                            veto.PSThickness() / 2,
                                            (veto.TopPSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};
    const auto logicTopStripBox{Make<G4LogicalVolume>(solidTopStripBox,
                                                      air,
                                                      "TopStripBox")};
    AssembleStrip("Top",
                  veto.TopLayer(),
                  topPSaLayerCount,
                  topModuleThickness,
                  topModuleWidth,
                  veto.ModuleOffset(),
                  logicTopStripBox,
                  logicTopModule);

    const auto solidSideStripBox{Make<G4Box>("temp",
                                             veto.PSWidth() / 2,
                                             veto.PSThickness() / 2,
                                             (veto.SidePSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};
    const auto logicSideStripBox{Make<G4LogicalVolume>(solidSideStripBox,
                                                       air,
                                                       "SideStripBox")};
    AssembleStrip("Side",
                  veto.SideLayer(),
                  sidePSaLayerCount,
                  sideModuleThickness,
                  sideModuleWidth,
                  veto.ModuleOffset(),
                  logicSideStripBox,
                  logicSideModule);

    const auto solidCap1StripBox{Make<G4Box>("temp",
                                             veto.PSWidth() / 2,
                                             veto.PSThickness() / 2,
                                             (veto.Cap1PSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};
    const auto logicCap1StripBox{Make<G4LogicalVolume>(solidCap1StripBox,
                                                       air,
                                                       "Cap1StripBox")};
    AssembleStrip("Cap1",
                  veto.CapLayer(),
                  cap1PSaLayerCount,
                  capModuleThickness,
                  cap1ModuleWidth,
                  veto.ModuleOffset(),
                  logicCap1StripBox,
                  logicCap1Module);

    const auto solidCap2StripBox{Make<G4Box>("temp",
                                             veto.PSWidth() / 2,
                                             veto.PSThickness() / 2,
                                             (veto.Cap2PSLength() + veto.SiPMThickness() + veto.SiPMCouplerThickness()) / 2)};
    const auto logicCap2StripBox{Make<G4LogicalVolume>(solidCap2StripBox,
                                                       air,
                                                       "Cap2StripBox")};
    AssembleStrip("Cap2",
                  veto.CapLayer(),
                  cap2PSaLayerCount,
                  capModuleThickness,
                  cap2ModuleWidth,
                  veto.ModuleOffset(),
                  logicCap2StripBox,
                  logicCap2Module);
}
}; // namespace MACE::Detector::Definition