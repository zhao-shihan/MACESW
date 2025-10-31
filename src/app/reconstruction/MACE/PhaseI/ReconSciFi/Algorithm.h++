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

auto FindCrossCoordinates(double lID, double rID, double tID, int lNumber, int rNumber, int tNumber, double x0, double y0)
    -> std::vector<muc::array3d>;

auto FindLayerID(int id) -> int;

auto HitNumber(std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>>;

template<typename... Args>
auto InSameSubarray(Args... args) -> bool;

auto DividedHit(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>>& data, double deltaTime)
    -> std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>>;

auto PositionTransform(const std::vector<std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::SiPMHit>>>>& data)
    -> std::vector<std::shared_ptr<Mustard::Data::Tuple<MACE::PhaseI::Data::ReconTrack>>>;

} // namespace MACE::PhaseI::ReconSciFi
