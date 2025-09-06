#pragma once

#include "MACE/SimECAL/Action/DetectorConstruction.h++"

#include "Mustard/Geant4X/Interface/DetectorMessenger.h++"

namespace MACE::SimECAL::inline Messenger {

class DetectorMessenger final : public Mustard::Geant4X::DetectorMessenger<DetectorMessenger,
                                                                           DetectorConstruction,
                                                                           "SimECAL"> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    DetectorMessenger() = default;
    ~DetectorMessenger() = default;
};

} // namespace MACE::SimECAL::inline Messenger
