#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Detector/Description/Target.h++"
#include "MACE/PhaseI/Detector/Description/UsePhaseIDefault.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

auto UsePhaseIDefault() -> void {
    using namespace Mustard::LiteralUnit::Length;
    namespace MACE = MACE::Detector::Description;
    { // set accelerator up/downstream length to a random equal value
        auto& accelerator{MACE::Accelerator::Instance()};
        accelerator.MaxPotentialPosition(0);
        accelerator.AccelerateFieldLength(2);
        accelerator.DecelerateFieldLength(2);
    }
    { // Use cylinder target
        auto& target{MACE::Target::Instance()};
        target.ShapeType(MACE::Target::TargetShapeType::Cylinder);
    }
    { // bigger windows for ECAL
        auto& ecal{MACE::ECAL::Instance()};
        ecal.UpstreamWindowRadius(104_mm);
        ecal.DownstreamWindowRadius(104_mm);
        ecal.InnerRadius(260_mm);
    }
}

} // namespace MACE::PhaseI::Detector::Description
