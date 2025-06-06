#include "MACE/SimTTC/Analysis.h++"
#include "MACE/SimTTC/SD/TTCSiPMSD.h++"

namespace MACE::SimTTC::inline SD {

auto TTCSiPMSD::EndOfEvent(G4HCofThisEvent* hc) -> void {
    Simulation::TTCSiPMSD::EndOfEvent(hc);
    Analysis::Instance().SubmitTTCSiPMHC(*fHitsCollection->GetVector());
}

} // namespace MACE::SimTTC::inline SD
