#pragma once

#include "Mustard/Geant4X/Run/MPIRunManager.h++"

#include <memory>

namespace MACE::PhaseI::SimMACEPhaseI {

class Analysis;

class RunManager final : public Mustard::Geant4X::MPIRunManager {
public:
    RunManager();
    ~RunManager();

    static auto Instance() -> auto& { return static_cast<RunManager&>(*GetRunManager()); }

private:
    std::unique_ptr<Analysis> fAnalysis;
};

} // namespace MACE::PhaseI::SimMACEPhaseI
