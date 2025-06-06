#pragma once

#include "MACE/Simulation/SD/TTCSD.h++"

namespace MACE::SimTTC::inline SD {

class TTCSD final : public Simulation::TTCSD {
public:
    using Simulation::TTCSD::TTCSD;

    auto EndOfEvent(G4HCofThisEvent* hc) -> void override;
};

} // namespace MACE::SimTTC::inline SD
