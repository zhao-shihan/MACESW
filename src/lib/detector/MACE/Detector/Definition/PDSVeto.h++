#pragma once

#include "Mustard/Detector/Definition/DefinitionBase.h++"

namespace MACE::Detector::Definition {

class PDSVeto final : public Mustard::Detector::Definition::DefinitionBase {
private:
    auto Construct(bool checkOverlaps) -> void override;
};

} // namespace MACE::Detector::Definition
