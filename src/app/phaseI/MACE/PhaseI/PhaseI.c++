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

#include "MACE/PhaseI/PhaseI.h++"
#include "MACE/PhaseI/ReconECAL/ReconECAL.h++"
#include "MACE/PhaseI/ReconSciFi/ReconSciFi.h++"
#include "MACE/PhaseI/SimMACEPhaseI/SimMACEPhaseI.h++"

#include "Mustard/Application/SubprogramLauncher.h++"

namespace MACE::PhaseI {

PhaseI::PhaseI() :
    Subprogram{"PhaseI", "Subprograms for the Phase-I Muonium-to-Antimuonium Conversion Experiment (MACE Phase-I)."} {}

auto PhaseI::Main(int argc, char* argv[]) const -> int {
    Mustard::Application::SubprogramLauncher launcher;
    launcher.AddSubprogram<MACE::PhaseI::ReconECAL::ReconECAL>();
    launcher.AddSubprogram<MACE::PhaseI::ReconSciFi::ReconSciFi>();
    launcher.AddSubprogram<MACE::PhaseI::SimMACEPhaseI::SimMACEPhaseI>();
    return launcher.LaunchMain(argc, argv);
}

} // namespace MACE::PhaseI
