#pragma once

#include "MACE/Simulation/SD/TTCSD.h++"

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

class TTCSD final : public MACE::Simulation::TTCSD {
public:
    using MACE::Simulation::TTCSD::TTCSD;

    auto EndOfEvent(G4HCofThisEvent* hc) -> void override;
};

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
