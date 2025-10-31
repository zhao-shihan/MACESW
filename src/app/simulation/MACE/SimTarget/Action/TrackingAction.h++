#pragma once

#include "MACE/SimTarget/Analysis.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4Types.hh"
#include "G4UserTrackingAction.hh"

#include "gsl/gsl"

class G4ParticleDefinition;

namespace MACE::SimTarget::inline Action {

class TrackingAction final : public Mustard::Env::Memory::PassiveSingleton<TrackingAction>,
                             public G4UserTrackingAction {
public:
    TrackingAction();

    auto PreUserTrackingAction(const G4Track* track) -> void override;
    auto PostUserTrackingAction(const G4Track* track) -> void override;

private:
    Mustard::Data::Tuple<MuoniumTrack>* fMuoniumTrack;
};

} // namespace MACE::SimTarget::inline Action
