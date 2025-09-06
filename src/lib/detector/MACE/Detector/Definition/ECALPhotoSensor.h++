#pragma once

#include "Mustard/Detector/Definition/DefinitionBase.h++"

namespace MACE::Detector::Definition {

class ECALPhotoSensor final : public Mustard::Detector::Definition::DefinitionBase {
private:
    auto Construct(bool checkOverlaps) -> void override;

    auto ConstructMPPC(bool checkOverlaps) -> void;
    auto ConstructPMT(bool checkOverlaps) -> void;
};

} // namespace MACE::Detector::Definition
