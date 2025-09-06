#pragma once

#include "MACE/Data/SimVertex.h++"
#include "MACE/SimECAL/Messenger/AnalysisMessenger.h++"
#include "MACE/SimECAL/Messenger/PrimaryGeneratorActionMessenger.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"
#include "Mustard/Geant4X/Generator/EcoMugCosmicRayMuon.h++"
#include "Mustard/Geant4X/Generator/GeneralParticleSourceX.h++"

#include "G4VUserPrimaryGeneratorAction.hh"

#include "muc/ptrvec"

namespace MACE::SimECAL::inline Action {

class PrimaryGeneratorAction final : public Mustard::Env::Memory::PassiveSingleton<PrimaryGeneratorAction>,
                                     public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();

    auto SwitchToGPSX() -> void { fGenerator = &fAvailableGenerator.gpsx; }
    auto SwitchToEcoMug() -> void { fGenerator = &fAvailableGenerator.ecoMug; }

    auto SavePrimaryVertexData() const -> auto { return fSavePrimaryVertexData; }
    auto SavePrimaryVertexData(bool val) -> void { fSavePrimaryVertexData = val; }

    auto GeneratePrimaries(G4Event* event) -> void override;

private:
    auto UpdatePrimaryVertexData(const G4Event& event) -> void;

private:
    struct {
        Mustard::Geant4X::GeneralParticleSourceX gpsx;
        Mustard::Geant4X::EcoMugCosmicRayMuon ecoMug;
    } fAvailableGenerator;
    G4VPrimaryGenerator* fGenerator;

    bool fSavePrimaryVertexData;
    muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>> fPrimaryVertexData;

    AnalysisMessenger::Register<PrimaryGeneratorAction> fAnalysisMessengerRegister;
    PrimaryGeneratorActionMessenger::Register<PrimaryGeneratorAction> fPrimaryGeneratorActionMessengerRegister;
};

} // namespace MACE::SimECAL::inline Action
