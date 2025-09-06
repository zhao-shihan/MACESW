#pragma once

#include "MACE/PhaseI/SimMACEPhaseI/Action/DetectorConstruction.h++"

#include "Mustard/Geant4X/Interface/DetectorMessenger.h++"

namespace MACE::PhaseI::SimMACEPhaseI::inline Messenger {

class DetectorMessenger final : public Mustard::Geant4X::DetectorMessenger<DetectorMessenger,
                                                                           DetectorConstruction,
                                                                           "SimMACEPhaseI"> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    DetectorMessenger() = default;
    ~DetectorMessenger() = default;
};

} // namespace MACE::PhaseI::SimMACEPhaseI::inline Messenger
