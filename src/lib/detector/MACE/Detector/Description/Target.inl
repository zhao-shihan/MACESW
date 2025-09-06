namespace MACE::Detector::Description {

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
    return -fThickness <= x[2] and x[2] <= 0 and
           muc::abs(x[0]) <= fWidth / 2 and
           muc::abs(x[1]) <= fWidth / 2;
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
    const auto notShadowed{x[2] > 0 or
                           muc::abs(x[0]) > fWidth / 2 or
                           muc::abs(x[1]) > fWidth / 2};
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
    if (x[2] < -fDepth or muc::abs(x[0]) > fHalfExtent or muc::abs(x[1]) > fHalfExtent) {
        return true;
    }
    using std::numbers::sqrt3;
    const auto p{Pitch()};

    const auto u0{p * muc::llround((x[0] - (1 / sqrt3) * x[1]) / p)};
    const auto v0{p * muc::llround((2 / sqrt3) * x[1] / p)};
    const auto x0{u0 + v0 / 2};
    const auto y0{(sqrt3 / 2) * v0};

    const auto deltaX{x[0] - x0};
    const auto deltaY{x[1] - y0};
    const auto deltaXY2MinusR2{muc::pow(deltaX, 2) + (deltaY + fRadius) * (deltaY - fRadius)};

    if (deltaXY2MinusR2 <= 0) {
        return false;
    }
    const auto deltaXY2MinusR2PlusP2{deltaXY2MinusR2 + muc::pow(p, 2)};
    const auto pDeltaX{p * deltaX};
    return deltaXY2MinusR2PlusP2 > muc::abs(2 * pDeltaX) and
           deltaXY2MinusR2PlusP2 > muc::abs(pDeltaX + sqrt3 * p * deltaY);
}

auto Target::MultiLayerTarget::VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    const auto x0{muc::even(fCount) ? fSpacing / 2 : -fThickness / 2};
    const auto r{fSpacing + fThickness};
    const auto u{(x[0] + x0) / r};
    return u - muc::llround(u - 0.5) >= fSpacing / r and
           muc::abs(x[0]) <= fCount * r and
           muc::abs(x[1]) <= fHeight / 2 and
           muc::abs(x[2]) <= fWidth / 2;
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
                           muc::abs(x[0]) > fCount * r or
                           muc::abs(x[1]) > fHeight / 2 or
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
    if (muc::abs(x[2]) > fHalfExtentZ or muc::abs(x[1]) > fHalfExtentY) {
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
    return deltaZY2MinusR2PlusP2 > muc::abs(2 * pDeltaZ) and
           deltaZY2MinusR2PlusP2 > muc::abs(pDeltaZ + sqrt3 * p * deltaY);
}

auto Target::CylinderTarget::VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    return -fThickness / 2 <= x[2] and x[2] <= fThickness / 2 and
           muc::hypot_sq(x[0], x[1]) <= muc::pow(fRadius, 2);
}

auto Target::CylinderTarget::DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool {
    return x[2] > fThickness / 2 or
           muc::hypot_sq(x[0], x[1]) > muc::pow(fRadius, 2);
}

} // namespace MACE::Detector::Description
