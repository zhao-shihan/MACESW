#include "MACE/PhaseI/SimMACEPhaseI/Analysis.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/TTCSD.h++"

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

auto TTCSD::EndOfEvent(G4HCofThisEvent* hc) -> void {
    MACE::Simulation::TTCSD::EndOfEvent(hc);
    Analysis::Instance().SubmitTTCHC(*fHitsCollection->GetVector());
}

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
