#pragma once

#include "Mustard/Extension/Geant4X/Run/MPIRunManager.h++"

#include <memory>

namespace MACE::SimTTC {

class Analysis;

class RunManager final : public Mustard::Geant4X::MPIRunManager {
public:
    RunManager();
    ~RunManager();

    static auto Instance() -> auto& { return static_cast<RunManager&>(*GetRunManager()); }

private:
    std::unique_ptr<Analysis> fAnalysis;
};

} // namespace MACE::SimTTC
