#include "MACE/SimTTC/Action/EventAction.h++"
#include "MACE/SimTTC/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimTTC/Action/TrackingAction.h++"
#include "MACE/SimTTC/Analysis.h++"

#include "G4Event.hh"

namespace MACE::SimTTC::inline Action {

EventAction::EventAction() :
    PassiveSingleton{this},
    G4UserEventAction{} {}

auto EventAction::BeginOfEventAction(const G4Event*) -> void {
    if (auto& trackingAction{TrackingAction::Instance()};
        trackingAction.SaveDecayVertexData()) {
        trackingAction.ClearDecayVertexData();
    }
}

auto EventAction::EndOfEventAction(const G4Event*) -> void {
    auto& analysis{Analysis::Instance()};
    if (const auto& trackingAction{TrackingAction::Instance()};
        trackingAction.SaveDecayVertexData()) {
        analysis.SubmitDecayVertexData(trackingAction.DecayVertexData());
    }
    analysis.EventEndAction();
}

} // namespace MACE::SimTTC::inline Action
