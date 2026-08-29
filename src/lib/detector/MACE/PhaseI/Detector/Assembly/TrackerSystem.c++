#include "MACE/PhaseI/Detector/Assembly/TrackerSystem.h++"
#include "MACE/PhaseI/Detector/Definition/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Definition/TTC.h++"

namespace MACE::PhaseI::Detector::Assembly {

TrackerSystem::TrackerSystem(Mustard::Detector::Definition::DefinitionBase& mother, bool checkOverlap) :
    AssemblyBase{} {
    auto& sciFiTracker{mother.NewDaughter<MACE::PhaseI::Detector::Definition::SciFiTracker>(checkOverlap)};
    // TTC should be added here

    AddTop(sciFiTracker);
    TopComplete();
}

} // namespace MACE::PhaseI::Detector::Assembly