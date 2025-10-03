#include "MACE/Detector/Definition/Target.h++"
#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::Density;
using namespace Mustard::LiteralUnit::MathConstantSuffix;

auto Target::Construct(G4bool checkOverlaps) -> void {
    const auto& target{Description::Target::Instance()};
    const auto& accelerator{Description::Accelerator::Instance()};

    const auto nist{G4NistManager::Instance()};
    auto silicaAerogel{nist->FindMaterial(target.MaterialName())};
    if (silicaAerogel == nullptr) {
        silicaAerogel = new G4Material{target.MaterialName(), target.SilicaAerogelDensity(), 3, kStateSolid, target.EffectiveTemperature()};
        silicaAerogel->AddMaterial(nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"), 0.625);
        silicaAerogel->AddMaterial(nist->FindOrBuildMaterial("G4_WATER"), 0.374);
        silicaAerogel->AddElement(nist->FindOrBuildElement("C"), 0.001);

        const auto mpt{new G4MaterialPropertiesTable};
        mpt->AddConstProperty("MUONIUM_MFP", target.MeanFreePath(), true);
        mpt->AddConstProperty("MUONIUM_FORM_PROB", target.FormationProbability(), true);
        silicaAerogel->SetMaterialPropertiesTable(mpt);
    }

    switch (const auto z0{(accelerator.UpstreamFieldLength() - accelerator.DownstreamFieldLength()) / 2};
            target.ShapeType()) {
    case Description::Target::TargetShapeType::Cuboid: {
        const auto& cuboid{target.Cuboid()};
        const auto solid{Make<G4Box>(
            target.Name(),
            cuboid.Width() / 2,
            cuboid.Height() / 2,
            cuboid.Thickness() / 2)};
        const auto logic{Make<G4LogicalVolume>(
            solid,
            silicaAerogel,
            target.Name())};
        Make<G4PVPlacement>(
            G4TranslateZ3D{z0} *
                G4RotateY3D{-cuboid.TiltAngle()} *
                G4TranslateZ3D{-cuboid.Thickness() / 2},
            logic,
            target.Name(),
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);
        return;
    }
    case Description::Target::TargetShapeType::MultiLayer: {
        const auto& multiLayer{target.MultiLayer()};
        const auto solid{Make<G4Box>(
            target.Name(),
            multiLayer.Thickness() / 2,
            multiLayer.Height() / 2,
            multiLayer.Width() / 2)};
        const auto logic{Make<G4LogicalVolume>(
            solid,
            silicaAerogel,
            target.Name())};
        const auto r{multiLayer.Spacing() + multiLayer.Thickness()};
        for (int k{}; k < multiLayer.Count(); ++k) {
            Make<G4PVPlacement>( // clang-format off
                G4Transform3D{{}, {k * r - r * (multiLayer.Count() - 1) / 2, 0, z0}}, // clang-format on
                logic,
                target.Name(),
                Mother().LogicalVolume(),
                false,
                k,
                checkOverlaps);
        }
        return;
    }
    case Description::Target::TargetShapeType::Cylinder: {
        const auto& cylinder{target.Cylinder()};
        const auto solid{Make<G4Tubs>(
            target.Name(),
            0,
            cylinder.Radius(),
            cylinder.Thickness() / 2,
            0,
            2_pi)};
        const auto logic{Make<G4LogicalVolume>(
            solid,
            silicaAerogel,
            target.Name())};
        Make<G4PVPlacement>( // clang-format off
            G4Transform3D{{}, {0, 0, z0}}, // clang-format on
            logic,
            target.Name(),
            Mother().LogicalVolume(),
            false,
            0,
            checkOverlaps);
        return;
    }
    }
}

} // namespace MACE::Detector::Definition
