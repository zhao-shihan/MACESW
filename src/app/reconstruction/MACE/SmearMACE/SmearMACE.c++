#include "MACE/Data/MMSTrack.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/SmearMACE/CLI.h++"
#include "MACE/SmearMACE/SmearMACE.h++"
#include "MACE/SmearMACE/Smearer.h++"

#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/ROOTX/MakeTextTMacro.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "TFile.h"
#include "TInterpreter.h"
#include "TMacro.h"

#include "mplr/mplr.hpp"

#include "gsl/gsl"

#include "fmt/format.h"

#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace MACE::SmearMACE {

SmearMACE::SmearMACE() :
    Subprogram{"SmearMACE", "Add resolution effect to simulation data."} {}

auto SmearMACE::Main(int argc, char* argv[]) const -> int {
    CLI cli;
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<256> random{cli};
    gInterpreter->ProcessLine(R"(
        const auto Gauss{
            [](auto mu, auto sigma) {
                return gRandom->Gaus(mu, sigma);
            }};
    )");

    const auto outputPath{Mustard::Parallel::ProcessSpecificPath(cli.OutputFilePath()).replace_extension(".root").generic_string()};
    TFile file{outputPath.c_str(), cli.OutputFileMode().c_str(), "", ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose};
    if (not file.IsOpen()) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot open file '{}' with mode '{}'", outputPath, cli.OutputFileMode()));
    }
    do {
        if (mplr::comm_world().rank() != 0) {
            break;
        }
        std::stringstream smearingConfigText;
        const auto AppendConfigText{
            [&](const auto& nameInConfigText, const auto& smearingConfig, const auto& identity) {
                if (smearingConfig.empty() and not identity) {
                    return;
                }
                fmt::print(smearingConfigText, "{}:\n", nameInConfigText);
                if (not smearingConfig.empty()) {
                    for (auto&& [var, smear] : smearingConfig) {
                        fmt::print(smearingConfigText, "  {}: {}\n", var, smear);
                    }
                }
            }};
        AppendConfigText("CDCSimHit", cli.CDCSimHitSmearingConfig(), cli.CDCSimHitIdentity());
        AppendConfigText("TTCSimHit", cli.TTCSimHitSmearingConfig(), cli.TTCSimHitIdentity());
        AppendConfigText("MMSSimTrack", cli.MMSSimTrackSmearingConfig(), cli.MMSSimTrackIdentity());
        AppendConfigText("MCPSimHit", cli.MCPSimHitSmearingConfig(), cli.MCPSimHitIdentity());
        AppendConfigText("ECALSimHit", cli.ECALSimHitSmearingConfig(), cli.ECALSimHitIdentity());
        Mustard::ROOTX::MakeTextTMacro(smearingConfigText.str(), "SmearingConfig", "Print SmearMACE smearing configuration")->Write();
    } while (false);
    {
        Mustard::Data::Processor<> processor;

        Smearer smearer{cli.InputFilePath(), processor};
        const auto [iFirst, iLast]{cli.DatasetIndexRange()};
        const auto Smear{
            [&, iFirst = iFirst, iLast = iLast]<
                typename... Ts>(std::type_identity<Ts...>, const auto& nameFmt, const auto& smearingConfig, const auto& identity) {
                if (not smearingConfig.empty() or identity) {
                    for (auto i{iFirst}; i < iLast; ++i) {
                        smearer.Smear<Ts...>(fmt::vformat(nameFmt, fmt::make_format_args(i)), smearingConfig);
                    }
                }
            }};

        Smear(std::type_identity<Data::CDCSimHit>{}, cli.CDCSimHitNameFormat(), cli.CDCSimHitSmearingConfig(), cli.CDCSimHitIdentity());
        Smear(std::type_identity<Data::TTCSimHit>{}, cli.TTCSimHitNameFormat(), cli.TTCSimHitSmearingConfig(), cli.TTCSimHitIdentity());
        Smear(std::type_identity<Data::MMSSimTrack>{}, cli.MMSSimTrackNameFormat(), cli.MMSSimTrackSmearingConfig(), cli.MMSSimTrackIdentity());
        Smear(std::type_identity<Data::MCPSimHit>{}, cli.MCPSimHitNameFormat(), cli.MCPSimHitSmearingConfig(), cli.MCPSimHitIdentity());
        Smear(std::type_identity<Data::ECALSimHit>{}, cli.ECALSimHitNameFormat(), cli.ECALSimHitSmearingConfig(), cli.ECALSimHitIdentity());
    }

    return EXIT_SUCCESS;
}

} // namespace MACE::SmearMACE
