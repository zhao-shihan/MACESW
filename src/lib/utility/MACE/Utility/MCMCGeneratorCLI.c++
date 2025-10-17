#include "MACE/Utility/MCMCGeneratorCLI.h++"

#include "Mustard/CLI/CLI.h++"

namespace MACE::inline Utility {

MCMCGeneratorCLIModule::MCMCGeneratorCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli) :
    ModuleBase{cli} {
    TheCLI()
        ->add_argument("--thinning-ratio")
        .help("Thinning ratio in MCMC sampling. Larger the ratio, more samples will be discarded, "
              "and events generated will be more likely to be i.i.d.")
        .nargs(1)
        .scan<'g', double>();
    TheCLI()
        ->add_argument("--acf-sample-size")
        .help("Sample size for estimation autocorrelation function (ACF).")
        .nargs(1)
        .scan<'i', unsigned>();
}

} // namespace MACE::inline Utility
