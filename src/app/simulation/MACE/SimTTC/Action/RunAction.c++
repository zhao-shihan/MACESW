#include "MACE/SimTTC/Action/RunAction.h++"
#include "MACE/SimTTC/Analysis.h++"

#include "G4Run.hh"

namespace MACE::SimTTC::inline Action {

RunAction::RunAction() :
    PassiveSingleton{this},
    G4UserRunAction{} {}

auto RunAction::BeginOfRunAction(const G4Run* run) -> void {
    Analysis::Instance().RunBeginAction(run->GetRunID());
}

auto RunAction::EndOfRunAction(const G4Run* run) -> void {
    Analysis::Instance().RunEndAction(run->GetRunID());
}

} // namespace MACE::SimTTC::inline Action
