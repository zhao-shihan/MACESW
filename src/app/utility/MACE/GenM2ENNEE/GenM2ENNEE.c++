#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/GenM2ENNEE/GenM2ENNEE.h++"
#include "MACE/Utility/InitialStateCLIModule.h++"
#include "MACE/Utility/MCMCGeneratorCLI.h++"
#include "MACE/Utility/WriteAutocorrelationFunction.h++"

#include "Mustard/CLHEPX/Random/Xoshiro.h++"
#include "Mustard/Data/GeneratedEvent.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Execution/Executor.h++"
#include "Mustard/IO/File.h++"
#include "Mustard/IO/Print.h++"
#include "Mustard/Physics/Generator/M2ENNEEGenerator.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "CLHEP/Random/Random.h"

#include "muc/numeric"
#include "muc/utility"

#include <cmath>
#include <string>
#include <type_traits>

namespace MACE::GenM2ENNEE {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::string_literals;

GenM2ENNEE::GenM2ENNEE() :
    Subprogram{"GenM2ENNEE", "Generate internal conversion muon decay (mu -> e nu nu e e)."} {}

auto GenM2ENNEE::Main(int argc, char* argv[]) const -> int {
    MCMCGeneratorCLI<InitialStateCLIModule<"polarized", "muon">> cli;
    cli.DefaultOutput("m2ennee.root");
    cli.DefaultOutputTree("m2ennee");
    auto& biasCLI{cli->add_mutually_exclusive_group()};
    biasCLI.add_argument("--mace-bias").help("Enable MACE detector signal region importance sampling.").flag();
    biasCLI.add_argument("--ep-ek-bias").help("Apply soft upper bound for atomic positron kinetic energy.").flag();
    biasCLI.add_argument("--emiss-bias").help("Apply soft upper bound for missing energy.").flag();
    cli->add_argument("--pxy-softening-factor").help("Softening factor for transverse momentum soft cut in --mace-bias.").default_value(0.25_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--cos-theta-softening-factor").help("Softening factor for momentum cosine soft cut in --mace-bias.").default_value(0.025).required().nargs(1).scan<'g', double>();
    cli->add_argument("--ep-ek-soft-upper-bound").help("Soft upper bound for atomic positron kinetic energy in --ep-ek-bias or --mace-bias.").default_value(0_eV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--ep-ek-softening-factor").help("Softening factor for atomic positron kinetic energy upper bound in --ep-ek-bias or --mace-bias.").default_value(1_keV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--emiss-soft-upper-bound").help("Soft upper bound for missing energy in --emiss-bias.").default_value(0_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--emiss-softening-factor").help("Softening factor for missing energy upper bound in --emiss-bias.").default_value(1_MeV).required().nargs(1).scan<'g', double>();
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<256> random{cli};

    Mustard::M2ENNEEGenerator generator("mu+", cli.Momentum(), cli.Polarization(),
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
            // .         e+ n   n   e-  e+
            const auto& [p0, _1, _2, p3, p4]{momenta};
            const auto p3Seen{scPxy(p3.perp()) > scPxy(outPxyCut) and scCos(muc::abs(p3.cosTheta())) < scCos(cosCut)};
            const auto p0Miss{scPxy(p0.perp()) < scPxy(inPxyCut) or scCos(muc::abs(p0.cosTheta())) > scCos(cosCut)};
            const auto p4Miss{scPxy(p4.perp()) < scPxy(inPxyCut) or scCos(muc::abs(p4.cosTheta())) > scCos(cosCut)};
            const auto p0Low{scEk(p0.e() - electron_mass_c2) < scEk(epEkCut)};
            const auto p4Low{scEk(p4.e() - electron_mass_c2) < scEk(epEkCut)};
            return p3Seen and ((p0Miss and p4Low) or (p4Miss and p0Low));
        });
    } else if (cli["--ep-ek-bias"] == true) {
        generator.Acceptance([epEkCut = cli->get<double>("--ep-ek-soft-upper-bound"),
                              scEk = muc::soft_cmp{cli->get<double>("--ep-ek-softening-factor")}](auto&& p) {
            const auto epEk{p[0].e() - electron_mass_c2};
            return scEk(epEk) < scEk(epEkCut);
        });
    } else if (cli["--emiss-bias"] == true) {
        generator.Acceptance([eMissCut = cli->get<double>("--emiss-soft-upper-bound"),
                              scEMiss = muc::soft_cmp{cli->get<double>("--emiss-softening-factor")}](auto&& momenta) {
            // .         e+ n   n   e-  e+
            const auto& [p0, _1, _2, p3, p4]{momenta};
            const auto eMiss{muon_mass_c2 - (p0.e() + p3.e() + p4.e())};
            return scEMiss(eMiss) < scEMiss(eMissCut);
        });
    }

    // Integrate matrix element
    Mustard::Executor<unsigned long long> executor{"Generation", "Sample"};
    const auto [phaseSpaceIntegral, nEff, integrationState]{cli.PhaseSpaceIntegral(executor, generator)};
    const auto width{muc::pow(2 * pi, 4) / (2 * muon_mass_c2) * phaseSpaceIntegral};
    const auto branchingRatio{width * (muon_lifetime / hbar_Planck)};
    Mustard::MasterPrint("Branching ratio:\n"
                         "  {} +/- {}  (rel. unc.: {:.3}%, N_eff: {:.2f})\n"
                         "\n",
                         branchingRatio.value, branchingRatio.uncertainty,
                         branchingRatio.uncertainty / branchingRatio.value * 100, nEff);

    // Initialize generator and write ACF
    Mustard::File<TFile> file{cli->get("--output"), cli->get("--output-mode")};
    auto& rng{*CLHEP::HepRandom::getTheEngine()};
    const auto autocorrelationFunction{generator.MCMCInitialize(rng)};
    WriteAutocorrelationFunction(autocorrelationFunction);

    // Return if nothing to be generated
    const auto nEvent{cli.GenerateOrExit()};
    if (not nEvent.has_value()) {
        return EXIT_SUCCESS;
    }

    // Generate events
    Mustard::Data::Output<Mustard::Data::GeneratedKinematics> writer{cli->get("--output-tree")};
    executor(*nEvent, [&](auto) {
        const auto [weight, pdgID, p]{generator(rng)};
        Mustard::Data::Tuple<Mustard::Data::GeneratedKinematics> event;
        // 0: e+, 3: e-, 4: e+
        Get<"pdgID">(event) = {pdgID[0], pdgID[3], pdgID[4]};
        Get<"E">(event) = {float(p[0].e()), float(p[3].e()), float(p[4].e())};
        Get<"px">(event) = {float(p[0].x()), float(p[3].x()), float(p[4].x())};
        Get<"py">(event) = {float(p[0].y()), float(p[3].y()), float(p[4].y())};
        Get<"pz">(event) = {float(p[0].z()), float(p[3].z()), float(p[4].z())};
        Get<"w">(event) = branchingRatio.value * weight;
        writer.Fill(event);
    });
    executor.PrintExecutionSummary();
    writer.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::GenM2ENNEE
