#pragma once

#include "Mustard/Detector/Definition/DefinitionBase.h++"

namespace MACE::PhaseI::Detector::Definition {

class TTC final : public Mustard::Detector::Definition::DefinitionBase {
private:
    auto Construct(bool checkOverlaps) -> void override;
};

} // namespace MACE::Detector::Definition
