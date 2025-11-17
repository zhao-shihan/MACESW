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

namespace MACE::SimPTS {

constexpr auto defaultMacro = {
    "#############################################################################",
    "# Initialization settings",
    "#############################################################################",
    "",
    "/control/verbose 2",
    "#/control/saveHistory",
    "/run/verbose 2",
    "",
    "#/Mustard/Detector/Description/Ixport SimPTS_geom.yaml",
    "",
    "/run/initialize",
    "",
    "/gps/particle             anti_muonium",
    "/gps/pos/type             Volume",
    "/gps/pos/shape            Ellipsoid",
    "/gps/pos/halfx            2 cm",
    "/gps/pos/halfy            2 cm",
    "/gps/pos/halfz            3 cm",
    "/gps/ene/mono             0 eV",
    "",
    "/MACE/Analysis/FilePath SimPTS_vis.root",
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
    "/vis/viewer/set/lightsVector -1 1 1",
    "#/vis/viewer/set/rotationStyle freeRotation",
    "/vis/viewer/set/projection p",
    "/vis/viewer/set/style s",
    "/vis/viewer/set/auxiliaryEdge false",
    "#/vis/viewer/set/lineSegmentsPerCircle 100",
    "/vis/viewer/zoom 2.5",
    "",
    "# Colors",
    "/vis/geometry/set/colour Accelerator                   0     1     1     1     0.1",
    "/vis/geometry/set/colour AcceleratorField              0     1     1     1     0.05",
    "/vis/geometry/set/colour Collimator                    0     1     1     1     0.01",
    "/vis/geometry/set/colour ECALField                      0     1     1     1     0.05",
    "/vis/geometry/set/colour ECALMagnet                     0     1     1     1     0.05",
    "/vis/geometry/set/colour ECALShield                     0     1     1     1     0.05",
    "/vis/geometry/set/colour MCPChamber                    0     1     1     1     0.05",
    "/vis/geometry/set/colour MCPChamberPipe                0     1     1     1     0.05",
    "/vis/geometry/set/colour MCPChamberPipeVacuum          0     1     1     1     0.01",
    "/vis/geometry/set/colour MCPChamberVacuum              0     1     1     1     0.01",
    "/vis/geometry/set/colour MMSBeamPipe                   0     1     1     1     0.05",
    "/vis/geometry/set/colour MMSBeamPipeVacuum             0     1     1     1     0.01",
    "/vis/geometry/set/colour MMSField                      0     1     1     1     0.05",
    "/vis/geometry/set/colour MMSMagnet                     0     1     1     1     0.05",
    "/vis/geometry/set/colour MMSShield                     0     1     1     1     0.05",
    "/vis/geometry/set/colour ShieldingWall                 0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidBeamPipeS1            0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidBeamPipeS1Vacuum      0     1     1     1     0.01",
    "/vis/geometry/set/colour SolenoidBeamPipeS2            0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidBeamPipeS2Vacuum      0     1     1     1     0.01",
    "/vis/geometry/set/colour SolenoidBeamPipeS3            0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidBeamPipeS3Vacuum      0     1     1     1     0.01",
    "/vis/geometry/set/colour SolenoidBeamPipeT1            0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidBeamPipeT1Vacuum      0     1     1     1     0.01",
    "/vis/geometry/set/colour SolenoidBeamPipeT2            0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidBeamPipeT2Vacuum      0     1     1     1     0.01",
    "/vis/geometry/set/colour SolenoidFieldS1               0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidFieldS2               0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidFieldS3               0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidFieldT1               0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidFieldT2               0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidS1                    0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidS2                    0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidS3                    0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidShieldS1              0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidShieldS2              0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidShieldS3              0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidShieldT1              0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidShieldT2              0     1     1     1     0.1",
    "/vis/geometry/set/colour SolenoidT1                    0     1     1     1     0.05",
    "/vis/geometry/set/colour SolenoidT2                    0     1     1     1     0.05",
    "/vis/geometry/set/colour VirtualDetectorA              0     1     1     1     0.1",
    "/vis/geometry/set/colour VirtualDetectorB              0     1     1     1     0.1",
    "/vis/geometry/set/colour VirtualDetectorC              0     1     1     1     0.1",
    "/vis/geometry/set/colour VirtualDetectorD              0     1     1     1     0.1",
    "/vis/geometry/set/colour World                         0     1     1     1     0.001",
    "",
    "/vis/scene/add/date",
    "/vis/scene/add/axes 0 0 0 10 cm",
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

#endif

    ""};

} // namespace MACE::SimPTS
