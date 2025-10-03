#pragma once

#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/FieldOption.h++"
#include "MACE/Detector/Description/MMSBeamPipe.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/MMSMagnet.h++"
#include "MACE/Detector/Description/MMSShield.h++"
#include "MACE/Detector/Description/TTC.h++"

#include "Mustard/Detector/Assembly/AssemblyBase.h++"

namespace MACE::Detector::Assembly {

class MMS : public Mustard::Detector::Assembly::AssemblyBase {
public:
    using ProminentDescription = std::tuple<Detector::Description::CDC,
                                            Detector::Description::FieldOption,
                                            Detector::Description::MMSBeamPipe,
                                            Detector::Description::MMSField,
                                            Detector::Description::MMSMagnet,
                                            Detector::Description::MMSShield,
                                            Detector::Description::TTC>;

public:
    MMS(Mustard::Detector::Definition::DefinitionBase& mother, bool checkOverlap);
};

} // namespace MACE::Detector::Assembly
