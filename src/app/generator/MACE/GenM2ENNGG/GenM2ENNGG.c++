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

#include "MACE/GenM2ENNGG/GenM2ENNGG.h++"
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
#include "Mustard/Physics/Generator/M2ENNGGGenerator.h++"
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

namespace MACE::GenM2ENNGG {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::string_literals;

GenM2ENNGG::GenM2ENNGG() :
    Subprogram{"GenM2ENNGG", "Generate double radiative muon decay (mu+ -> e+ nu nu gamma gamma)."} {}

auto GenM2ENNGG::Main(int argc, char* argv[]) const -> int {
    MCMCGeneratorCLI<InitialStateCLIModule<"polarized", "muon">> cli;
    cli.DefaultOutput("m2enngg.root");
    cli.DefaultOutputTree("m2enngg");
    cli->add_argument("--ir-cut").help("IR cut for final-state photons.").default_value(electron_mass_c2).required().nargs(1).scan<'g', double>();
    auto& biasCLI{cli->add_mutually_exclusive_group()};
    biasCLI.add_argument("--emiss-bias").help("Apply soft upper bound for missing energy.").flag();
    cli->add_argument("--emiss-soft-upper-bound").help("Soft upper bound for missing energy in --emiss-bias.").default_value(0_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("--emiss-softening-factor").help("Softening factor for missing energy upper bound in --emiss-bias.").default_value(1_MeV).required().nargs(1).scan<'g', double>();
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<256> random{cli};

    Mustard::M2ENNGGGenerator generator("mu+", cli.Momentum(), cli.Polarization(), cli->get<double>("--ir-cut"),
                                        cli->present<double>("--thinning-ratio"), cli->present<unsigned>("--acf-sample-size"));

    if (cli["--emiss-bias"] == true) {
        generator.Acceptance([eMissCut = cli->get<double>("--emiss-soft-upper-bound"),
                              scEMiss = muc::soft_cmp{cli->get<double>("--emiss-softening-factor")}](auto&& momenta) {
            //.          e+  n   n   g   g
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
        // 0: e+, 3: g, 4: g
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

} // namespace MACE::GenM2ENNGG
