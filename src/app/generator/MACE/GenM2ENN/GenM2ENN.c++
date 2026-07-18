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

#include "MACE/GenM2ENN/GenM2ENN.h++"
#include "MACE/Generator/InitialStateCLIModule.h++"
#include "MACE/Generator/MCMCGeneratorCLI.h++"
#include "MACE/Generator/WriteAutocorrelationFunction.h++"

#include "Mustard/Data/GeneratedEvent.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Execution/Executor.h++"
#include "Mustard/IO/File.h++"
#include "Mustard/IO/Print.h++"
#include "Mustard/Physics/Generator/M2ENNGenerator.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "CLHEP/Random/Random.h"

#include "muc/numeric"
#include "muc/utility"

#include <string>

namespace MACE::GenM2ENN {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::string_literals;

GenM2ENN::GenM2ENN() :
    Subprogram{"GenM2ENN", "Generate muon decay (mu+ -> e+ nu nu)."} {}

auto GenM2ENN::Main(int argc, char* argv[]) const -> int {
    Generator::MCMCGeneratorCLI<Generator::InitialStateCLIModule<"unpolarized", "muon">> cli;
    cli.DefaultOutput("m2enn.root");
    cli.DefaultOutputTree("m2enn");
    cli.AddMCMCStepSizeOption();
    Mustard::Env::MPIEnv env{argc, argv, cli};
    Mustard::UseXoshiro<256> random{cli};

    Mustard::M2ENNGenerator generator("mu+", cli.Momentum(),
                                      cli->present<double>("--thinning-ratio"), cli->present<unsigned>("--acf-sample-size"),
                                      cli->present<double>("--mcmc-step-size"));

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
    Generator::WriteAutocorrelationFunction(autocorrelationFunction);

    // Generate events
    if (*nEvent == 0) {
        return EXIT_SUCCESS;
    }
    Mustard::Data::Output<Mustard::Data::GeneratedKinematics> writer{cli->get("--output-tree")};
    executor(*nEvent, [&](auto) {
        const auto [weight, pdgID, p]{generator(rng)};
        Mustard::Data::Tuple<Mustard::Data::GeneratedKinematics> event;
        Get<"pdgID">(event) = std::vector{pdgID[0]};
        Get<"E">(event) = std::vector{static_cast<float>(p[0].e())};
        Get<"px">(event) = std::vector{static_cast<float>(p[0].x())};
        Get<"py">(event) = std::vector{static_cast<float>(p[0].y())};
        Get<"pz">(event) = std::vector{static_cast<float>(p[0].z())};
        Get<"w">(event) = branchingRatio.value * weight;
        writer.Fill(event);
    });
    executor.PrintExecutionSummary();
    writer.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::GenM2ENN
