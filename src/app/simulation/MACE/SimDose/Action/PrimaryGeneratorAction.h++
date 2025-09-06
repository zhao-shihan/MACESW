#pragma once

#include "Mustard/Env/Memory/PassiveSingleton.h++"
#include "Mustard/Geant4X/Generator/GeneralParticleSourceX.h++"

#include "G4VUserPrimaryGeneratorAction.hh"

namespace MACE::SimDose::inline Action {

class PrimaryGeneratorAction final : public Mustard::Env::Memory::PassiveSingleton<PrimaryGeneratorAction>,
                                     public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();

    auto GeneratePrimaries(G4Event* event) -> void override;

private:
    Mustard::Geant4X::GeneralParticleSourceX fGPSX;
};

} // namespace MACE::SimDose::inline Action
