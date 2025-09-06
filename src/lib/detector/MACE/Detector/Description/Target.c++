#include "MACE/Detector/Description/Target.h++"

#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "CLHEP/Vector/Rotation.h"

#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4NistManager.hh"

#include "muc/utility"

#include <string>

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;
using namespace Mustard::LiteralUnit::Density;
using namespace Mustard::LiteralUnit::Temperature;

Target::Target() :
    DescriptionBase{"Target"},
    fShapeType{TargetShapeType::MultiLayer},
    fCuboid{},
    fMultiLayer{},
    fCylinder{},
    fSilicaAerogelDensity{27_mg_cm3},
    fEffectiveTemperature{400_K},
    fFormationProbability{0.655},
    fMeanFreePath{200_nm} {}

auto Target::Material() const -> G4Material* {
    constexpr auto materialName{"SilicaAerogel"};
    const auto nist{G4NistManager::Instance()};

    auto silicaAerogel{nist->FindMaterial(materialName)};
    if (silicaAerogel) {
        return silicaAerogel;
    }

    silicaAerogel = new G4Material{materialName, fSilicaAerogelDensity, 3, kStateSolid, fEffectiveTemperature};
    silicaAerogel->AddMaterial(nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"), 0.625);
    silicaAerogel->AddMaterial(nist->FindOrBuildMaterial("G4_WATER"), 0.374);
    silicaAerogel->AddElement(nist->FindOrBuildElement("C"), 0.001);

    const auto mpt{new G4MaterialPropertiesTable};
    mpt->AddConstProperty("MUONIUM_MFP", fMeanFreePath, true);
    mpt->AddConstProperty("MUONIUM_FORM_PROB", fFormationProbability, true);
    silicaAerogel->SetMaterialPropertiesTable(mpt);

    return silicaAerogel;
}

auto Target::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue<std::string>(
        node, [this](auto&& shape) {
            if (shape == "Cuboid") {
                fShapeType = TargetShapeType::Cuboid;
            } else if (shape == "MultiLayer") {
                fShapeType = TargetShapeType::MultiLayer;
            } else if (shape == "Cylinder") {
                fShapeType = TargetShapeType::Cylinder;
            } else {
                Mustard::PrintError(fmt::format("Unknown target shape '{}', skipping", shape));
            }
        },
        "ShapeType");
    {
        ImportValue<double>(
            node, [this](auto value) { fCuboid.Width(value); },
            "Cuboid", "Width");
        ImportValue<double>(
            node, [this](auto value) { fCuboid.Thickness(value); },
            "Cuboid", "Thickness");
        ImportValue<std::string>(
            node, [this](auto&& detail) {
                if (detail == "Flat") {
                    fCuboid.DetailType(CuboidTarget::ShapeDetailType::Flat);
                } else if (detail == "Perforated") {
                    fCuboid.DetailType(CuboidTarget::ShapeDetailType::Perforated);
                } else {
                    Mustard::PrintError(fmt::format("Unknown cuboid target detail '{}', skipping", detail));
                }
            },
            "Cuboid", "DetailType");
        {
            ImportValue<double>(
                node, [this](auto value) { fCuboid.Perforated().Extent(value); },
                "Cuboid", "Perforated", "Extent");
            ImportValue<double>(
                node, [this](auto value) { fCuboid.Perforated().Spacing(value); },
                "Cuboid", "Perforated", "Spacing");
            ImportValue<double>(
                node, [this](auto value) { fCuboid.Perforated().Diameter(value); },
                "Cuboid", "Perforated", "Diameter");
            ImportValue<double>(
                node, [this](auto value) { fCuboid.Perforated().Depth(value); },
                "Cuboid", "Perforated", "Depth");
        }
    }
    {
        ImportValue<double>(
            node, [this](auto value) { fMultiLayer.Width(value); },
            "MultiLayer", "Width");
        ImportValue<double>(
            node, [this](auto value) { fMultiLayer.Height(value); },
            "MultiLayer", "Height");
        ImportValue<double>(
            node, [this](auto value) { fMultiLayer.Thickness(value); },
            "MultiLayer", "Thickness");
        ImportValue<double>(
            node, [this](auto value) { fMultiLayer.Spacing(value); },
            "MultiLayer", "Spacing");
        ImportValue<int>(
            node, [this](auto value) { fMultiLayer.Count(value); },
            "MultiLayer", "Count");
        ImportValue<std::string>(
            node, [this](auto&& detail) {
                if (detail == "Flat") {
                    fMultiLayer.DetailType(MultiLayerTarget::ShapeDetailType::Flat);
                } else if (detail == "Perforated") {
                    fMultiLayer.DetailType(MultiLayerTarget::ShapeDetailType::Perforated);
                } else {
                    Mustard::PrintError(fmt::format("Unknown MultiLayer target detail '{}', skipping", detail));
                }
            },
            "MultiLayer", "DetailType");
        {
            ImportValue<double>(
                node, [this](auto value) { fMultiLayer.Perforated().ExtentZ(value); },
                "MultiLayer", "Perforated", "ExtentZ");
            ImportValue<double>(
                node, [this](auto value) { fMultiLayer.Perforated().ExtentY(value); },
                "MultiLayer", "Perforated", "ExtentY");
            ImportValue<double>(
                node, [this](auto value) { fMultiLayer.Perforated().Spacing(value); },
                "MultiLayer", "Perforated", "Spacing");
            ImportValue<double>(
                node, [this](auto value) { fMultiLayer.Perforated().Diameter(value); },
                "MultiLayer", "Perforated", "Diameter");
        }
    }
    {
        ImportValue<double>(
            node, [this](auto value) { fCylinder.Radius(value); },
            "Cylinder", "Radius");
        ImportValue<double>(
            node, [this](auto value) { fCylinder.Thickness(value); },
            "Cylinder", "Thickness");
    }
    ImportValue(node, fSilicaAerogelDensity, "SilicaAerogelDensity");
    ImportValue(node, fEffectiveTemperature, "EffectiveTemperature");
    ImportValue(node, fFormationProbability, "FormationProbability");
    ImportValue(node, fMeanFreePath, "MeanFreePath");
}

auto Target::ExportAllValue(YAML::Node& node) const -> void {
    using namespace std::string_literals;
    ExportValue(
        node, [this] {
            switch (fShapeType) {
            case TargetShapeType::Cuboid:
                return "Cuboid"s;
            case TargetShapeType::MultiLayer:
                return "MultiLayer"s;
            case TargetShapeType::Cylinder:
                return "Cylinder"s;
            }
            muc::unreachable();
        }(),
        "ShapeType");
    {
        ExportValue(node, fCuboid.Width(), "Cuboid", "Width");
        ExportValue(node, fCuboid.Thickness(), "Cuboid", "Thickness");
        ExportValue(
            node, [this] {
                switch (fCuboid.DetailType()) {
                case CuboidTarget::ShapeDetailType::Flat:
                    return "Flat"s;
                case CuboidTarget::ShapeDetailType::Perforated:
                    return "Perforated"s;
                }
                muc::unreachable();
            }(),
            "Cuboid", "DetailType");
        {
            ExportValue(node, fCuboid.Perforated().Extent(), "Cuboid", "Perforated", "Extent");
            ExportValue(node, fCuboid.Perforated().Spacing(), "Cuboid", "Perforated", "Spacing");
            ExportValue(node, fCuboid.Perforated().Diameter(), "Cuboid", "Perforated", "Diameter");
            ExportValue(node, fCuboid.Perforated().Depth(), "Cuboid", "Perforated", "Depth");
        }
    }
    {
        ExportValue(node, fMultiLayer.Width(), "MultiLayer", "Width");
        ExportValue(node, fMultiLayer.Height(), "MultiLayer", "Height");
        ExportValue(node, fMultiLayer.Thickness(), "MultiLayer", "Thickness");
        ExportValue(node, fMultiLayer.Spacing(), "MultiLayer", "Spacing");
        ExportValue(node, fMultiLayer.Count(), "MultiLayer", "Count");
        ExportValue(
            node, [this] {
                switch (fMultiLayer.DetailType()) {
                case MultiLayerTarget::ShapeDetailType::Flat:
                    return "Flat"s;
                case MultiLayerTarget::ShapeDetailType::Perforated:
                    return "Perforated"s;
                }
                muc::unreachable();
            }(),
            "MultiLayer", "DetailType");
        {
            ExportValue(node, fMultiLayer.Perforated().ExtentZ(), "MultiLayer", "Perforated", "ExtentZ");
            ExportValue(node, fMultiLayer.Perforated().ExtentY(), "MultiLayer", "Perforated", "ExtentY");
            ExportValue(node, fMultiLayer.Perforated().Spacing(), "MultiLayer", "Perforated", "Spacing");
            ExportValue(node, fMultiLayer.Perforated().Diameter(), "MultiLayer", "Perforated", "Diameter");
        }
    }
    {
        ExportValue(node, fCylinder.Radius(), "Cylinder", "Radius");
        ExportValue(node, fCylinder.Thickness(), "Cylinder", "Thickness");
    }
    ExportValue(node, fSilicaAerogelDensity, "SilicaAerogelDensity");
    ExportValue(node, fEffectiveTemperature, "EffectiveTemperature");
    ExportValue(node, fFormationProbability, "FormationProbability");
    ExportValue(node, fMeanFreePath, "MeanFreePath");
}

Target::CuboidTarget::CuboidTarget() :
    ShapeBase{this},
    fWidth{6_cm},
    fThickness{1_cm},
    fDetailType{ShapeDetailType::Perforated},
    fPerforated{} {}

Target::CuboidTarget::PerforatedCuboid::PerforatedCuboid() :
    DetailBase{this},
    fHalfExtent{4_cm / 2},
    fSpacing{55_um},
    fRadius{184_um / 2},
    fDepth{2_mm} {}

Target::MultiLayerTarget::MultiLayerTarget() :
    ShapeBase{this},
    fWidth{6_cm},
    fHeight{5_cm},
    fThickness{3_mm},
    fSpacing{2_mm},
    fCount{11},
    fDetailType{ShapeDetailType::Perforated},
    fPerforated{} {}

Target::MultiLayerTarget::PerforatedMultiLayer::PerforatedMultiLayer() :
    DetailBase{this},
    fHalfExtentZ{5_cm / 2},
    fHalfExtentY{4_cm / 2},
    fSpacing{55_um},
    fRadius{184_um / 2} {}

Target::CylinderTarget::CylinderTarget() :
    ShapeBase{this},
    fRadius{30_mm},
    fThickness{60_mm} {}

} // namespace MACE::Detector::Description
