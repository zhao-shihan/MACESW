#include "MACE/Detector/Definition/TTC.h++"
#include "MACE/PhaseI/Detector/Assembly/TrackerSystem.h++"
#include "MACE/PhaseI/Detector/Definition/SciFiTracker.h++"

namespace MACE::PhaseI::Detector::Assembly {

TrackerSystem::TrackerSystem(Mustard::Detector::Definition::DefinitionBase& mother, bool checkOverlap) :
    AssemblyBase{} {
    auto& sciFiTracker{mother.NewDaughter<MACE::PhaseI::Detector::Definition::SciFiTracker>(checkOverlap)};
    // TTC should be added here

    AddTop(sciFiTracker);
    TopComplete();
}

} // namespace MACE::PhaseI::Detector::Assembly