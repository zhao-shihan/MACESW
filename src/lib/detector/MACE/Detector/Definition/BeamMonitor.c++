#include "MACE/Detector/Definition/BeamMonitor.h++"
#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/BeamMonitor.h++"
#include "MACE/Detector/Description/Target.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

namespace MACE::Detector::Definition {

using namespace Mustard::LiteralUnit::Density;

auto BeamMonitor::Enabled() const -> bool {
    return Description::BeamMonitor::Instance().Enabled();
}

auto BeamMonitor::Construct(G4bool checkOverlaps) -> void {
    const auto& monitor{Description::BeamMonitor::Instance()};
    const auto& accelerator{Description::Accelerator::Instance()};
    const auto& target{Description::Target::Instance()};

    G4Transform3D transform;
    switch (const auto z0{(accelerator.UpstreamFieldLength() - accelerator.DownstreamFieldLength()) / 2};
            target.ShapeType()) {
    case Description::Target::TargetShapeType::Cuboid: {
        const auto& cuboid{target.Cuboid()};
        const auto targetExtent{cuboid.Width() * cuboid.SinTiltAngle() / 2 + cuboid.Thickness() * cuboid.CosTiltAngle()};
        transform = G4TranslateZ3D{z0 - targetExtent - monitor.DistanceToTarget() - monitor.Thickness() / 2};
    } break;
    case Description::Target::TargetShapeType::MultiLayer:
        transform = G4TranslateZ3D{z0 - target.MultiLayer().Width() / 2 - monitor.DistanceToTarget() - monitor.Thickness() / 2};
        break;
    case Description::Target::TargetShapeType::Cylinder:
        transform = G4TranslateZ3D{z0 - target.Cylinder().Thickness() / 2 - monitor.DistanceToTarget() - monitor.Thickness() / 2};
        break;
    }

    const auto solid{Make<G4Box>(
        monitor.Name(),
        monitor.Width() / 2,
        monitor.Width() / 2,
        monitor.Thickness() / 2)};
    const auto logic{Make<G4LogicalVolume>(
        solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),
        monitor.Name())};
    Make<G4PVPlacement>(
        transform,
        logic,
        monitor.Name(),
        Mother().LogicalVolume(),
        false,
        0,
        checkOverlaps);
}

} // namespace MACE::Detector::Definition
