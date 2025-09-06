#include "MACE/SimPTS/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimPTS/Action/TrackingAction.h++"
#include "MACE/SimPTS/Analysis.h++"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

namespace MACE::SimPTS::inline Action {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
    PassiveSingleton{this},
    G4VUserPrimaryGeneratorAction{},
    fGPSX{},
    fSavePrimaryVertexData{true},
    fPrimaryVertexData{},
    fMessengerRegister{this} {}

auto PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) -> void {
    fGPSX.GeneratePrimaryVertex(event);
    if (fSavePrimaryVertexData) {
        UpdatePrimaryVertexData(*event);
    }
}

auto PrimaryGeneratorAction::UpdatePrimaryVertexData(const G4Event& event) -> void {
    fPrimaryVertexData.clear();
    fPrimaryVertexData.reserve(event.GetNumberOfPrimaryVertex());
    for (const auto* pv{event.GetPrimaryVertex()}; pv; pv = pv->GetNext()) {
        for (const auto* pp{pv->GetPrimary()}; pp; pp = pp->GetNext()) {
            const auto& v{fPrimaryVertexData.emplace_back(std::make_unique_for_overwrite<Mustard::Data::Tuple<MACE::Data::SimPrimaryVertex>>())};
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

} // namespace MACE::SimPTS::inline Action
