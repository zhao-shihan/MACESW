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
