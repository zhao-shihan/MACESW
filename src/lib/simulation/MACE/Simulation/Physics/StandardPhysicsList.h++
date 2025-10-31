#pragma once

#include "MACE/Simulation/Physics/StandardPhysicsListMessenger.h++"

#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "QBBC.hh"

namespace MACE::inline Simulation::inline Physics {

class StandardPhysicsListBase : public QBBC {
public:
    StandardPhysicsListBase();
    ~StandardPhysicsListBase() override = default;

    auto UseRadioactiveDecayPhysics() -> void;
    auto UseOpticalPhysics() -> void;

private:
    StandardPhysicsListMessenger::Register<StandardPhysicsListBase> fMessengerRegister;
};

class StandardPhysicsList final : public Mustard::Env::Memory::PassiveSingleton<StandardPhysicsList>,
                                  public StandardPhysicsListBase {
public:
    StandardPhysicsList();
};

} // namespace MACE::inline Simulation::inline Physics
