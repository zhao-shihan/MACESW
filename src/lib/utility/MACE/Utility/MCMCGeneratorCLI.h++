#pragma once

#include "MACE/Utility/MatrixElementBasedGeneratorCLI.h++"

#include "Mustard/CLI/Module/ModuleBase.h++"

namespace MACE::inline Utility {

class MCMCGeneratorCLIModule : public Mustard::CLI::ModuleBase {
public:
    MCMCGeneratorCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli);
};

template<std::derived_from<Mustard::CLI::ModuleBase>... AExtraModules>
using MCMCGeneratorCLI = MatrixElementBasedGeneratorCLI<MCMCGeneratorCLIModule,
                                                        AExtraModules...>;

} // namespace MACE::inline Utility
