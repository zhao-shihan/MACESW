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

#include "MACE/GenM2ENNG/GenM2ENNG.h++"
#include "MACE/Generator/InitialStateCLIModule.h++"
#include "MACE/Generator/MCMCGeneratorCLI.h++"
#include "MACE/Generator/WriteAutocorrelationFunction.h++"

#include "Mustard/Data/GeneratedEvent.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Execution/Executor.h++"
#include "Mustard/IO/File.h++"
#include "Mustard/IO/Print.h++"
#include "Mustard/Physics/Generator/M2ENNGGenerator.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "CLHEP/Random/Random.h"

#include "muc/numeric"
#include "muc/utility"

#include <string>

namespace MACE::GenM2ENNG {

using namespace Mustard::LiteralUnit::Angle;
using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::string_literals;

GenM2ENNG::GenM2ENNG() :
    Subprogram{"GenM2ENNG", "Generate radiative muon decay (mu+ -> e+ nu_e nu_mu gamma)."} {}

auto GenM2ENNG::Main(int argc, char* argv[]) const -> int {
    Generator::MCMCGeneratorCLI<Generator::InitialStateCLIModule<"polarized", "muon">> cli;
    cli.DefaultOutput("m2enng.root");
    cli.DefaultOutputTree("m2enng");
    cli.AddMCMCStepSizeOption();
    cli->add_argument("--soft-cutoff").help("Low-energy cutoff in c.m. frame for final-state photons.").default_value(5_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--collinear-cutoff").help("Collinear cutoff in c.m. frame for final-state photons.").default_value(0.1_rad).required().nargs(1).scan<'g', double>();
    auto& biasCLI{cli->add_mutually_exclusive_group()};
    biasCLI.add_argument("--emiss-bias").help("Apply soft upper bound for missing energy.").flag();
    cli->add_argument("--emiss-soft-upper-bound").help("Soft upper bound for missing energy in --emiss-bias.").default_value(0_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--emiss-softening-scale").help("Softening scale for missing energy upper bound in --emiss-bias.").default_value(1_MeV).required().nargs(1).scan<'g', double>();
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<256> random{cli};

    Mustard::M2ENNGGenerator generator("mu+", cli.Momentum(), cli.Polarization(),
                                       cli->get<double>("--soft-cutoff"), cli->get<double>("--collinear-cutoff"),
                                       cli->present<double>("--thinning-ratio"), cli->present<unsigned>("--acf-sample-size"),
                                       cli->present<double>("--mcmc-step-size"));

    if (cli["--emiss-bias"] == true) {
        generator.Acceptance([eMissCut = cli->get<double>("--emiss-soft-upper-bound"),
                              scEMiss = muc::soft_cmp{cli->get<double>("--emiss-softening-scale")}](auto&& momenta) {
            //.          e+  νe  νμ  γ
            const auto& [q1, _2, _3, q4]{momenta};
            const auto eMiss{muon_mass_c2 - (q1.e() + q4.e())};
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
        // Index: 0: e+, 1: νe, 2: νμ, 3: γ
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

} // namespace MACE::GenM2ENNG
