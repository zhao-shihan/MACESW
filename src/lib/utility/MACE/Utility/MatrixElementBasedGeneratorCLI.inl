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

namespace MACE::inline Utility {

template<int M, int N, typename A>
auto MatrixElementBasedGeneratorCLIModule::PhaseSpaceIntegral(Mustard::Executor<unsigned long long>& executor,
                                                              Mustard::MatrixElementBasedGenerator<M, N, A>& generator) const -> std::tuple<Mustard::Math::Estimate, double, Mustard::Math::MCIntegrationState> {
    std::tuple<Mustard::Math::Estimate, double, Mustard::Math::MCIntegrationState> result;
    auto& [integral, nEff, integrationState]{result};
    if (TheCLI()->is_used("--phase-space-integral")) {
        const auto integralFromConsole{TheCLI()->get<double>("--phase-space-integral")};
        integral.value = integralFromConsole;
        integral.uncertainty = std::numeric_limits<double>::quiet_NaN();
        nEff = std::numeric_limits<double>::quiet_NaN();
        Mustard::MasterPrintLn("Using pre-computed phase-space integral {}.", integralFromConsole);
    } else {
        const auto precisionGoal{TheCLI()->get<double>("--integral-precision-goal")};
        Mustard::Math::MCIntegrationState _{};
        if (const auto integrationState{ContinueIntegration()}) {
            result = generator.PhaseSpaceIntegral(executor, precisionGoal, *integrationState);
        } else {
            result = generator.PhaseSpaceIntegral(executor, precisionGoal);
        }
        Mustard::MasterPrintLn("You can save the above phase-space integral and integration state for future use "
                               "as long as initial state properties and acceptance function does not change "
                               "(see option -i or --phase-space-integral and --continue-integration)."
                               "\n");
    }
    return result;
}

} // namespace MACE::inline Utility
