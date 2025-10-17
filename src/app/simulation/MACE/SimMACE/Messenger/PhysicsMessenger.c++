#include "MACE/SimMACE/Action/DetectorConstruction.h++"
#include "MACE/SimMACE/Messenger/PhysicsMessenger.h++"

#include "G4UIcmdWithABool.hh"

namespace MACE::SimMACE::inline Messenger {

PhysicsMessenger::PhysicsMessenger() :
    SingletonMessenger{},
    fApplyProductionCutNearTarget{} {
    fApplyProductionCutNearTarget = std::make_unique<G4UIcmdWithABool>("/MACE/Physics/ApplyProductionCutNearTarget", this);
    fApplyProductionCutNearTarget->SetGuidance("If yes, apply default production cut to the material-to-vacuum region near the target");
    fApplyProductionCutNearTarget->SetParameterName("apply", false);
    fApplyProductionCutNearTarget->AvailableForStates(G4State_Idle);
}

PhysicsMessenger::~PhysicsMessenger() = default;

auto PhysicsMessenger::SetNewValue(G4UIcommand* command, G4String value) -> void {
    if (command == fApplyProductionCutNearTarget.get()) {
        Deliver<DetectorConstruction>([&](auto&& r) {
            r.ApplyProductionCutNearTarget(fApplyProductionCutNearTarget->GetNewBoolValue(value));
        });
    }
}

} // namespace MACE::SimMACE::inline Messenger
