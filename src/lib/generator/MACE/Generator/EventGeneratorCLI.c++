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

#include "MACE/Generator/EventGeneratorCLI.h++"

#include "Mustard/CLI/CLI.h++"
#include "Mustard/IO/Print.h++"

#include <cstdlib>

namespace MACE::Generator {

EventGeneratorCLIModule::EventGeneratorCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli) :
    ModuleBase{cli} {
    using namespace std::string_literals;
    TheCLI()
        ->add_argument("n")
        .help("Number of events to generate. Program will skip event generation if n is 0 (but could still compute the phase-space integral).")
        .nargs(1)
        .scan<'i', unsigned long long>();
    TheCLI()
        ->add_argument("-o", "--output")
        .help("Output file path.")
        .required()
        .nargs(1);
    TheCLI()
        ->add_argument("-m", "--output-mode")
        .help("Output file creation mode (see ROOT documentation for details).")
        .default_value("NEW"s)
        .required()
        .nargs(1);
    TheCLI()
        ->add_argument("-t", "--output-tree")
        .help("Output tree name.")
        .required()
        .nargs(1);
}

auto EventGeneratorCLIModule::DefaultOutput(std::string path) -> void {
    TheCLI()["--output"].default_value(std::move(path));
}

auto EventGeneratorCLIModule::DefaultOutputTree(std::string name) -> void {
    TheCLI()["--output-tree"].default_value(std::move(name));
}

auto EventGeneratorCLIModule::NEvent() const -> unsigned long long {
    const auto nEvent{TheCLI()->get<unsigned long long>("n")};
    if (nEvent == 0) {
        Mustard::MasterPrintLn("No events to generate.");
    } else {
        Mustard::MasterPrintLn("Generating {} events...", nEvent);
    }
    return nEvent;
}

} // namespace MACE::Generator
