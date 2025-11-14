#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/GenBkgM2ENNE/GenBkgM2ENNE.h++"
#include "MACE/Utility/InitialStateCLIModule.h++"
#include "MACE/Utility/MCMCGeneratorCLI.h++"
#include "MACE/Utility/WriteAutocorrelationFunction.h++"

#include "Mustard/Data/GeneratedEvent.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Execution/Executor.h++"
#include "Mustard/IO/File.h++"
#include "Mustard/IO/Print.h++"
#include "Mustard/Physics/Generator/M2ENNEGenerator.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "CLHEP/Random/Random.h"

#include "muc/numeric"
#include "muc/utility"

namespace MACE::GenBkgM2ENNE {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::string_literals;

GenBkgM2ENNE::GenBkgM2ENNE() :
    Subprogram{"GenBkgM2ENNE", "Generate muonium decay with hard photon exchange (M -> e+ nu nu e-)."} {}

auto GenBkgM2ENNE::Main(int argc, char* argv[]) const -> int {
    MCMCGeneratorCLI<InitialStateCLIModule<"unpolarized", "muonium">> cli;
    cli.DefaultOutput("m2enne_bkg.root");
    cli.DefaultOutputTree("m2enne");
    auto& biasCLI{cli->add_mutually_exclusive_group()};
    biasCLI.add_argument("--mace-bias").help("Enable MACE detector signal region importance sampling.").flag();
    biasCLI.add_argument("--ep-ek-bias").help("Apply soft upper bound for positron kinetic energy.").flag();
    cli->add_argument("--pxy-softening-factor").help("Softening factor for transverse momentum soft cut in --mace-bias.").default_value(0.25_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--cos-theta-softening-factor").help("Softening factor for momentum cosine soft cut in --mace-bias.").default_value(0.025).required().nargs(1).scan<'g', double>();
    cli->add_argument("--ep-ek-soft-upper-bound").help("Soft upper bound for positron kinetic energy in --ep-ek-bias or --mace-bias.").default_value(0_eV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--ep-ek-softening-factor").help("Softening factor for positron kinetic energy upper bound in --ep-ek-bias or --mace-bias.").default_value(1_keV).required().nargs(1).scan<'g', double>();
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<256> random{cli};

    Mustard::M2ENNEGenerator generator("muonium", cli.Momentum(),
                                       cli->present<double>("--thinning-ratio"), cli->present<unsigned>("--acf-sample-size"));

    if (cli["--mace-bias"] == true) {
        const auto& cdc{Detector::Description::CDC::Instance()};
        const auto& ttc{Detector::Description::TTC::Instance()};
        const auto mmsB{Detector::Description::MMSField::Instance().FastField()};
        generator.Acceptance([inPxyCut = (cdc.GasInnerRadius() / 2) * mmsB * c_light,
                              outPxyCut = (ttc.Radius() / 2) * mmsB * c_light,
                              cosCut = 1 / muc::hypot(2 * cdc.GasOuterRadius() / cdc.GasOuterLength(), 1.),
                              epEkCut = cli->get<double>("--ep-ek-soft-upper-bound"),
                              scPxy = muc::soft_cmp{cli->get<double>("--pxy-softening-factor")},
                              scCos = muc::soft_cmp{cli->get<double>("--cos-theta-softening-factor")},
                              scEk = muc::soft_cmp{cli->get<double>("--ep-ek-softening-factor")}](auto&& momenta) {
            // .         e+ n   n   e-
            const auto& [p0, _1, _2, p3]{momenta};
            const auto p0Low{scEk(p0.e() - electron_mass_c2) < scEk(epEkCut)};
            const auto p3Seen{scPxy(p3.perp()) > scPxy(outPxyCut) and scCos(muc::abs(p3.cosTheta())) < scCos(cosCut)};
            return p0Low and p3Seen;
        });
    } else if (cli["--ep-ek-bias"] == true) {
        generator.Acceptance([epEkCut = cli->get<double>("--ep-ek-soft-upper-bound"),
                              scEk = muc::soft_cmp{cli->get<double>("--ep-ek-softening-factor")}](auto&& p) {
            const auto epEk{p[0].e() - electron_mass_c2};
            return scEk(epEk) < scEk(epEkCut);
        });
    }

    // Integrate matrix element
    Mustard::Executor<unsigned long long> executor{"Generation", "Sample"};
    const auto [phaseSpaceIntegral, nEff, integrationState]{cli.PhaseSpaceIntegral(executor, generator)};
    const auto width{muc::pow(2 * pi, 4) / (2 * muonium_mass_c2) * phaseSpaceIntegral};
    const auto branchingRatio{width * (muonium_lifetime / hbar_Planck)};
    Mustard::MasterPrint("Branching ratio:\n"
                         "  {} +/- {}  (rel. unc.: {:.3}%, N_eff: {:.2f})\n"
                         "\n",
                         branchingRatio.value, branchingRatio.uncertainty,
                         branchingRatio.uncertainty / branchingRatio.value * 100, nEff);

    // Return if nothing to be generated
    const auto nEvent{cli.GenerateOrExit()};
    if (not nEvent.has_value()) {
        return EXIT_SUCCESS;
    }

    // Initialize generator and write ACF
    Mustard::ProcessSpecificFile<TFile> file{cli->get("--output"), cli->get("--output-mode")};
    auto& rng{*CLHEP::HepRandom::getTheEngine()};
    const auto autocorrelationFunction{generator.MCMCInitialize(rng)};
    WriteAutocorrelationFunction(autocorrelationFunction);

    // Generate events
    if (*nEvent == 0) {
        return EXIT_SUCCESS;
    }
    Mustard::Data::Output<Mustard::Data::GeneratedKinematics> writer{cli->get("--output-tree")};
    executor(*nEvent, [&](auto) {
        const auto [weight, pdgID, p]{generator(rng)};
        Mustard::Data::Tuple<Mustard::Data::GeneratedKinematics> event;
        // 0: e+, 3: e-
        Get<"pdgID">(event) = {pdgID[0], pdgID[3]};
        Get<"E">(event) = {static_cast<float>(p[0].e()), static_cast<float>(p[3].e())};
        Get<"px">(event) = {static_cast<float>(p[0].x()), static_cast<float>(p[3].x())};
        Get<"py">(event) = {static_cast<float>(p[0].y()), static_cast<float>(p[3].y())};
        Get<"pz">(event) = {static_cast<float>(p[0].z()), static_cast<float>(p[3].z())};
        Get<"w">(event) = branchingRatio.value * weight;
        writer.Fill(event);
    });
    executor.PrintExecutionSummary();
    writer.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::GenBkgM2ENNE
