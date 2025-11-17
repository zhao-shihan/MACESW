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

#include "MACE/Detector/Description/Target.h++"

#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "CLHEP/Vector/Rotation.h"

#include "muc/utility"

#include <string>

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit;

Target::Target() :
    DescriptionBase{"Target"},
    fShapeType{TargetShapeType::Cuboid},
    fCuboid{},
    fMultiLayer{},
    fCylinder{},
    fSilicaAerogelDensity{27_mg_cm3},
    fEffectiveTemperature{400_K},
    fFormationProbability{0.655},
    fMeanFreePath{200_nm},
    fMaterialName{"SilicaAerogel"} {}

auto Target::ShapeTypeString() const -> std::string_view {
    switch (fShapeType) {
    case TargetShapeType::Cuboid:
        return "Cuboid";
    case TargetShapeType::MultiLayer:
        return "MultiLayer";
    case TargetShapeType::Cylinder:
        return "Cylinder";
    }
    muc::unreachable();
}

auto Target::ShapeType(std::string_view val) -> void {
    if (val == "Cuboid") {
        fShapeType = TargetShapeType::Cuboid;
    } else if (val == "MultiLayer") {
        fShapeType = TargetShapeType::MultiLayer;
    } else if (val == "Cylinder") {
        fShapeType = TargetShapeType::Cylinder;
    } else {
        Mustard::PrintWarning(fmt::format("Unknown target shape '{}', ignoring it", val));
    }
}

Target::CuboidTarget::CuboidTarget() :
    ShapeBase{this},
    fWidth{12_cm},
    fHeight{6_cm},
    fThickness{5_mm},
    fTiltAngle{60_deg},
    fCosTiltAngle{std::cos(fTiltAngle)},
    fSinTiltAngle{std::sin(fTiltAngle)},
    fDetailType{ShapeDetailType::Perforated},
    fPerforated{this} {}

auto Target::CuboidTarget::TiltAngle(double val) -> void {
    fTiltAngle = val;
    fCosTiltAngle = std::cos(val);
    fSinTiltAngle = std::sin(val);
}

auto Target::CuboidTarget::DetailTypeString() const -> std::string_view {
    switch (fDetailType) {
    case ShapeDetailType::Flat:
        return "Flat";
    case ShapeDetailType::Perforated:
        return "Perforated";
    }
    muc::unreachable();
}

auto Target::CuboidTarget::DetailType(std::string_view val) -> void {
    if (val == "Flat") {
        fDetailType = ShapeDetailType::Flat;
    } else if (val == "Perforated") {
        fDetailType = ShapeDetailType::Perforated;
    } else {
        Mustard::PrintWarning(fmt::format("Unknown multiLayer target detail '{}', ignoring it", val));
    }
}

Target::CuboidTarget::PerforatedCuboid::PerforatedCuboid(gsl::not_null<const CuboidTarget*> cuboid) :
    DetailBase{this},
    fCuboid{cuboid},
    fWidthExtent{10_cm},
    fHeightExtent{5_cm},
    fSpacing{55_um},
    fDiameter{184_um},
    fDepth{5_mm} {}

Target::MultiLayerTarget::MultiLayerTarget() :
    ShapeBase{this},
    fWidth{6_cm},
    fHeight{5_cm},
    fThickness{3_mm},
    fSpacing{2_mm},
    fCount{11},
    fDetailType{ShapeDetailType::Perforated},
    fPerforated{} {}

auto Target::MultiLayerTarget::DetailTypeString() const -> std::string_view {
    switch (fDetailType) {
    case ShapeDetailType::Flat:
        return "Flat";
    case ShapeDetailType::Perforated:
        return "Perforated";
    }
    muc::unreachable();
}

auto Target::MultiLayerTarget::DetailType(std::string_view val) -> void {
    if (val == "Flat") {
        fDetailType = ShapeDetailType::Flat;
    } else if (val == "Perforated") {
        fDetailType = ShapeDetailType::Perforated;
    } else {
        Mustard::PrintWarning(fmt::format("Unknown multiLayer target detail '{}', ignoring it", val));
    }
}

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

auto Target::ImportAllValue(const YAML::Node& node) -> void {
    ImportValue<std::string>(node, [this](auto&& value) { ShapeType(value); }, "ShapeType");
    {
        ImportValue<double>(node, [this](auto value) { fCuboid.Width(value); }, "Cuboid", "Width");
        ImportValue<double>(node, [this](auto value) { fCuboid.Height(value); }, "Cuboid", "Height");
        ImportValue<double>(node, [this](auto value) { fCuboid.Thickness(value); }, "Cuboid", "Thickness");
        ImportValue<double>(node, [this](auto value) { fCuboid.TiltAngle(value); }, "Cuboid", "TiltAngle");
        ImportValue<std::string>(node, [this](auto&& value) { fCuboid.DetailType(value); }, "Cuboid", "DetailType");
        {
            ImportValue<double>(node, [this](auto value) { fCuboid.Perforated().WidthExtent(value); }, "Cuboid", "Perforated", "WidthExtent");
            ImportValue<double>(node, [this](auto value) { fCuboid.Perforated().HeightExtent(value); }, "Cuboid", "Perforated", "HeightExtent");
            ImportValue<double>(node, [this](auto value) { fCuboid.Perforated().Spacing(value); }, "Cuboid", "Perforated", "Spacing");
            ImportValue<double>(node, [this](auto value) { fCuboid.Perforated().Diameter(value); }, "Cuboid", "Perforated", "Diameter");
            ImportValue<double>(node, [this](auto value) { fCuboid.Perforated().Depth(value); }, "Cuboid", "Perforated", "Depth");
        }
    }
    {
        ImportValue<double>(node, [this](auto value) { fMultiLayer.Width(value); }, "MultiLayer", "Width");
        ImportValue<double>(node, [this](auto value) { fMultiLayer.Height(value); }, "MultiLayer", "Height");
        ImportValue<double>(node, [this](auto value) { fMultiLayer.Thickness(value); }, "MultiLayer", "Thickness");
        ImportValue<double>(node, [this](auto value) { fMultiLayer.Spacing(value); }, "MultiLayer", "Spacing");
        ImportValue<int>(node, [this](auto value) { fMultiLayer.Count(value); }, "MultiLayer", "Count");
        ImportValue<std::string>(node, [this](auto&& value) { fMultiLayer.DetailType(value); }, "MultiLayer", "DetailType");
        {
            ImportValue<double>(node, [this](auto value) { fMultiLayer.Perforated().ExtentZ(value); }, "MultiLayer", "Perforated", "ExtentZ");
            ImportValue<double>(node, [this](auto value) { fMultiLayer.Perforated().ExtentY(value); }, "MultiLayer", "Perforated", "ExtentY");
            ImportValue<double>(node, [this](auto value) { fMultiLayer.Perforated().Spacing(value); }, "MultiLayer", "Perforated", "Spacing");
            ImportValue<double>(node, [this](auto value) { fMultiLayer.Perforated().Diameter(value); }, "MultiLayer", "Perforated", "Diameter");
        }
    }
    {
        ImportValue<double>(node, [this](auto value) { fCylinder.Radius(value); }, "Cylinder", "Radius");
        ImportValue<double>(node, [this](auto value) { fCylinder.Thickness(value); }, "Cylinder", "Thickness");
    }
    ImportValue(node, fSilicaAerogelDensity, "SilicaAerogelDensity");
    ImportValue(node, fEffectiveTemperature, "EffectiveTemperature");
    ImportValue(node, fFormationProbability, "FormationProbability");
    ImportValue(node, fMeanFreePath, "MeanFreePath");
}

auto Target::ExportAllValue(YAML::Node& node) const -> void {
    ExportValue(node, ShapeTypeString(), "ShapeType");
    {
        ExportValue(node, fCuboid.Width(), "Cuboid", "Width");
        ExportValue(node, fCuboid.Thickness(), "Cuboid", "Thickness");
        ExportValue(node, fCuboid.DetailTypeString(), "Cuboid", "DetailType");
        {
            ExportValue(node, fCuboid.Perforated().WidthExtent(), "Cuboid", "Perforated", "WidthExtent");
            ExportValue(node, fCuboid.Perforated().HeightExtent(), "Cuboid", "Perforated", "HeightExtent");
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
        ExportValue(node, fMultiLayer.DetailTypeString(), "MultiLayer", "DetailType");
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

} // namespace MACE::Detector::Description
