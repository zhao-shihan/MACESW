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

#include "MACE/Detector/Definition/BeamDegrader.h++"
#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/BeamDegrader.h++"
#include "MACE/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::Density;

auto BeamDegrader::Enabled() const -> bool {
    return Description::BeamDegrader::Instance().Enabled();
}

auto BeamDegrader::Construct(G4bool checkOverlaps) -> void {
    const auto& degrader{Description::BeamDegrader::Instance()};
    const auto& accelerator{Description::Accelerator::Instance()};
    const auto& target{Description::Target::Instance()};

    G4Transform3D transform;
    switch (const auto z0{(accelerator.UpstreamFieldLength() - accelerator.DownstreamFieldLength()) / 2};
            target.ShapeType()) {
    case Description::Target::TargetShapeType::Cuboid:
        transform = G4TranslateZ3D{z0 - target.Cuboid().EffectiveThickness() - degrader.DistanceToTarget() - degrader.Thickness() / 2} *
                    G4RotateY3D{-target.Cuboid().TiltAngle()};
        break;
    case Description::Target::TargetShapeType::MultiLayer:
        transform = G4TranslateZ3D{z0 - target.MultiLayer().Width() / 2 - degrader.DistanceToTarget() - degrader.Thickness() / 2};
        break;
    case Description::Target::TargetShapeType::Cylinder:
        transform = G4TranslateZ3D{z0 - target.Cylinder().Thickness() / 2 - degrader.DistanceToTarget() - degrader.Thickness() / 2};
        break;
    }

    const auto solid{Make<G4Box>(
        degrader.Name(),
        degrader.Width() / 2,
        degrader.Height() / 2,
        degrader.Thickness() / 2)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        G4NistManager::Instance()->FindOrBuildMaterial(degrader.MaterialName()),
        degrader.Name())};
    Make<G4PVPlacement>(
        transform,
        logic,
        degrader.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
