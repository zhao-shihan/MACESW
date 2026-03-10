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

namespace MACE::Detector::Description {

template<typename ADerivedShape>
Target::ShapeBase<ADerivedShape>::ShapeBase(ADerivedShape* derived) :
    Mustard::Env::Memory::WeakSingleton<ADerivedShape>{derived} {
    static_assert(
        requires(const ADerivedShape shape, double x[3], bool inside) {
            requires std::is_base_of_v<ShapeBase<ADerivedShape>, ADerivedShape>;
            requires std::is_final_v<ADerivedShape>;
            { shape.VolumeContain(x) } -> std::same_as<bool>;
            { shape.Contain(x, inside) } -> std::same_as<bool>;
            { shape.DetectableAt(x) } -> std::same_as<bool>;
        });
}

template<typename ADerivedShape>
template<typename ADerivedDetail>
Target::ShapeBase<ADerivedShape>::DetailBase<ADerivedDetail>::DetailBase(ADerivedDetail* derived) :
    Mustard::Env::Memory::WeakSingleton<ADerivedDetail>{derived} {
    static_assert(
        requires(const ADerivedDetail detail, double x[3]) {
            requires std::is_base_of_v<DetailBase<ADerivedDetail>, ADerivedDetail>;
            requires std::is_final_v<ADerivedDetail>;
            { detail.DetailContain(x) } -> std::same_as<bool>;
            { detail.DetailDetectable(x) } -> std::same_as<bool>;
        });
}

auto Target::CuboidTarget::VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    const auto [x1, y1, z1]{RotateBack(x)};
    return -fThickness <= z1 and z1 <= 0 and
           std::abs(x1) <= fWidth / 2 and
           std::abs(y1) <= fHeight / 2;
}

auto Target::CuboidTarget::Contain(const Mustard::Concept::InputVector3D auto& x, bool insideVolume) const -> bool {
    switch (fDetailType) {
    case ShapeDetailType::Flat:
        return insideVolume;
    case ShapeDetailType::Perforated:
        return insideVolume and
               fPerforated.DetailContain(x);
    }
    muc::unreachable();
}

auto Target::CuboidTarget::DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    const auto [x1, y1, z1]{RotateBack(x)};
    const auto notShadowed{z1 > 0 or std::abs(x1) > fWidth / 2 or std::abs(y1) > fHeight / 2 or
                           x[0] < -(fWidth * fCosTiltAngle / 2 + fThickness * fSinTiltAngle) or
                           x[0] > fWidth * fCosTiltAngle / 2};
    switch (fDetailType) {
    case ShapeDetailType::Flat:
        return notShadowed;
    case ShapeDetailType::Perforated:
        return notShadowed or
               fPerforated.DetailDetectable(x);
    }
    muc::unreachable();
}

auto Target::CuboidTarget::PerforatedCuboid::DetailContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    const auto [x1, y1, z1]{fCuboid->RotateBack(x)};
    if (z1 < -fDepth or std::abs(x1) > fWidthExtent / 2 or std::abs(y1) > fHeightExtent / 2) {
        return true;
    }
    using std::numbers::sqrt3;
    const auto p{fSpacing + fDiameter};

    const auto u0{p * muc::llround((x1 - (1 / sqrt3) * y1) / p)};
    const auto v0{p * muc::llround((2 / sqrt3) * y1 / p)};
    const auto x0{u0 + v0 / 2};
    const auto y0{(sqrt3 / 2) * v0};

    const auto deltaX{x1 - x0};
    const auto deltaY{y1 - y0};
    const auto radius{fDiameter / 2};
    const auto deltaXY2MinusR2{muc::pow(deltaX, 2) + (deltaY + radius) * (deltaY - radius)};

    if (deltaXY2MinusR2 <= 0) {
        return false;
    }
    const auto deltaXY2MinusR2PlusP2{deltaXY2MinusR2 + muc::pow(p, 2)};
    const auto pDeltaX{p * deltaX};
    return deltaXY2MinusR2PlusP2 > std::abs(2 * pDeltaX) and
           deltaXY2MinusR2PlusP2 > std::abs(pDeltaX + sqrt3 * p * deltaY);
}

auto Target::CuboidTarget::RotateBack(const Mustard::Concept::InputVector3D auto& x) const -> muc::array3d {
    return {fCosTiltAngle * x[0] + fSinTiltAngle * x[2],
            x[1],
            -fSinTiltAngle * x[0] + fCosTiltAngle * x[2]};
}

auto Target::MultiLayerTarget::VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    const auto x0{muc::even(fCount) ? fSpacing / 2 : -fThickness / 2};
    const auto r{fSpacing + fThickness};
    const auto u{(x[0] + x0) / r};
    return u - muc::llround(u - 0.5) >= fSpacing / r and
           std::abs(x[0]) <= fCount * r and
           std::abs(x[1]) <= fHeight / 2 and
           std::abs(x[2]) <= fWidth / 2;
}

auto Target::MultiLayerTarget::Contain(const Mustard::Concept::InputVector3D auto& x, bool insideVolume) const -> bool {
    switch (fDetailType) {
    case ShapeDetailType::Flat:
        return insideVolume;
    case ShapeDetailType::Perforated:
        return insideVolume and
               fPerforated.DetailContain(x);
    }
    muc::unreachable();
}

auto Target::MultiLayerTarget::DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    const auto x0{muc::even(fCount) ? fSpacing / 2 : -fThickness / 2};
    const auto r{fSpacing + fThickness};
    const auto u{(x[0] + x0) / r};
    const auto notShadowed{u - muc::llround(u - 0.5) < fSpacing / r or
                           std::abs(x[0]) > fCount * r or
                           std::abs(x[1]) > fHeight / 2 or
                           x[2] > fWidth / 2};
    switch (fDetailType) {
    case ShapeDetailType::Flat:
        return notShadowed;
    case ShapeDetailType::Perforated:
        return notShadowed or
               fPerforated.DetailDetectable(x);
    }
    muc::unreachable();
}

auto Target::MultiLayerTarget::PerforatedMultiLayer::DetailContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    if (std::abs(x[2]) > fHalfExtentZ or std::abs(x[1]) > fHalfExtentY) {
        return true;
    }
    using std::numbers::sqrt3;
    const auto p{Pitch()};

    const auto u0{p * muc::llround((x[2] - (1 / sqrt3) * x[1]) / p)};
    const auto v0{p * muc::llround((2 / sqrt3) * x[1] / p)};
    const auto z0{u0 + v0 / 2};
    const auto y0{(sqrt3 / 2) * v0};

    const auto deltaZ{x[2] - z0};
    const auto deltaY{x[1] - y0};
    const auto deltaZY2MinusR2{muc::pow(deltaZ, 2) + (deltaY + fRadius) * (deltaY - fRadius)};

    if (deltaZY2MinusR2 <= 0) {
        return false;
    }
    const auto deltaZY2MinusR2PlusP2{deltaZY2MinusR2 + muc::pow(p, 2)};
    const auto pDeltaZ{p * deltaZ};
    return deltaZY2MinusR2PlusP2 > std::abs(2 * pDeltaZ) and
           deltaZY2MinusR2PlusP2 > std::abs(pDeltaZ + sqrt3 * p * deltaY);
}

auto Target::CylinderTarget::VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    return -fThickness / 2 <= x[2] and x[2] <= fThickness / 2 and
           muc::hypot_sq(x[0], x[1]) <= muc::pow(fRadius, 2);
}

auto Target::CylinderTarget::DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    return x[2] > fThickness / 2 or
           muc::hypot_sq(x[0], x[1]) > muc::pow(fRadius, 2);
}

auto Target::VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    switch (fShapeType) {
    case TargetShapeType::Cuboid:
        return fCuboid.VolumeContain(x);
    case TargetShapeType::MultiLayer:
        return fMultiLayer.VolumeContain(x);
    case TargetShapeType::Cylinder:
        return fCylinder.VolumeContain(x);
    }
    muc::unreachable();
}

auto Target::Contain(const Mustard::Concept::InputVector3D auto& x, bool insideVolume) const -> bool {
    switch (fShapeType) {
    case TargetShapeType::Cuboid:
        return fCuboid.Contain(x, insideVolume);
    case TargetShapeType::MultiLayer:
        return fMultiLayer.Contain(x, insideVolume);
    case TargetShapeType::Cylinder:
        return fCylinder.Contain(x, insideVolume);
    }
    muc::unreachable();
}

auto Target::DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    switch (fShapeType) {
    case TargetShapeType::Cuboid:
        return fCuboid.DetectableAt(x);
    case TargetShapeType::MultiLayer:
        return fMultiLayer.DetectableAt(x);
    case TargetShapeType::Cylinder:
        return fCylinder.DetectableAt(x);
    }
    muc::unreachable();
}

} // namespace MACE::Detector::Description
