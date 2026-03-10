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

#pragma once

#include <initializer_list>

namespace MACE::SimECAL {

constexpr auto defaultMacro = {
    "#############################################################################",
    "# Initialization settings",
    "#############################################################################",
    "",
    "/control/verbose 2",
    "/control/saveHistory",
    "/run/verbose 2",
    "",
    "/MACE/Physics/UseRadioactiveDecayPhysics",
    "#/MACE/Physics/UseOpticalPhysics",
    "",
    "/run/initialize",
    "",
    "#/MACE/Generator/SwitchToEcoMug",
    "/MACE/Generator/SwitchToGPSX",
    "",
    "/MACE/Analysis/FilePath SimECAL_vis.root",
    "/MACE/Analysis/FileMode RECREATE",
    "",

#if MUSTARD_USE_G4VIS

    "#############################################################################",
    "# Visualization settings",
    "#############################################################################",
    "",
    "/vis/open OGL",
    "",
    "/vis/viewer/set/autoRefresh false",
    "/vis/verbose errors",
    "",
    "/vis/drawVolume",
    "",
    "/vis/viewer/set/viewpointVector -1 0.5 0.5",
    "#/vis/viewer/set/lightsVector -1 1 1",
    "#/vis/viewer/set/rotationStyle freeRotation",
    "/vis/viewer/set/projection p",
    "/vis/viewer/set/style s",
    "/vis/viewer/set/auxiliaryEdge false",
    "#/vis/viewer/set/lineSegmentsPerCircle 100",
    "/vis/viewer/zoom 1.5",
    "#/vis/viewer/addCutawayPlane 0 0 0",
    "",
    "# Colors",
    "/vis/geometry/set/colour ECALCrystal                   0     1     1     1     0.1",
    "/vis/geometry/set/colour ECALPMCathode                0     1     1     1     0.3",
    "/vis/geometry/set/colour ECALPMTCoupler                0     1     1     1     0.1",
    "/vis/geometry/set/colour ECALPMTShell                  0     1     1     1     0.1",
    "/vis/geometry/set/colour ECALPMTVacuum                 0       1       1       1       0.",
    "/vis/geometry/set/colour ECALMPPCWindow                0     1     1     1     0.2",
    "/vis/geometry/set/colour ECALMPPCCoupler                0     1     1     1     0.1",
    "/vis/geometry/set/colour ECALTunnel                    0     1     1     1     0.1",
    "/vis/geometry/set/colour ECALShield                    0     1     1     1     0.1",
    "/vis/geometry/set/colour ECALMagnet                    0     1     1     1     0.1",
    "",
    "/vis/scene/add/date",
    "/vis/scene/add/axes 0 0 0 40 cm",
    "/vis/scene/add/trajectories smooth #rich",
    "/vis/scene/add/hits",
    "",
    "/vis/filtering/trajectories/create/particleFilter",
    "/vis/filtering/trajectories/particleFilter-0/add mu+",
    "/vis/filtering/trajectories/particleFilter-0/add mu-",
    "/vis/filtering/trajectories/particleFilter-0/add muonium",
    "/vis/filtering/trajectories/particleFilter-0/add anti_muonium",
    "/vis/filtering/trajectories/particleFilter-0/add e+",
    "/vis/filtering/trajectories/particleFilter-0/add e-",
    "/vis/filtering/trajectories/particleFilter-0/add gamma",
    "/vis/filtering/trajectories/particleFilter-0/add opticalphoton",
    "/vis/filtering/trajectories/particleFilter-0/add neutron",
    "/vis/filtering/trajectories/particleFilter-0/add anti_neutron",
    "/vis/filtering/trajectories/particleFilter-0/add proton",
    "/vis/filtering/trajectories/particleFilter-0/add anti_proton",
    "/vis/filtering/trajectories/particleFilter-0/add pi+",
    "/vis/filtering/trajectories/particleFilter-0/add pi-",
    "/vis/filtering/trajectories/particleFilter-0/add pi0",
    "",
    "/vis/modeling/trajectories/create/drawByParticleID",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setDrawAuxPts false",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setAuxPtsSize 2",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setAuxPtsSizeType screen",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setAuxPtsType dots",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setDrawLine true",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setLineWidth 2",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setDrawStepPts true",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsSize 2",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsSizeType screen",
    "/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsType dots",
    "",
    "/vis/modeling/trajectories/drawByParticleID-0/setRGBA muonium 1 0 1 1",
    "/vis/modeling/trajectories/drawByParticleID-0/setRGBA anti_muonium 1 0 1 1",
    "",
    "/vis/scene/endOfEventAction accumulate",
    "",
    "/vis/verbose warnings",
    "/vis/viewer/set/autoRefresh true",
    "/vis/geometry/set/visibility World 0 false",

// "#############################################################################",
// "# General Particle Source settings",
// "#############################################################################",

// "/gps/particle gamma",
// "/gps/ene/mono 0.511 MeV",
// "/gps/pos/centre 0. 0. 0. cm",
// "/gps/direction 0 0.370438 -0.599382",
// "/gps/ang/type iso",

// "/gps/particle e+",
// "/gps/direction 0 0 1",
// "/gps/ene/mono 5 keV",
// "/gps/pos/type Beam",
// "/gps/pos/shape Circle",
// "/gps/pos/radius 0 cm",
// "/gps/pos/sigma_r 5.6 mm",
// "/gps/pos/centre 0. 0. -40. cm",

#endif

    ""};

} // namespace MACE::SimECAL
