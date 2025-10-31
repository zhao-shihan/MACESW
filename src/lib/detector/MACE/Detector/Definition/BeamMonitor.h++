#pragma once

#include "Mustard/Detector/Definition/DefinitionBase.h++"

namespace MACE::Detector::Definition {

class BeamMonitor final : public Mustard::Detector::Definition::DefinitionBase {
public:
    auto Enabled() const -> bool override;

private:
    auto Construct(bool checkOverlaps) -> void override;
};

} // namespace MACE::Detector::Definition
