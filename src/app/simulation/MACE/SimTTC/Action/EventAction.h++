#pragma once

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4UserEventAction.hh"

namespace MACE::SimTTC::inline Action {

class EventAction final : public Mustard::Env::Memory::PassiveSingleton<EventAction>,
                          public G4UserEventAction {
public:
    EventAction();

    auto BeginOfEventAction(const G4Event*) -> void override;
    auto EndOfEventAction(const G4Event*) -> void override;
};

} // namespace MACE::SimTTC::inline Action
