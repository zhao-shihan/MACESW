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
