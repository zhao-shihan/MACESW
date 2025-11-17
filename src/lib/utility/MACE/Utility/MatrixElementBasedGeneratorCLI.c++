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

#include "MACE/Utility/MatrixElementBasedGeneratorCLI.h++"

#include "Mustard/IO/PrettyLog.h++"

#include "gsl/gsl"

#include <vector>

namespace MACE::inline Utility {

MatrixElementBasedGeneratorCLIModule::MatrixElementBasedGeneratorCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli) :
    ModuleBase{cli} {
    TheCLI()
        ->add_argument("-i", "--phase-space-integral")
        .help("Pre-computed phase-space integral. Program will skip integration and use this value if set.")
        .nargs(1)
        .scan<'g', double>();
    TheCLI()
        ->add_argument("--integral-precision-goal")
        .help("Precision goal for phase-space integral.")
        .default_value(0.01)
        .required()
        .nargs(1)
        .scan<'g', double>();
    TheCLI()
        ->add_argument("--continue-integration")
        .help("Integration state for continuing phase-space integration.")
        .nargs(3)
        .scan<'g', long double>();
}

auto MatrixElementBasedGeneratorCLIModule::ContinueIntegration() const -> std::optional<Mustard::Math::MCIntegrationState> {
    const auto cliState{TheCLI()->present<std::vector<long double>>("--continue-integration")};
    if (not cliState.has_value()) {
        return {};
    }
    if (not TheCLI()->is_used("--seed")) {
        Mustard::MasterPrintWarning("Option --continue-integration set but --seed not set! You are probably using the previous seed, "
                                    "and it will generate the same event series. Try set exclusive seeds for each run, or simply --seed 0");
    }
    Mustard::Math::MCIntegrationState state{};
    state.sum[0] = cliState->at(0);
    state.sum[1] = cliState->at(1);
    state.n = gsl::narrow_cast<decltype(state.n)>(cliState->at(2));
    return state;
}

} // namespace MACE::inline Utility
