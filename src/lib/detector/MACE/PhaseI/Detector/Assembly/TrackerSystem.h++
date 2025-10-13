#pragma once

#include "MACE/Detector/Description/FieldOption.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Description/TTC.h++"

#include "Mustard/Detector/Assembly/AssemblyBase.h++"

namespace MACE::PhaseI::Detector::Assembly {

class TrackerSystem : public Mustard::Detector::Assembly::AssemblyBase {
public:
    using DescriptionInUse = std::tuple<MACE::PhaseI::Detector::Description::SciFiTracker,
                                        MACE::PhaseI::Detector::Description::TTC>;

public:
    TrackerSystem(Mustard::Detector::Definition::DefinitionBase& mother, bool checkOverlap);
};

} // namespace MACE::PhaseI::Detector::Assembly
