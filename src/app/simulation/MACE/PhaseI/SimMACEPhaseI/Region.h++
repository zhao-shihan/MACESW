#pragma once

#include "G4Region.hh"

namespace MACE::PhaseI::SimMACEPhaseI {

enum struct RegionType {
    DefaultGaseous,
    DefaultSolid,
    ECALSensitive,
    Shield,
    Target,
    Vacuum
};

class Region final : public G4Region {
public:
    Region(const G4String& name, RegionType type);

    auto Type() const -> auto { return fRegionType; }

private:
    const RegionType fRegionType;
};

} // namespace MACE::PhaseI::SimMACEPhaseI
