#pragma once

#include "MACE/SimDose/Action/DetectorConstruction.h++"

#include "Mustard/Geant4X/Interface/DetectorMessenger.h++"

namespace MACE::SimDose::inline Messenger {

class DetectorMessenger final : public Mustard::Geant4X::DetectorMessenger<DetectorMessenger,
                                                                           DetectorConstruction,
                                                                           "SimDose"> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    DetectorMessenger() = default;
    ~DetectorMessenger() = default;
};

} // namespace MACE::SimDose::inline Messenger
