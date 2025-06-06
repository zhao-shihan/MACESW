#pragma once

#include "MACE/Simulation/SD/TTCSiPMSD.h++"

namespace MACE::SimTTC::inline SD {

class TTCSiPMSD final : public Simulation::TTCSiPMSD {
public:
    using Simulation::TTCSiPMSD::TTCSiPMSD;

    auto EndOfEvent(G4HCofThisEvent* hc) -> void override;
};

} // namespace MACE::SimTTC::inline SD
