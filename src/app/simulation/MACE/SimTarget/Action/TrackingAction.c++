#include "MACE/SimTarget/Action/TrackingAction.h++"

#include "Mustard/Geant4X/Particle/Antimuonium.h++"
#include "Mustard/Geant4X/Particle/Muonium.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Track.hh"

#include "gsl/gsl"

namespace MACE::SimTarget::inline Action {

using namespace Mustard::PhysicalConstant;

TrackingAction::TrackingAction() :
    PassiveSingleton{this},
    G4UserTrackingAction{},
    fMuoniumTrack{} {}

auto TrackingAction::PreUserTrackingAction(const G4Track* track) -> void {
    if (const auto particle{track->GetParticleDefinition()};
        particle == Mustard::Geant4X::Muonium::Definition() or particle == Mustard::Geant4X::Antimuonium::Definition()) {
        fMuoniumTrack = Analysis::Instance().NewMuoniumTrack();
        Get<"EvtID">(*fMuoniumTrack) = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
        Get<"TrkID">(*fMuoniumTrack) = track->GetTrackID();
        Get<"PDGID">(*fMuoniumTrack) = particle->GetPDGEncoding();
        Get<"t0">(*fMuoniumTrack) = track->GetGlobalTime();
        Get<"x0">(*fMuoniumTrack) = track->GetPosition();
        Get<"Ek0">(*fMuoniumTrack) = track->GetKineticEnergy();
        Get<"p0">(*fMuoniumTrack) = track->GetMomentum();
    }
}

auto TrackingAction::PostUserTrackingAction(const G4Track* track) -> void {
    if (const auto particle{track->GetParticleDefinition()};
        particle == Mustard::Geant4X::Muonium::Definition() or particle == Mustard::Geant4X::Antimuonium::Definition()) {
        Get<"t">(*fMuoniumTrack) = track->GetGlobalTime();
        Get<"x">(*fMuoniumTrack) = track->GetPosition();
        Get<"Ek">(*fMuoniumTrack) = track->GetKineticEnergy();
        Get<"p">(*fMuoniumTrack) = track->GetMomentum();
    }
}

} // namespace MACE::SimTarget::inline Action
