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

#include "MACE/GenBkgM2ENNE/GenBkgM2ENNE.h++"
#include "MACE/GenM2ENNEE/GenM2ENNEE.h++"
#include "MACE/GenM2ENNGG/GenM2ENNGG.h++"
#include "MACE/MakeGeometry/MakeGeometry.h++"
#include "MACE/PhaseI/PhaseI.h++"
#include "MACE/ReconECAL/ReconECAL.h++"
#include "MACE/ReconMMSTrack/ReconMMSTrack.h++"
#include "MACE/SimDose/SimDose.h++"
#include "MACE/SimECAL/SimECAL.h++"
#include "MACE/SimMACE/SimMACE.h++"
#include "MACE/SimMMS/SimMMS.h++"
#include "MACE/SimPTS/SimPTS.h++"
#include "MACE/SimTTC/SimTTC.h++"
#include "MACE/SimTarget/SimTarget.h++"
#include "MACE/SmearMACE/SmearMACE.h++"

#include "Mustard/Application/SubprogramLauncher.h++"

auto main(int argc, char* argv[]) -> int {
    Mustard::Application::SubprogramLauncher launcher;
    launcher.AddSubprogram<MACE::GenBkgM2ENNE::GenBkgM2ENNE>();
    launcher.AddSubprogram<MACE::GenM2ENNEE::GenM2ENNEE>();
    launcher.AddSubprogram<MACE::GenM2ENNGG::GenM2ENNGG>();
    launcher.AddSubprogram<MACE::MakeGeometry::MakeGeometry>();
    launcher.AddSubprogram<MACE::PhaseI::PhaseI>();
    launcher.AddSubprogram<MACE::ReconECAL::ReconECAL>();
    launcher.AddSubprogram<MACE::ReconMMSTrack::ReconMMSTrack>();
    launcher.AddSubprogram<MACE::SimDose::SimDose>();
    launcher.AddSubprogram<MACE::SimECAL::SimECAL>();
    launcher.AddSubprogram<MACE::SimMACE::SimMACE>();
    launcher.AddSubprogram<MACE::SimMMS::SimMMS>();
    launcher.AddSubprogram<MACE::SimPTS::SimPTS>();
    launcher.AddSubprogram<MACE::SimTarget::SimTarget>();
    launcher.AddSubprogram<MACE::SimTTC::SimTTC>();
    launcher.AddSubprogram<MACE::SmearMACE::SmearMACE>();
    return launcher.LaunchMain(argc, argv);
}
