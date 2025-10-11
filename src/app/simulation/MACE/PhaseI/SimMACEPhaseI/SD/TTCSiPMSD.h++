#pragma once

#include "MACE/Simulation/SD/TTCSiPMSD.h++"

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

class TTCSiPMSD final : public MACE::Simulation::TTCSiPMSD {
public:
    using MACE::Simulation::TTCSiPMSD::TTCSiPMSD;

    auto EndOfEvent(G4HCofThisEvent* hc) -> void override;
};

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
