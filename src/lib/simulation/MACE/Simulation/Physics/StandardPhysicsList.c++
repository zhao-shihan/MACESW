#include "MACE/Detector/Description/Target.h++"
#include "MACE/Simulation/Physics/StandardPhysicsList.h++"

#include "Mustard/Env/BasicEnv.h++"
#include "Mustard/Geant4X/Physics/MuonLFVDecayPhysics.h++"
#include "Mustard/Geant4X/Physics/MuoniumLFVDecayPhysics.h++"
#include "Mustard/Geant4X/Physics/MuoniumPhysics.h++"
#include "Mustard/Geant4X/Physics/PionKaonDecayMakeSpinPhysics.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "G4BuilderType.hh"
#include "G4EmParameters.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4MscStepLimitType.hh"
#include "G4OpticalParameters.hh"
#include "G4OpticalPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4SpinDecayPhysics.hh"

#include "muc/utility"

#include "fmt/core.h"

#include <algorithm>

namespace MACE::inline Simulation::inline Physics {

StandardPhysicsListBase::StandardPhysicsListBase() :
    QBBC{std::max({}, muc::to_underlying(Mustard::Env::BasicEnv::Instance().VerboseLevel()))},
    fMessengerRegister{this} {
    // EMZ
    ReplacePhysics(new G4EmStandardPhysics_option4{verboseLevel});
    // Muonium physics
    RegisterPhysics(new Mustard::Geant4X::MuoniumPhysics<Detector::Description::Target>{verboseLevel});
    // HP decay for muon and muonium
    RegisterPhysics(new Mustard::Geant4X::MuonLFVDecayPhysics{verboseLevel});
    RegisterPhysics(new Mustard::Geant4X::MuoniumLFVDecayPhysics{verboseLevel});
    // Pion/Kaon decay into polarized muon
    RegisterPhysics(new Mustard::Geant4X::PionKaonDecayMakeSpinPhysics{verboseLevel});

    // Set EM parameters
    using namespace Mustard::LiteralUnit::Energy;
    auto& emParameter{*G4EmParameters::Instance()};
    emParameter.SetLowestElectronEnergy(0.1_eV);
    emParameter.SetLowestMuHadEnergy(0.1_eV);
    emParameter.SetLowestTripletEnergy(0.1_eV);
    emParameter.SetMinEnergy(0.1_eV);
    emParameter.SetMscMuHadStepLimitType(fUseSafetyPlus);
    emParameter.SetMscPositronCorrection(true);
}

auto StandardPhysicsListBase::UseRadioactiveDecayPhysics() -> void {
    RegisterPhysics(new G4RadioactiveDecayPhysics{verboseLevel});
}

auto StandardPhysicsListBase::UseOpticalPhysics() -> void {
    RegisterPhysics(new G4OpticalPhysics{verboseLevel});
    G4OpticalParameters::Instance()->SetBoundaryInvokeSD(true);
}

StandardPhysicsList::StandardPhysicsList() :
    PassiveSingleton{this},
    StandardPhysicsListBase{} {}

} // namespace MACE::inline Simulation::inline Physics
