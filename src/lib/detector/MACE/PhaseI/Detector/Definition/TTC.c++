#include "MACE/PhaseI/Detector/Definition/TTC.h++"
#include "MACE/PhaseI/Detector/Description/TTC.h++"

#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorCast.h++"

#include "G4Box.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"

#include "gsl/gsl"

#include "fmt/format.h"

#include <algorithm>
#include <vector>

namespace MACE::PhaseI::Detector::Definition {

using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;

auto TTC::Construct(G4bool checkOverlaps) -> void {
    const auto& ttc{Description::TTC::Instance()};
    const auto nistManager{G4NistManager::Instance()};
    // Define Element and Material
    const auto& ttcScintillatorMaterial{nistManager->BuildMaterialWithNewDensity("TTCScintillatorMaterial", "G4_POLYSTYRENE", ttc.Density())};

    const auto hydrogenElement{nistManager->FindOrBuildElement("H")};
    const auto carbonElement{nistManager->FindOrBuildElement("C")};
    const auto oxygenElement{nistManager->FindOrBuildElement("O")};
    const auto siliconElement{nistManager->FindOrBuildElement("Si")};

    const auto lightCoupler{new G4Material("TTCLightCouplerMaterial", ttc.LightCouplerDensity(), 4, kStateLiquid)};
    lightCoupler->AddElement(carbonElement, ttc.LightCouplerCarbonElement());
    lightCoupler->AddElement(hydrogenElement, ttc.LightCouplerHydrogenElement());
    lightCoupler->AddElement(oxygenElement, ttc.LightCouplerOxygenElement());
    lightCoupler->AddElement(siliconElement, ttc.LightCouplerSiliconElement());

    const auto window{new G4Material("TTCWindowMaterial", ttc.WindowDensity(), 3, kStateSolid)};
    window->AddElement(carbonElement, ttc.WindowCarbonElement());
    window->AddElement(hydrogenElement, ttc.WindowHydrogenElement());
    window->AddElement(oxygenElement, ttc.WindowOxygenElement());

    // Construct Material Optical Properties Tables
    const auto [minPhotonEnergy, maxPhotonEnergy]{std::ranges::minmax(ttc.ScintillationComponent1EnergyBin())};
    const auto scintillatorPropertiesTable{new G4MaterialPropertiesTable};
    scintillatorPropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, ttc.RIndex());
    scintillatorPropertiesTable->AddProperty("ABSLENGTH", {minPhotonEnergy, maxPhotonEnergy}, ttc.AbsLength());
    scintillatorPropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1", ttc.ScintillationComponent1EnergyBin(), ttc.ScintillationComponent1());
    scintillatorPropertiesTable->AddConstProperty("SCINTILLATIONYIELD", ttc.ScintillationYield());
    scintillatorPropertiesTable->AddConstProperty("SCINTILLATIONRISETIME1", ttc.ScintillationRiseTimeConstant1());
    scintillatorPropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", ttc.ScintillationDecayTimeConstant1());
    scintillatorPropertiesTable->AddConstProperty("RESOLUTIONSCALE", ttc.ResolutionScale());
    ttcScintillatorMaterial->SetMaterialPropertiesTable(scintillatorPropertiesTable);

    const auto lightCouplerPropertiesTable{new G4MaterialPropertiesTable};
    lightCouplerPropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, ttc.LightCouplerRIndex()); // EJ-550
    lightCouplerPropertiesTable->AddProperty("ABSLENGTH", {minPhotonEnergy, maxPhotonEnergy}, ttc.LightCouplerAbsLength());
    lightCoupler->SetMaterialPropertiesTable(lightCouplerPropertiesTable);

    const auto windowPropertiesTable{new G4MaterialPropertiesTable};
    windowPropertiesTable->AddProperty("RINDEX", {minPhotonEnergy, maxPhotonEnergy}, ttc.WindowRIndex());
    window->SetMaterialPropertiesTable(windowPropertiesTable);

    const auto reflectorSurfacePropertiesTable{new G4MaterialPropertiesTable};
    reflectorSurfacePropertiesTable->AddProperty("REFLECTIVITY", {minPhotonEnergy, maxPhotonEnergy}, ttc.ReflectorReflectivity());

    const auto couplerSurfacePropertiesTable{new G4MaterialPropertiesTable};
    couplerSurfacePropertiesTable->AddProperty("TRANSMITTANCE", {minPhotonEnergy, maxPhotonEnergy}, ttc.CouplerTransmittance());

    const auto airPaintSurfacePropertiesTable{new G4MaterialPropertiesTable};
    airPaintSurfacePropertiesTable->AddProperty("REFLECTIVITY", {minPhotonEnergy, maxPhotonEnergy}, ttc.AirPaintReflectivity());

    const auto cathodeSurfacePropertiesTable{new G4MaterialPropertiesTable};
    cathodeSurfacePropertiesTable->AddProperty("REFLECTIVITY", {minPhotonEnergy, maxPhotonEnergy}, ttc.CathodeSurface());
    cathodeSurfacePropertiesTable->AddProperty("EFFICIENCY", ttc.SiPMEnergyBin(), ttc.SiPMEfficiency());

    // Construct Detector
    int tileID{};
    const auto nCircle{ssize(ttc.NAlongPhi())};

    // set up the PCB
    const auto ttcPCBSolid{Make<G4Box>(
        "TTCPCBSolid",
        ttc.PCBWidth() / 2,
        ttc.PCBThickness() / 2,
        ttc.PCBLength() / 2)};
    const auto ttcPCBLogic{Make<G4LogicalVolume>(
        ttcPCBSolid,
        nistManager->FindOrBuildMaterial("G4_POLYCARBONATE"),
        "TTCPCB")};
    // set up the Window
    const auto ttcWindowSolid{Make<G4Box>(
        "TTCWindowSolid",
        ttc.WindowWidth() / 2,
        ttc.WindowThickness() / 2,
        ttc.WindowLength() / 2)};
    const auto ttcWindowLogic{Make<G4LogicalVolume>(
        ttcWindowSolid,
        window,
        "TTCWindow")};
    // set up the Silicon
    const auto ttcSiliconeSolid{Make<G4Box>(
        "TTCSiliconeSolid",
        ttc.SiliconeWidth() / 2,
        ttc.SiliconeThickness() / 2,
        ttc.SiliconeLength() / 2)};
    const auto ttcSiliconeLogic{Make<G4LogicalVolume>(
        ttcSiliconeSolid,
        nistManager->FindOrBuildMaterial("G4_Si"),
        "TTCSilicone")};
    // set up the LightCoupler
    const auto ttcLightCouplerSolid{Make<G4Box>(
        "TTCLightCouplerSolid",
        ttc.LightCouplerWidth() / 2,
        ttc.LightCouplerThickness() / 2,
        ttc.LightCouplerLength() / 2)};
    const auto ttcLightCouplerLogic{Make<G4LogicalVolume>(
        ttcLightCouplerSolid,
        lightCoupler,
        "TTCLightCoupler")};
    // set up the empty air motherbox
    const auto ttcVirtualBoxSolid{Make<G4Box>(
        "TTCVirtualBoxSolid",
        ttc.PCBWidth() / 2,
        ttc.Length() / 2 + ttc.PCBThickness() + ttc.WindowThickness() + ttc.LightCouplerThickness(),
        ttc.Width() / 2)};
    const auto ttcVirtualBoxMaterial{G4Material::GetMaterial("G4_AIR")};
    const auto ttcVirtualBoxLogic{Make<G4LogicalVolume>(
        ttcVirtualBoxSolid,
        ttcVirtualBoxMaterial,
        "TTCVirtualBox")};
    for (gsl::index i{}; i < nCircle; ++i) {
        auto deltaPhi{2 * pi / ttc.NAlongPhi()[i]};
        // set the position of air mother box
        const auto transform{/*G4RotateZ3D{Mustard::Math::IsEven(i) ? 0 : deltaPhi / 2} **/
                             G4Translate3D{G4ThreeVector(ttc.Radius()[i], 0, ttc.ZPosition()[i])} *
                             G4RotateZ3D{ttc.SlantAngle()}};
        for (int j{}; j < ttc.NAlongPhi()[i]; ++j, ++tileID) {
            Make<G4PVPlacement>(
                G4RotateZ3D{j * deltaPhi} * transform,
                ttcVirtualBoxLogic,
                "TTCVirtualBoxPhysics",
                Mother().LogicalVolume(),
                false,
                tileID,
                checkOverlaps);
        }
    }
    // set up the TTC scintillator
    const auto ttcScintillatorSolid{Make<G4Box>(
        "TTCScintillatorSolid",
        ttc.Thickness() / 2,
        ttc.Length() / 2,
        ttc.Width() / 2)};
    const auto ttcScintillatorLogic{Make<G4LogicalVolume>(
        ttcScintillatorSolid,
        ttcScintillatorMaterial,
        "TTCScintillator")};
    // set the position of the TTC scintillator inside the air mother box
    auto ttcScintillatorPhysics{Make<G4PVPlacement>(
        nullptr,
        G4ThreeVector(0, 0, 0),
        ttcScintillatorLogic,
        "TTCScintillatorPhysics",
        ttcVirtualBoxLogic,
        false,
        checkOverlaps)};
    // set the position of the LightCoupler inside the air mother box
    auto ttcLightCouplerUpPhysics{Make<G4PVPlacement>(
        nullptr,
        G4ThreeVector(0, (ttc.Length() + ttc.LightCouplerThickness()) / 2, 0),
        ttcLightCouplerLogic,
        "TTCLightCouplerPhysics",
        ttcVirtualBoxLogic,
        false,
        checkOverlaps)};
    auto ttcLightCouplerDownPhysics{Make<G4PVPlacement>(
        nullptr,
        G4ThreeVector(0, -(ttc.Length() + ttc.LightCouplerThickness()) / 2, 0),
        ttcLightCouplerLogic,
        "TTCLightCouplerPhysics",
        ttcVirtualBoxLogic,
        false,
        checkOverlaps)};
    // set the position of the Window inside the air mother box
    Make<G4PVPlacement>(
        nullptr,
        G4ThreeVector(0, ttc.LightCouplerThickness() + (ttc.Length() + ttc.WindowThickness()) / 2, 0),
        ttcWindowLogic,
        "TTCWindowPhysics",
        ttcVirtualBoxLogic,
        false,
        ttc.NSiPM() - 1, // SiPMLocalID
        checkOverlaps);
    Make<G4PVPlacement>(
        G4Translate3D{G4ThreeVector(0, -(ttc.LightCouplerThickness() + (ttc.Length() + ttc.WindowThickness()) / 2), 0)} * G4RotateZ3D{pi},
        ttcWindowLogic,
        "TTCWindowPhysics",
        ttcVirtualBoxLogic,
        false,
        ttc.NSiPM() - 2, // SiPMLocalID
        checkOverlaps);
    // set the position of the PCB inside the air mother box
    Make<G4PVPlacement>(
        nullptr,
        G4ThreeVector(0, ttc.WindowThickness() + ttc.LightCouplerThickness() + (ttc.Length() + ttc.PCBThickness()) / 2, 0),
        ttcPCBLogic,
        "TTCPCBPhysics",
        ttcVirtualBoxLogic,
        false,
        checkOverlaps);
    Make<G4PVPlacement>(
        nullptr,
        G4ThreeVector(0, -(ttc.WindowThickness() + ttc.LightCouplerThickness() + (ttc.Length() + ttc.PCBThickness()) / 2), 0),
        ttcPCBLogic,
        "TTCPCBPhysics",
        ttcVirtualBoxLogic,
        false,
        checkOverlaps);

    // Construct Optical Surface
    const auto reflectorSurface{new G4OpticalSurface("TTCReflector", unified, polished, dielectric_metal)};
    new G4LogicalSkinSurface{"TTCReflectorSurface", ttcScintillatorLogic, reflectorSurface};
    reflectorSurface->SetMaterialPropertiesTable(reflectorSurfacePropertiesTable);
    const auto airPaintSurface{new G4OpticalSurface("TTCAirPaint", unified, polished, dielectric_metal)};
    new G4LogicalBorderSurface{"TTCAirPaintSurface",
                               PhysicalVolume("TTCVirtualBoxPhysics"),
                               ttcScintillatorPhysics,
                               airPaintSurface};
    airPaintSurface->SetMaterialPropertiesTable(airPaintSurfacePropertiesTable);

    const auto couplerUpSurface{new G4OpticalSurface("TTCCoupler", unified, polished, dielectric_dielectric)};
    new G4LogicalBorderSurface{"TTCCouplerSurface",
                               ttcScintillatorPhysics,
                               ttcLightCouplerUpPhysics,
                               couplerUpSurface};
    couplerUpSurface->SetMaterialPropertiesTable(couplerSurfacePropertiesTable);
    const auto couplerDownSurface{new G4OpticalSurface("TTCCoupler", unified, polished, dielectric_dielectric)};
    new G4LogicalBorderSurface{"TTCCouplerSurface",
                               ttcScintillatorPhysics,
                               ttcLightCouplerDownPhysics,
                               couplerDownSurface};
    couplerDownSurface->SetMaterialPropertiesTable(couplerSurfacePropertiesTable);

    // set the position of the Silicon inside the Window
    Make<G4PVPlacement>(
        nullptr,
        G4ThreeVector(0, (ttc.SiliconeThickness() - ttc.WindowThickness()) / 2, 0),
        ttcSiliconeLogic,
        "TTCSiliconePhysics",
        ttcWindowLogic,
        false,
        checkOverlaps);

    // Construct Silicon Optical Surface
    const auto cathodeSurface{new G4OpticalSurface("TTCCathode", unified, polished, dielectric_metal)};
    new G4LogicalSkinSurface{"TTCCathodeSkinSurface", ttcSiliconeLogic, cathodeSurface};
    cathodeSurface->SetMaterialPropertiesTable(cathodeSurfacePropertiesTable);
}

} // namespace MACE::PhaseI::Detector::Definition
