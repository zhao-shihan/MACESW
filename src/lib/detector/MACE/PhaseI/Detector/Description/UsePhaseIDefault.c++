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

#include "MACE/Detector/Description/Accelerator.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/Detector/Description/Target.h++"
#include "MACE/PhaseI/Detector/Description/UsePhaseIDefault.h++"

#include "Mustard/IO/Print.h++"
#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::PhaseI::Detector::Description {

auto UsePhaseIDefault() -> void {
    using namespace Mustard::LiteralUnit::Length;
    using namespace Mustard::LiteralUnit::Time;
    using namespace Mustard::LiteralUnit::Energy;
    namespace MACE = MACE::Detector::Description;

    Mustard::MasterPrintLn("Applying PhaseI default detector description settings...");

    { // set accelerator up/downstream length to a random equal value
        auto& accelerator{MACE::Accelerator::Instance()};
        accelerator.MaxPotentialPosition(0);
        accelerator.AccelerateFieldLength(2);
        accelerator.DecelerateFieldLength(2);
    }
    { // Use cylinder target
        auto& target{MACE::Target::Instance()};
        target.ShapeType(MACE::Target::TargetShapeType::Cylinder);
        target.FormationProbability(0);
    }
    { // bigger windows for ECAL
        auto& ecal{MACE::ECAL::Instance()};
        ecal.InnerRadius(260_mm);
        ecal.CrystalHypotenuse(280_mm);
        ecal.UpstreamWindowRadius(104_mm);
        ecal.DownstreamWindowRadius(104_mm);
        ecal.ScintillationEnergyBin({3.300891_eV, 3.394291_eV, 3.459551_eV, 3.515883_eV, 3.557591_eV, 3.591915_eV,
                                     3.622042_eV, 3.644458_eV, 3.678815_eV, 3.690132_eV, 3.715531_eV, 3.728362_eV,
                                     3.747776_eV, 3.768708_eV, 3.787216_eV, 3.80725_eV, 3.820723_eV, 3.847958_eV,
                                     3.871416_eV, 3.9022_eV, 3.932045_eV, 4.042466_eV, 4.157667_eV, 4.193192_eV,
                                     4.224366_eV, 4.24096_eV, 4.257685_eV, 4.274542_eV, 4.291534_eV, 4.306942_eV,
                                     4.343328_eV, 4.359111_eV, 4.396388_eV, 4.437953_eV, 4.471035_eV, 4.511202_eV,
                                     4.546346_eV, 4.602552_eV, 4.662178_eV, 4.733725_eV, 4.883613_eV, 5.114986_eV});
        ecal.ScintillationComponent1({0.152652, 0.190301, 0.233538, 0.282997, 0.328661, 0.379536,
                                      0.430119, 0.47672, 0.531663, 0.57447, 0.617499, 0.656175,
                                      0.695466, 0.729048, 0.768509, 0.803861, 0.841795, 0.88913,
                                      0.937399, 0.977291, 1.0, 0.972946, 0.880537, 0.821459,
                                      0.754517, 0.691979, 0.640116, 0.591668, 0.540922, 0.501151,
                                      0.452269, 0.413595, 0.35813, 0.307095, 0.264469, 0.218419,
                                      0.182053, 0.129528, 0.081253, 0.047185, 0.017148, 0.0});
        ecal.ScintillationYield(3500);
        ecal.ScintillationTimeConstant1(30_ns);
        ecal.UsePhaseICrystal(true);
        ecal.MPPCNPixelRowSet({8, 8, 8, 8, 8, 8, 8, 8, 8, 8});
        ecal.MPPCEnergyBin({1.771068_eV, 2.101763_eV, 2.478823_eV, 2.66882_eV, 2.756137_eV, 2.849362_eV, 2.949113_eV, 3.02425_eV, 3.099632_eV, 3.178868_eV, 3.3981_eV, 3.649811_eV, 3.877416_eV, 4.128759_eV, 4.348776_eV});
        ecal.MPPCEfficiency({0.26319, 0.395706, 0.503681, 0.525767, 0.555215, 0.570552, 0.597546, 0.59816, 0.599387, 0.593252, 0.521472, 0.466258, 0.458896, 0.408589, 0.196933});
        ecal.WaveformIntegralTime(200_ns);
    }
}

} // namespace MACE::PhaseI::Detector::Description
