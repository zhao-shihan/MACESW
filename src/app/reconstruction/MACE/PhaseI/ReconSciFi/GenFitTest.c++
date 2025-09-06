#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SensorRawHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"
#include "MACE/PhaseI/Detector/Description/SciFiTracker.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/CLI/BasicCLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Extension/MPIX/DataType.h++"
#include "Mustard/Extension/MPIX/ParallelizePath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/MathConstant.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "Algorithm.h++"
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
    Mustard::Env::CLI::BasicCLI<> cli;
    cli->add_argument("input").help("Input file path(s).").nargs(argparse::nargs_pattern::at_least_one);
    cli->add_argument("-t", "--input-tree").help("Input tree name.").default_value("data"s).required().nargs(1);
    cli->add_argument("-o", "--output").help("Output file path.").required().nargs(1);
    cli->add_argument("-m", "--output-mode").help("Output file creation mode.").default_value("NEW"s).required().nargs(1);
    cli->add_argument("-c", "--description").help("Description YAML file path.").nargs(1);
    Mustard::Env::MPIEnv env{argc, argv, {}};
    const auto& sciFiTracker{MACE::PhaseI::Detector::Description::SciFiTracker::Instance()};
    std::string fileName{argv[1]};
    std::string fileName1{argv[2]};
    TFile file{Mustard::MPIX::ParallelizePath("output1.root").generic_string().c_str(), "RECREATE"};
    Mustard::Data::Output<PhaseI::Data::ReconTrack> reconTrack{"G4Run0/ReconTrack"};
    Mustard::Data::Processor processor;

    std::map<int, std::set<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> hitSet;
    const auto& fiberMap = sciFiTracker.DetectorFiberInformation();

    processor.Process<PhaseI::Data::SciFiSiPMRawHit>(
        ROOT::RDataFrame{"G4Run0/SciFiSiPMHit", fileName}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) { return; }
            // if (*Get<"EvtID">(**event.begin()) != 15997) { return; }
            muc::timsort(event,
                         [](auto&& hit1, auto&& hit2) {
                             return std::tie(Get<"SiPMID">(*hit1), Get<"t">(*hit1)) < std::tie(Get<"SiPMID">(*hit2), Get<"t">(*hit2));
                         });

            std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>> siPMHitData;
            for (std::ranges::subrange siPMHitRange{event.begin(), event.begin()};
                 siPMHitRange.begin() != event.end();
                 siPMHitRange = {siPMHitRange.end(), siPMHitRange.end()}) {
                siPMHitRange = std::ranges::equal_range(siPMHitRange.begin(), event.end(), *Get<"SiPMID">(**siPMHitRange.begin()), std::less{},
                                                        [](auto&& hit) { return Get<"SiPMID">(*hit); });
                int count = 0;
                double initialTime = *Get<"t">(**siPMHitRange.begin());
                double endTime = initialTime + sciFiTracker.ThresholdTime();
                for (int j{}; j < std::ssize(siPMHitRange); ++j) {
                    if (*Get<"t">(*siPMHitRange[j]) >= initialTime && *Get<"t">(*siPMHitRange[j]) < endTime) {
                        initialTime = *Get<"t">(*siPMHitRange[j]);
                        count++;
                        if (count == sciFiTracker.Threshold()) {
                            endTime = initialTime + sciFiTracker.TimeWindow();
                            siPMHitData.emplace_back(std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>());
                            *Get<"t">(*siPMHitData.back()) = *Get<"t">(*siPMHitRange[j]);
                            *Get<"EvtID">(*siPMHitData.back()) = *Get<"EvtID">(*siPMHitRange[j]);
                            *Get<"SiPMID">(*siPMHitData.back()) = *Get<"SiPMID">(*siPMHitRange[j]);

                            while ([&] {
                                if (j >= std::ssize(siPMHitRange)) {
                                    return false;
                                } else {
                                    return (j < std::ssize(siPMHitRange) && *Get<"t">(*siPMHitRange[j]) < endTime);
                                }
                            }()) {
                                count++;
                                j++;
                            }
                            *Get<"nOptPho">(*siPMHitData.back()) = count;
                            count = 0;
                            if (j < std::ssize(siPMHitRange)) {
                                initialTime = endTime + sciFiTracker.DeadTime();
                                endTime = initialTime + sciFiTracker.ThresholdTime();
                            }
                        }
                    } else if (j < std::ssize(siPMHitRange)) {
                        while ([&] {
                            if ((j) >= std::ssize(siPMHitRange)) {
                                return false;
                            } else {
                                return ((j) < std::ssize(siPMHitRange) && *Get<"t">(*siPMHitRange[j]) < endTime);
                            }
                        }()) {
                            if (j < std::ssize(siPMHitRange)) {
                                j++;
                            }
                        }

                        if (j < std::ssize(siPMHitRange)) {
                            if (initialTime < *Get<"t">(*siPMHitRange[j]))
                                initialTime = *Get<"t">(*siPMHitRange[j]);
                        }
                        endTime = initialTime + sciFiTracker.ThresholdTime();
                        count = 0;
                    } else {
                        break;
                    }
                }
            }

            auto cluster{HitNumber(siPMHitData, sciFiTracker.ThresholdTime())};
            auto divHit{DividedHit(cluster, sciFiTracker.ThresholdTime())};
            auto initialPosition{PositionTransform(divHit, muc::array3d{})};

            for (auto&& initialclusterList : initialPosition) {
                std::vector<muc::array3d> point;
                std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>> hit;
                for (auto&& initialcluster : initialclusterList) {
                    point.push_back(initialcluster.first);
                    hit.push_back(initialcluster.second);
                    if (std::ssize(initialclusterList) > 3) {
                        hitSet[*Get<"EvtID">(*initialcluster.second.front())].insert(initialcluster.second.begin(), initialcluster.second.end());
                        // std::cout << *Get<"EvtID">(*initialcluster.second.front()) << " " << std::ssize(initialclusterList) << std::endl;
                    }
                }
            }
            // for (const auto& hit : siPMHitData) {
            //     hitMap[*Get<"EvtID">(*hit)].emplace_back(hit);
            // }
        });

    new TGeoManager("Geometry", "Geane geometry");
    TGeoManager::Import("macephasei_geometry/macephasei.root");
    genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
    genfit::FieldManager::getInstance()->init(new genfit::ConstField(0., 0., 0.));
    // 设置材料效应处理
    genfit::MaterialEffects::getInstance()->setEnergyLossBrems(false);
    genfit::MaterialEffects::getInstance()->setEnergyLossBetheBloch(false);

    processor.Process<PhaseI::Data::ReconTrack>(
        ROOT::RDataFrame{"G4Run0/ReconTrack", fileName1}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) { return; }
            // if (*Get<"EvtID">(**event.begin()) != 15994) { return; }

            try {
                // init fitter
                auto fitter = new genfit::KalmanFitterRefTrack();

                // particle pdg code;
                const int pdg = -11;

                // start values for the fit, e.g. from pattern recognition
                TVector3 pos(Get<"x">(**event.begin())[0] * 0.1, Get<"x">(**event.begin())[1] * 0.1, Get<"x">(**event.begin())[2] * 0.1);
                TVector3 mom(Get<"p">(**event.begin())[0] / 20, Get<"p">(**event.begin())[1] / 20, Get<"p">(**event.begin())[2] / 20);

                // for (auto& elem : event) {
                //     std::cout << Get<"EvtID">(*elem) << " " << Get<"x">(*elem)[0] << " " << Get<"x">(*elem)[1] << " " << Get<"x">(*elem)[2] << " "
                //               << Get<"p">(*elem)[0] << " " << Get<"p">(*elem)[1] << " " << Get<"p">(*elem)[2] << std::endl;
                // }
                // trackrep
                genfit::AbsTrackRep* rep = new genfit::RKTrackRep(pdg);

                // create track
                genfit::Track fitTrack(rep, pos, mom);
                int detID = 0;
                int hitID = 0;

                for (auto&& hit : hitSet[*Get<"EvtID">(**event.begin())]) {
                    if (fiberMap[*Get<"SiPMID">(*hit)].layerType == "Transverse") {
                        double x = fiberMap[*Get<"SiPMID">(*hit)].radius * std::cos(fiberMap[*Get<"SiPMID">(*hit)].rotationAngle);
                        double y = fiberMap[*Get<"SiPMID">(*hit)].radius * std::sin(fiberMap[*Get<"SiPMID">(*hit)].rotationAngle);
                        addVerticalWireMeasurement(&fitTrack, x * 0.1, y * 0.1, sciFiTracker.FiberLength() / 2 * 0.1, -sciFiTracker.FiberLength() / 2 * 0.1, detID++, hitID++);
                    } else {
                        TMatrixDSym hitCov(7);
                        hitCov.UnitMatrix();
                        hitCov(6, 6) = 0.0289 * 0.0289; // 漂移距离的方差
                        TVectorD hitCoords(7);
                        hitCoords[0] = 0.;                                                                                    // x
                        hitCoords[1] = 0.;                                                                                    // y
                        hitCoords[2] = 0.;                                                                                    // z
                        hitCoords[3] = fiberMap[*Get<"SiPMID">(*hit)].radius * 0.1;                                           // r
                        hitCoords[4] = std::copysign(sciFiTracker.FiberLength() * 0.1, fiberMap[*Get<"SiPMID">(*hit)].pitch); // pitch
                        hitCoords[5] = fiberMap[*Get<"SiPMID">(*hit)].rotationAngle - pi;                                     // phi0
                        hitCoords[6] = 0;                                                                                     // 漂移距离=0 (直接击中)

                        // genfit::HelixMeasurement* measurement = new genfit::HelixMeasurement(hitCoords, hitCov, detID++, hitID++, nullptr);
                        //  measurement->Print();
                        // measurement->setLeftRightResolution(-1);
                        // fitTrack.insertPoint(new genfit::TrackPoint(measurement, &fitTrack));
                    }
                }

                // check
                fitTrack.checkConsistency();

                // do the fit
                fitter->processTrack(&fitTrack);

                const genfit::FitStatus* fitStatus = fitTrack.getFitStatus();
                double targetRadius = 4.5; // 目标圆柱面半径（cm）

                if (fitStatus && fitStatus->isFitted()) {
                    // 获取轨迹表示
                    genfit::AbsTrackRep* rep = fitTrack.getTrackRep(0);

                    // 获取初始状态（可以使用任意拟合点作为起点）
                    genfit::StateOnPlane state = fitTrack.getFittedState(0);

                    // 创建圆柱面：轴为z轴，半径为4.5cm
                    TVector3 cylinderAxis(0, 0, 1);  // z轴方向
                    TVector3 cylinderPoint(0, 0, 0); // 轴上的一个点（原点）

                    // 使用GenFit的圆柱面外推功能
                    rep->extrapolateToCylinder(
                        state,
                        targetRadius,  // double 类型
                        cylinderPoint, // TVector3 类型
                        cylinderAxis,  // TVector3 类型
                        true,
                        false);

                    // 获取外推后的状态
                    TVector3 Pos = state.getPos();
                    TVector3 Mom = state.getMom();

                    auto startPoint = std::make_shared<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>();
                    Get<"EvtID">(*startPoint) = *Get<"EvtID">(**event.begin());
                    Get<"x">(*startPoint) = muc::array3d{Pos.X() * 10, Pos.Y() * 10, Pos.Z() * 10}; // 转换为mm
                    Get<"p">(*startPoint) = muc::array3d{Mom.X() / Mom.Mag(), Mom.Y() / Mom.Mag(), Mom.Z() / Mom.Mag()};

                    std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>> outputs;
                    outputs.push_back(startPoint);
                    reconTrack.Fill(outputs);

                    // std::cout << "  Position: ("
                    //           << Pos.X() << ", "
                    //           << Pos.Y() << ", "
                    //           << Pos.Z() << ") cm\n";
                    // std::cout << "  Momentum: ("
                    //           << Mom.X() / Mom.Mag() << ", "
                    //           << Mom.Y() / Mom.Mag() << ", "
                    //           << Mom.Z() / Mom.Mag() << ") GeV/c\n";
                }
                delete fitter;
            } catch (const genfit::Exception& e) {
                std::cerr << "GenFit Exception caught: " << e.what() << std::endl;
            }
        });
    reconTrack.Write();
    return EXIT_SUCCESS;
}
} // namespace MACE::PhaseI::ReconSciFi