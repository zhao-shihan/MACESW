// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/GenM2ENNEE/GenM2ENNEE.h++"
#include "MACE/Generator/InitialStateCLIModule.h++"
#include "MACE/Generator/MCMCGeneratorCLI.h++"
#include "MACE/Generator/WriteAutocorrelationFunction.h++"

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

#include <string>

namespace MACE::GenM2ENNEE {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::string_literals;

GenM2ENNEE::GenM2ENNEE() :
    Subprogram{"GenM2ENNEE", "Generate internal conversion muon decay (mu+ -> e+ nu_e nu_mu e- e+)."} {}

auto GenM2ENNEE::Main(int argc, char* argv[]) const -> int {
    Generator::MCMCGeneratorCLI<Generator::InitialStateCLIModule<"polarized", "muon">> cli;
    cli.DefaultOutput("m2ennee.root");
    cli.DefaultOutputTree("m2ennee");
    cli.AddMCMCStepSizeOption();
    auto& biasCLI{cli->add_mutually_exclusive_group()};
    biasCLI.add_argument("--mace-bias").help("Enable MACE detector signal region importance sampling.").flag();
    biasCLI.add_argument("--ep-ek-bias").help("Apply soft upper bound for positron kinetic energy.").flag();
    biasCLI.add_argument("--emiss-bias").help("Apply soft upper bound for missing energy.").flag();
    cli->add_argument("--pxy-softening-scale").help("Softening scale for transverse momentum soft cut in --mace-bias.").default_value(0.25_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--cos-theta-softening-scale").help("Softening scale for momentum cosine soft cut in --mace-bias.").default_value(0.025).required().nargs(1).scan<'g', double>();
    cli->add_argument("--ep-ek-soft-upper-bound").help("Soft upper bound for positron kinetic energy in --ep-ek-bias or --mace-bias.").default_value(0_eV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--ep-ek-softening-scale").help("Softening scale for positron kinetic energy upper bound in --ep-ek-bias or --mace-bias.").default_value(1_keV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--emiss-soft-upper-bound").help("Soft upper bound for missing energy in --emiss-bias.").default_value(0_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--emiss-softening-scale").help("Softening scale for missing energy upper bound in --emiss-bias.").default_value(1_MeV).required().nargs(1).scan<'g', double>();
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<256> random{cli};

    Mustard::M2ENNEEGenerator generator("mu+", cli.Momentum(), cli.Polarization(),
                                        cli->present<double>("--thinning-ratio"), cli->present<unsigned>("--acf-sample-size"),
                                        cli->present<double>("--mcmc-step-size"));

    if (cli["--mace-bias"] == true) {
        const auto& cdc{Detector::Description::CDC::Instance()};
        const auto& ttc{Detector::Description::TTC::Instance()};
        const auto mmsB{Detector::Description::MMSField::Instance().NominalField()};
        generator.Acceptance([inPxyCut = (cdc.GasInnerRadius() / 2) * mmsB * c_light,
                              outPxyCut = (ttc.Radius() / 2) * mmsB * c_light,
                              cosCut = 1 / muc::hypot(2 * cdc.GasOuterRadius() / cdc.GasOuterLength(), 1.),
                              epEkCut = cli->get<double>("--ep-ek-soft-upper-bound"),
                              scPxy = muc::soft_cmp{cli->get<double>("--pxy-softening-scale")},
                              scCos = muc::soft_cmp{cli->get<double>("--cos-theta-softening-scale")},
                              scEk = muc::soft_cmp{cli->get<double>("--ep-ek-softening-scale")}](auto&& momenta) {
            //.          e+  νe νμ e-  e+
            const auto& [q1, _1, _2, q4, q5]{momenta};
            const auto emFast{scPxy(q4.perp()) > scPxy(outPxyCut) and scCos(muc::abs(q4.cosTheta())) < scCos(cosCut)};
            const auto ep1Miss{scPxy(q1.perp()) < scPxy(inPxyCut) or scCos(muc::abs(q1.cosTheta())) > scCos(cosCut)};
            const auto ep2Miss{scPxy(q5.perp()) < scPxy(inPxyCut) or scCos(muc::abs(q5.cosTheta())) > scCos(cosCut)};
            const auto ep1Low{scEk(q1.e() - electron_mass_c2) < scEk(epEkCut)};
            const auto ep2Low{scEk(q5.e() - electron_mass_c2) < scEk(epEkCut)};
            return emFast and ((ep1Miss and ep2Low) or (ep2Miss and ep1Low));
        });
    } else if (cli["--ep-ek-bias"] == true) {
        generator.Acceptance([epEkCut = cli->get<double>("--ep-ek-soft-upper-bound"),
                              scEk = muc::soft_cmp{cli->get<double>("--ep-ek-softening-scale")}](auto&& p) {
            const auto epEk{p[0].e() - electron_mass_c2};
            return scEk(epEk) < scEk(epEkCut);
        });
    } else if (cli["--emiss-bias"] == true) {
        generator.Acceptance([eMissCut = cli->get<double>("--emiss-soft-upper-bound"),
                              scEMiss = muc::soft_cmp{cli->get<double>("--emiss-softening-scale")}](auto&& momenta) {
            //.          e+  νe  νμ  e-  e+
            const auto& [q1, _1, _2, q4, q5]{momenta};
            const auto eMiss{muon_mass_c2 - (q1.e() + q4.e() + q5.e())};
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

    // Return if nothing to be generated
    const auto nEvent{cli.NEvent()};
    if (nEvent == 0) {
        return EXIT_SUCCESS;
    }

    // Initialize generator and write ACF
    Mustard::ProcessSpecificFile<TFile> file{cli->get("--output"), cli->get("--output-mode")};
    auto& rng{*CLHEP::HepRandom::getTheEngine()};
    const auto autocorrelationFunction{generator.MCMCInitialize(rng)};
    Generator::WriteAutocorrelationFunction(autocorrelationFunction);

    // Generate events
    Mustard::Data::Output<Mustard::Data::GeneratedKinematics> writer{cli->get("--output-tree")};
    executor(nEvent, [&](auto) {
        const auto [weight, pdgID, p]{generator(rng)};
        Mustard::Data::Tuple<Mustard::Data::GeneratedKinematics> event;
        // Index: 0: e+, 1: νe, 2: νμ, 3: e-, 4: e+
        Get<"pdgID">(event) = {pdgID[0], pdgID[3], pdgID[4]};
        Get<"E">(event) = {static_cast<float>(p[0].e()), static_cast<float>(p[3].e()), static_cast<float>(p[4].e())};
        Get<"px">(event) = {static_cast<float>(p[0].x()), static_cast<float>(p[3].x()), static_cast<float>(p[4].x())};
        Get<"py">(event) = {static_cast<float>(p[0].y()), static_cast<float>(p[3].y()), static_cast<float>(p[4].y())};
        Get<"pz">(event) = {static_cast<float>(p[0].z()), static_cast<float>(p[3].z()), static_cast<float>(p[4].z())};
        Get<"w">(event) = branchingRatio.value * weight;
        writer.Fill(event);
    });
    executor.PrintExecutionSummary();
    writer.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::GenM2ENNEE
