#pragma once

#include "MACE/SimTarget/Messenger/PrimaryGeneratorActionMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"
#include "Mustard/Geant4X/Generator/DataReaderPrimaryGenerator.h++"
#include "Mustard/Geant4X/Generator/GeneralParticleSourceX.h++"

#include "G4VUserPrimaryGeneratorAction.hh"

namespace MACE::SimTarget::inline Action {

class PrimaryGeneratorAction final : public Mustard::Env::Memory::PassiveSingleton<PrimaryGeneratorAction>,
                                     public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();

    auto SwitchToGPSX() -> void { fGenerator = &fAvailableGenerator.gpsx; }
    auto SwitchToFromDataPrimaryGenerator() -> void { fGenerator = &fAvailableGenerator.dataReaderPrimaryGenerator; }

    auto NVertex() const -> int;

    auto GeneratePrimaries(G4Event* event) -> void override { fGenerator->GeneratePrimaryVertex(event); }

private:
    struct {
        Mustard::Geant4X::GeneralParticleSourceX gpsx;
        Mustard::Geant4X::DataReaderPrimaryGenerator dataReaderPrimaryGenerator;
    } fAvailableGenerator;
    G4VPrimaryGenerator* fGenerator;

    PrimaryGeneratorActionMessenger::Register<PrimaryGeneratorAction> fPrimaryGeneratorActionMessengerRegister;
};

} // namespace MACE::SimTarget::inline Action
