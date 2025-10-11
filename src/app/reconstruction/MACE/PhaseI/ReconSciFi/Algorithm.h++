#include "MACE/PhaseI/Data/Hit.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SensorRawHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/Data/Track.h++"

#include "Mustard/Data/Tuple.h++"

#include "muc/array"

#include <algorithm>
#include <memory>
#include <vector>

namespace MACE::PhaseI::ReconSciFi {

auto FindCrossCoordinates(double lAngle, double rAngle, double tAngle, double rLLayer, double rRLayer, double rTLayer, double y0, const muc::array3d dir)
    -> std::vector<muc::array3d>;

auto FindLayerID(int id) -> int;

auto FindHLMinDistanceSqaure(
    double spiral_r,            // 螺旋线半径
    double spiral_b,            // 螺旋线螺距参数
    double rotationAngle,       // 螺旋线绕z轴旋转角度
    const muc::array3d line_p0, // 直线过定点
    const muc::array3d line_dir, double initialT, double initialTheta) -> std::tuple<double, double, double>;

auto FindLLMinDistanceSqaure(
    const muc::array3d line1_p0,  // 直线1起点
    const muc::array3d line1_dir, // 直线1方向向量
    const muc::array3d line2_p0,  // 直线2起点
    const muc::array3d line2_dir  // 直线2方向向量
    ) -> double;

auto HitNumber(std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>;

auto DividedHit(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>;

auto PositionTransform(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>> data, const muc::array3d dir)
    -> std::vector<std::vector<std::pair<muc::array3d, std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>>>;

auto DirectionFit(std::vector<muc::array3d>& points)
    -> std::tuple<muc::array3d, muc::array3d, double>;

auto TrackFit(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SciFiSimHit>>>>& cluster,
              muc::array3d initialS,
              muc::array3d initialP,
              double initialChi2)
    -> std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::Track>>>;

} // namespace MACE::PhaseI::ReconSciFi
