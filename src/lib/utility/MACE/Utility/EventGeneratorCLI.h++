#pragma once

#include "Mustard/CLI/Module/ModuleBase.h++"
#include "Mustard/CLI/MonteCarloCLI.h++"

#include <optional>
#include <string>

namespace MACE::inline Utility {

class EventGeneratorCLIModule : public Mustard::CLI::ModuleBase {
public:
    explicit EventGeneratorCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli);

    auto DefaultOutput(std::string path) -> void;
    auto DefaultOutputTree(std::string name) -> void;

    auto GenerateOrExit() const -> std::optional<unsigned long long>;
};

template<std::derived_from<Mustard::CLI::ModuleBase>... AExtraModules>
using EventGeneratorCLI = Mustard::CLI::MonteCarloCLI<EventGeneratorCLIModule,
                                                      AExtraModules...>;

} // namespace MACE::inline Utility
