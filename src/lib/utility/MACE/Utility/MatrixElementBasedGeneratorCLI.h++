#pragma once

#include "MACE/Utility/EventGeneratorCLI.h++"

#include "Mustard/CLI/CLI.h++"
#include "Mustard/CLI/Module/ModuleBase.h++"
#include "Mustard/Execution/Executor.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/IO/Print.h++"
#include "Mustard/Math/Estimate.h++"
#include "Mustard/Math/MCIntegrationUtility.h++"
#include "Mustard/Physics/Generator/MatrixElementBasedGenerator.h++"

#include "muc/array"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <optional>
#include <tuple>

namespace MACE::inline Utility {

class MatrixElementBasedGeneratorCLIModule : public Mustard::CLI::ModuleBase {
public:
    MatrixElementBasedGeneratorCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli);

    template<int M, int N, typename A>
    auto PhaseSpaceIntegral(Mustard::Executor<unsigned long long>& executor,
                            Mustard::MatrixElementBasedGenerator<M, N, A>& generator) const -> std::tuple<Mustard::Math::Estimate, double, Mustard::Math::MCIntegrationState>;

private:
    auto ContinueIntegration() const -> std::optional<Mustard::Math::MCIntegrationState>;
};

template<std::derived_from<Mustard::CLI::ModuleBase>... AExtraModules>
using MatrixElementBasedGeneratorCLI = EventGeneratorCLI<MatrixElementBasedGeneratorCLIModule,
                                                         AExtraModules...>;

} // namespace MACE::inline Utility

#include "MACE/Utility/MatrixElementBasedGeneratorCLI.inl"
