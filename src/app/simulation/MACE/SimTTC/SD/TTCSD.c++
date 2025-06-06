#include "MACE/SimTTC/Analysis.h++"
#include "MACE/SimTTC/SD/TTCSD.h++"

namespace MACE::SimTTC::inline SD {

auto TTCSD::EndOfEvent(G4HCofThisEvent* hc) -> void {
    Simulation::TTCSD::EndOfEvent(hc);
    Analysis::Instance().SubmitTTCHC(*fHitsCollection->GetVector());
}

} // namespace MACE::SimTTC::inline SD
