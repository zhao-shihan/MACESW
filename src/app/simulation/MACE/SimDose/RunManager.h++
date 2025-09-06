#pragma once

#include "Mustard/CLI/Module/Geant4ReferencePhysicsListModule.h++"
#include "Mustard/Geant4X/Run/MPIRunManager.h++"

#include <memory>

namespace MACE::SimDose {

class Analysis;

class RunManager final : public Mustard::Geant4X::MPIRunManager {
public:
    RunManager(Mustard::CLI::Geant4ReferencePhysicsListModule<"QBBC_EMZ">& cli);
    ~RunManager();

    static auto Instance() -> auto& { return static_cast<RunManager&>(*GetRunManager()); }

private:
    std::unique_ptr<Analysis> fAnalysis;
};

} // namespace MACE::SimDose
