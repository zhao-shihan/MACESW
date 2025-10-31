#pragma once

#include "Mustard/Geant4X/Run/MPIRunManager.h++"

#include <memory>

namespace MACE::SimTarget {

class Analysis;

class RunManager final : public Mustard::Geant4X::MPIRunManager {
public:
    RunManager();
    ~RunManager() override;

    static auto Instance() -> auto& { return static_cast<RunManager&>(*GetRunManager()); }

private:
    std::unique_ptr<Analysis> fAnalysis;
};

} // namespace MACE::SimTarget
