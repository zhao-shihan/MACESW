#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SensorRawHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Finder/GenFitDAFFinder.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitDAFFitter.h++"
#include "MACE/PhaseI/Reconstruction/SciFiTracking/Fitter/GenFitReferenceKalmanFitter.h++"

#include "Mustard/CLI/BasicCLI.h++"
#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "GenFitTest.h++"

#include "ROOT/RDataFrame.hxx"
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"

#include "muc/algorithm"
#include "muc/array"

#include "fmt/format.h"

#include <ConstField.h>
#include <EventDisplay.h>
#include <Exception.h>
#include <FieldManager.h>
#include <HelixMeasurement.h>
#include <KalmanFitterRefTrack.h>
#include <MaterialEffects.h>
#include <PlanarMeasurement.h>
#include <RKTrackRep.h>
#include <SpacepointMeasurement.h>
#include <StateOnPlane.h>
#include <TEveManager.h>
#include <TGeoManager.h>
#include <TGeoMaterialInterface.h>
#include <TMath.h>
#include <TVector3.h>
#include <Track.h>
#include <TrackPoint.h>
#include <WireMeasurement.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numbers>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace MACE::PhaseI::ReconSciFi {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::LiteralUnit::Time;
using namespace Mustard::MathConstant;
using namespace Mustard::PhysicalConstant;
using namespace std::literals;

GenFitTest::GenFitTest() :
    Subprogram{"GenFitTest", "GenFit test for Scintilating Fiber Tracker (SciFi Tracker) event reconstruction."} {}

void addVerticalWireMeasurement(genfit::Track* track, double x, double y, double zStart, double zEnd, int detID, int hitID) {
    // 创建导线的两个端点（与z轴平行）
    TVector3 p1(x, y, zStart); // 起点
    TVector3 p2(x, y, zEnd);   // 终点

    // 创建7维坐标向量
    TVectorD hitCoords(7);
    hitCoords[0] = p1.X(); // 端点1 x
    hitCoords[1] = p1.Y(); // 端点1 y
    hitCoords[2] = p1.Z(); // 端点1 z
    hitCoords[3] = p2.X(); // 端点2 x
    hitCoords[4] = p2.Y(); // 端点2 y
    hitCoords[5] = p2.Z(); // 端点2 z
    hitCoords[6] = 0;      // 漂移距离=0 (直接击中)

    TMatrixDSym hitCov(7);
    hitCov.UnitMatrix();
    hitCov(6, 6) = 0.0289 * 0.0289; // 漂移距离的方差

    // 创建导线测量
    genfit::WireMeasurement* meas =
        new genfit::WireMeasurement(hitCoords, hitCov, detID, hitID++, nullptr);
    // meas->Print();
    //  设置左右分辨率
    meas->setLeftRightResolution(-1);

    // 添加到轨迹
    track->insertPoint(new genfit::TrackPoint(meas, track));
}

auto GenFitTest::Main(int argc, char* argv[]) const -> int {
    Mustard::CLI::BasicCLI<> cli;
    cli->add_argument("input").help("Input file path(s).").nargs(argparse::nargs_pattern::at_least_one);
    cli->add_argument("-t", "--input-tree").help("Input tree name.").default_value("data"s).required().nargs(1);
    cli->add_argument("-o", "--output").help("Output file path.").required().nargs(1);
    cli->add_argument("-m", "--output-mode").help("Output file creation mode.").default_value("NEW"s).required().nargs(1);
    cli->add_argument("-c", "--description").help("Description YAML file path.").nargs(1);
    Mustard::Env::MPIEnv env{argc, argv, {}};
    std::string fileName{argv[1]};
    TFile file{Mustard::Parallel::ProcessSpecificPath("output1.root").generic_string().c_str(), "RECREATE"};
    Mustard::Data::Output<PhaseI::Data::Track> reconTrack{"G4Run0/ReconTrack"};

    MACE::PhaseI::SciFiTracking::GenFitDAFFinder<MACE::PhaseI::Data::SciFiHit, MACE::PhaseI::Data::Track> finder;
    // MACE::PhaseI::SciFiTracking::GenFitReferenceKalmanFitter<MACE::PhaseI::Data::SciFiHit, MACE::PhaseI::Data::Track> fitter{0.00289};
    MACE::PhaseI::SciFiTracking::GenFitDAFFitter<MACE::PhaseI::Data::SciFiHit, MACE::PhaseI::Data::Track> fitter{0.00289};
    fitter.EnableEventDisplay(false);

    Mustard::Data::Processor processor;
    processor.Process<PhaseI::Data::SciFiSimHit>(
        ROOT::RDataFrame{"G4Run0/SciFiSimHit", fileName}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return std::tie(Get<"FiberID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"FiberID">(*hit2), Get<"t">(*hit2));
                         });

            std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>> sciFiHitData;
            for (auto&& hit : event) {
                auto sciFiHit{std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>()};
                *Get<"EvtID">(*sciFiHit) = *Get<"EvtID">(*hit);
                *Get<"FiberID">(*sciFiHit) = *Get<"FiberID">(*hit);
                *Get<"nOptPho">(*sciFiHit) = *Get<"nOptPho">(*hit);
                *Get<"t">(*sciFiHit) = *Get<"t">(*hit);
                sciFiHitData.emplace_back(std::move(sciFiHit));
            }

            auto nextTrackID{0};
            for (auto&& [trackID, good] : finder(sciFiHitData, nextTrackID).good) {
                const auto track{fitter(good.hitData, good.seed).track};
                if (track == nullptr) {
                    continue;
                }
                std::cout << Get<"EvtID">(*good.seed) << " " << Get<"p">(*good.seed)[0] << " " << Get<"p">(*good.seed)[1] << " " << Get<"p">(*good.seed)[2] << std::endl;
                // std::cout << Get<"EvtID">(*good.seed) << " " << Get<"x">(*good.seed)[0] << " " << Get<"x">(*good.seed)[1] << " " << Get<"x">(*good.seed)[2] << std::endl;
                std::cout << Get<"EvtID">(*track) << " " << Get<"p">(*track)[0] << " " << Get<"p">(*track)[1] << " " << Get<"p">(*track)[2] << std::endl;
                reconTrack.Fill(*track);
            }
        });
    reconTrack.Write();
    // fitter.OpenEventDisplay();
    return EXIT_SUCCESS;
}
} // namespace MACE::PhaseI::ReconSciFi