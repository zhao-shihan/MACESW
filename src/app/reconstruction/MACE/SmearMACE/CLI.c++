#include "MACE/SmearMACE/CLI.h++"

#include "Mustard/IO/PrettyLog.h++"

#include "muc/math"

#include "gsl/gsl"

#include "fmt/core.h"

#include <cstdlib>

namespace MACE::SmearMACE {

CLIModule::CLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli) :
    ModuleBase{cli} {
    TheCLI()
        ->add_argument("input")
        .nargs(argparse::nargs_pattern::at_least_one)
        .help("Input file path(s).");
    TheCLI()
        ->add_argument("-o", "--output")
        .help("Output file path. Suffix '_smeared' on input file name by default.");
    TheCLI()
        ->add_argument("-m", "--output-mode")
        .help("Output file creation mode. Default to 'NEW'.");

    TheCLI()
        ->add_argument("-i", "--index-range")
        .nargs(1, 2)
        .scan<'i', gsl::index>()
        .default_value(std::vector<gsl::index>{0, 1})
        .help("Set number of datasets (index in [0, size) range), or index range (in [first, last) pattern)");

    auto& cdcHitMutexGroup{TheCLI()->add_mutually_exclusive_group()};
    cdcHitMutexGroup
        .add_argument("--cdc-hit")
        .nargs(2)
        .append()
        .help("Smear a simulated CDC hit variable by a smearing expression (e.g. --cdc-hit d 'gRandom->Gaus(d, 0.2*sqrt(d/5))').");
    cdcHitMutexGroup
        .add_argument("--cdc-hit-id")
        .flag()
        .help("Save CDC hit data in output file without smearing.");
    TheCLI()
        ->add_argument("--cdc-hit-name")
        .help("Set dataset name format. Default to 'G4Run{}/CDCSimHit'.");

    auto& ttcHitMutexGroup{TheCLI()->add_mutually_exclusive_group()};
    ttcHitMutexGroup
        .add_argument("--ttc-hit")
        .nargs(2)
        .append()
        .help("Smear a simulated TTC hit variable by a smearing expression (e.g. --ttc-hit t 'gRandom->Gaus(t, 0.05)').");
    ttcHitMutexGroup
        .add_argument("--ttc-hit-id")
        .flag()
        .help("Save TTC hit data in output file without smearing.");
    TheCLI()
        ->add_argument("--ttc-hit-name")
        .help("Set dataset name format. Default to 'G4Run{}/TTCSimHit'.");

    auto& mmsTrackMutexGroup{TheCLI()->add_mutually_exclusive_group()};
    mmsTrackMutexGroup
        .add_argument("--mms-track")
        .nargs(2)
        .append()
        .help("Smear a simulated CDC track variable by a smearing expression (e.g. --mms-track Ek 'gRandom->Gaus(Ek, 1)').");
    mmsTrackMutexGroup
        .add_argument("--mms-track-id")
        .flag()
        .help("Save CDC track data in output file without smearing.");
    TheCLI()
        ->add_argument("--mms-track-name")
        .help("Set dataset name format. Default to 'G4Run{}/MMSSimTrack'.");

    auto& mcpHitMutexGroup{TheCLI()->add_mutually_exclusive_group()};
    mcpHitMutexGroup
        .add_argument("--mcp-hit")
        .nargs(2)
        .append()
        .help("Smear a simulated MCP hit variable by a smearing expression (e.g. --mcp-hit t 'gRandom->Gaus(t, 0.5)').");
    mcpHitMutexGroup
        .add_argument("--mcp-hit-id")
        .flag()
        .help("Save MCP hit data in output file without smearing.");
    TheCLI()
        ->add_argument("--mcp-hit-name")
        .help("Set dataset name format. Default to 'G4Run{}/MCPSimHit'.");

    auto& ecalHitMutexGroup{TheCLI()->add_mutually_exclusive_group()};
    ecalHitMutexGroup
        .add_argument("--ecal-hit")
        .nargs(2)
        .append()
        .help("Smear a simulated ECAL hit variable by a smearing expression (e.g. --ecal-hit Edep 'gRandom->Gaus(Edep, 0.041*sqrt(E/0.511))').");
    ecalHitMutexGroup
        .add_argument("--ecal-hit-id")
        .flag()
        .help("Save ECAL hit data in output file without smearing.");
    TheCLI()
        ->add_argument("--ecal-hit-name")
        .help("Set dataset name format. Default to 'G4Run{}/ECALSimHit'.");
}

auto CLIModule::DatasetIndexRange() const -> std::pair<gsl::index, gsl::index> {
    const auto var{TheCLI()->get<std::vector<gsl::index>>("-i")};
    Expects(var.size() == 1 or var.size() == 2);
    if (var.size() == 1) {
        return {0, var.front()};
    }
    return {var.front(), var.back()};
}

auto CLIModule::OutputFilePath() const -> std::filesystem::path {
    if (auto output{TheCLI()->present("-o")}) {
        return *std::move(output);
    }
    auto inputList{InputFilePath()};
    if (inputList.size() > 1) {
        Mustard::PrintError("Cannot automatically construct output file path since # input file path > 1. Use -o or --output");
        std::exit(EXIT_FAILURE);
    }
    if (inputList.front().find('*') != std::string::npos) {
        Mustard::PrintError("Cannot automatically construct output file path since input file path includes wildcards. Use -o or --output");
        std::exit(EXIT_FAILURE);
    }
    std::filesystem::path input{std::move(inputList.front())};
    const auto extension{input.extension()};
    return input.replace_extension().concat("_smeared").replace_extension(extension);
}

auto CLIModule::ParseSmearingConfig(std::string_view arg) const -> muc::flat_hash_map<std::string, std::string> {
    auto var{TheCLI()->present<std::vector<std::string>>(arg)};
    if (not var.has_value()) {
        return {};
    }
    Ensures(muc::even(var->size()));
    muc::flat_hash_map<std::string, std::string> config;
    for (gsl::index i{}; i < ssize(*var); i += 2) {
        auto [_, inserted]{config.try_emplace(std::move(var->at(i)), std::move(var->at(i + 1)))};
        if (not inserted) {
            Mustard::PrintError(fmt::format("Duplicate variable '{}'", var->at(i)));
            std::exit(EXIT_FAILURE);
        }
    }
    return config;
}

} // namespace MACE::SmearMACE
