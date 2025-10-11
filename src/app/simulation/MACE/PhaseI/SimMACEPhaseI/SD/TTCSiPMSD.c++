#include "MACE/PhaseI/SimMACEPhaseI/Analysis.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SD/TTCSiPMSD.h++"

namespace MACE::PhaseI::SimMACEPhaseI::inline SD {

auto TTCSiPMSD::EndOfEvent(G4HCofThisEvent* hc) -> void {
    MACE::Simulation::TTCSiPMSD::EndOfEvent(hc);
    Analysis::Instance().SubmitTTCSiPMHC(*fHitsCollection->GetVector());
}

} // namespace MACE::PhaseI::SimMACEPhaseI::inline SD
