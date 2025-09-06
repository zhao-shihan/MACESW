#include "MACE/SimTarget/Action/PrimaryGeneratorAction.h++"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

#include "muc/utility"

namespace MACE::SimTarget::inline Action {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
    PassiveSingleton{this},
    G4VUserPrimaryGeneratorAction{},
    fAvailableGenerator{},
    fGenerator{&fAvailableGenerator.gpsx},
    fPrimaryGeneratorActionMessengerRegister{this} {}

auto PrimaryGeneratorAction::NVertex() const -> int {
    const auto& [gpsx, dataReaderPrimaryGenerator]{fAvailableGenerator};
    if (fGenerator == &gpsx) {
        return gpsx.NVertex();
    } else if (fGenerator == &dataReaderPrimaryGenerator) {
        return dataReaderPrimaryGenerator.NVertex();
    }
    muc::unreachable();
}

} // namespace MACE::SimTarget::inline Action
