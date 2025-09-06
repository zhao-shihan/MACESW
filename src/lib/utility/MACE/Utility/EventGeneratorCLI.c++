#include "MACE/Utility/EventGeneratorCLI.h++"

#include "Mustard/CLI/CLI.h++"
#include "Mustard/IO/Print.h++"

#include <cstdlib>

namespace MACE::inline Utility {

EventGeneratorCLIModule::EventGeneratorCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli) :
    ModuleBase{cli} {
    using namespace std::string_literals;
    TheCLI()
        ->add_argument("-g", "--generate")
        .help("Number of events to generate. Program will skip event generation if not set.")
        .nargs(1)
        .scan<'i', unsigned long long>();
    TheCLI()
        ->add_argument("-o", "--output")
        .help("Output file path.")
        .required()
        .nargs(1);
    TheCLI()
        ->add_argument("-m", "--output-mode")
        .help("Output file creation mode (see ROOT documentation for details).")
        .default_value("NEW"s)
        .required()
        .nargs(1);
    TheCLI()
        ->add_argument("-t", "--output-tree")
        .help("Output tree name.")
        .required()
        .nargs(1);
}

auto EventGeneratorCLIModule::DefaultOutput(std::string path) -> void {
    TheCLI()["--output"].default_value(std::move(path));
}

auto EventGeneratorCLIModule::DefaultOutputTree(std::string name) -> void {
    TheCLI()["--output-tree"].default_value(std::move(name));
}

auto EventGeneratorCLIModule::GenerateOrExit() const -> std::optional<unsigned long long> {
    const auto nGenerate{TheCLI()->present<unsigned long long>("--generate")};
    if (not nGenerate) {
        Mustard::MasterPrintLn("Option -g or --generate not set, skipping event generation.");
    } else {
        Mustard::MasterPrintLn("Generating {} events...", *nGenerate);
    }
    return nGenerate;
}

} // namespace MACE::inline Utility
