#include "MACE/SimTTC/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimTTC/Action/TrackingAction.h++"
#include "MACE/SimTTC/Analysis.h++"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

namespace MACE::SimTTC::inline Action {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
    PassiveSingleton{this},
    G4VUserPrimaryGeneratorAction{},
    fAvailableGenerator{.gpsx{}, .ecoMug{Mustard::Geant4X::EcoMugCosmicRayMuon::Coordinate::Beam}},
    fGenerator{&fAvailableGenerator.gpsx},
    fSavePrimaryVertexData{true},
    fPrimaryVertexData{},
    fMessengerRegister{this} {}

auto PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) -> void {
    fGenerator->GeneratePrimaryVertex(event);
    if (fSavePrimaryVertexData) {
        UpdatePrimaryVertexData(*event);
    }
}

auto PrimaryGeneratorAction::UpdatePrimaryVertexData(const G4Event& event) -> void {
    fPrimaryVertexData.clear();
    fPrimaryVertexData.reserve(event.GetNumberOfPrimaryVertex());
    for (const auto* pv{event.GetPrimaryVertex()}; pv; pv = pv->GetNext()) {
        for (const auto* pp{pv->GetPrimary()}; pp; pp = pp->GetNext()) {
            const auto& v{fPrimaryVertexData.emplace_back(std::make_unique_for_overwrite<Mustard::Data::Tuple<Data::SimPrimaryVertex>>())};
            Get<"EvtID">(*v) = event.GetEventID();
            Get<"PDGID">(*v) = pp->GetPDGcode();
            Get<"t0">(*v) = pv->GetT0();
            Get<"x0">(*v) = pv->GetPosition();
            Get<"Ek0">(*v) = pp->GetKineticEnergy();
            Get<"p0">(*v) = pp->GetMomentum();
        }
    }
    Analysis::Instance().SubmitPrimaryVertexData(fPrimaryVertexData);
}

} // namespace MACE::SimTTC::inline Action
