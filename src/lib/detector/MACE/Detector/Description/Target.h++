#pragma once

#include "Mustard/Concept/InputVector.h++"
#include "Mustard/Detector/Description/DescriptionBase.h++"
#include "Mustard/Env/Memory/WeakSingleton.h++"

#include "muc/math"
#include "muc/utility"

#include <algorithm>
#include <cmath>
#include <concepts>
#include <numbers>

class G4Material;

namespace MACE::Detector::Description {

class Target final : public Mustard::Detector::Description::DescriptionBase<Target> {
    friend Mustard::Env::Memory::SingletonInstantiator;

public:
    enum struct TargetShapeType {
        Cuboid,
        MultiLayer,
        Cylinder
    };

    template<typename ADerivedShape>
    class ShapeBase : public Mustard::Env::Memory::WeakSingleton<ADerivedShape> {
    protected:
        ShapeBase(ADerivedShape* derived);
        ~ShapeBase() = default;

    protected:
        template<typename ADerivedDetail>
        class DetailBase : public Mustard::Env::Memory::WeakSingleton<ADerivedDetail> {
        protected:
            DetailBase(ADerivedDetail* derived);
            ~DetailBase() = default;
        };
    };

    class CuboidTarget final : public ShapeBase<CuboidTarget> {
    public:
        enum struct ShapeDetailType {
            Flat,
            Perforated
        };

    public:
        CuboidTarget();

        auto Width() const -> auto { return fWidth; }
        auto Thickness() const -> auto { return fThickness; }

        auto Width(double val) -> void { fWidth = val; }
        auto Thickness(double val) -> void { fThickness = val; }

        auto DetailType() const -> auto { return fDetailType; }
        auto DetailType(ShapeDetailType val) -> void { fDetailType = val; }

        auto Perforated() const -> const auto& { return fPerforated; }
        auto Perforated() -> auto& { return fPerforated; }

        auto VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool;
        auto Contain(const Mustard::Concept::InputVector3D auto& x, bool insideVolume) const -> bool;
        auto DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool;

    private:
        class PerforatedCuboid final : public DetailBase<PerforatedCuboid> {
        public:
            PerforatedCuboid();

            auto HalfExtent() const { return fHalfExtent; }
            auto Extent() const { return 2 * fHalfExtent; }
            auto Spacing() const { return fSpacing; }
            auto Radius() const { return fRadius; }
            auto Diameter() const { return 2 * fRadius; }
            auto Depth() const { return fDepth; }
            auto Pitch() const { return fSpacing + Diameter(); }

            auto Extent(double ex) -> void { fHalfExtent = std::max(0., ex / 2); }
            auto Spacing(double spacing) -> void { fSpacing = std::max(0., spacing); }
            auto Diameter(double diameter) -> void { fRadius = std::max(0., diameter / 2); }
            auto Depth(double d) -> void { fDepth = d; }

            auto DetailContain(const Mustard::Concept::InputVector3D auto& x) const -> bool;
            auto DetailDetectable(const Mustard::Concept::InputVector3D auto&) const -> bool { return false; }

        private:
            double fHalfExtent;
            double fSpacing;
            double fRadius;
            double fDepth;
        };

    private:
        double fWidth;
        double fThickness;

        ShapeDetailType fDetailType;
        PerforatedCuboid fPerforated;
    };

    class MultiLayerTarget final : public ShapeBase<MultiLayerTarget> {
    public:
        enum struct ShapeDetailType {
            Flat,
            Perforated
        };

    public:
        MultiLayerTarget();

        auto Width() const -> auto { return fWidth; }
        auto Height() const -> auto { return fHeight; }
        auto Thickness() const -> auto { return fThickness; }
        auto Spacing() const -> auto { return fSpacing; }
        auto Count() const -> auto { return fCount; }

        auto Width(double val) -> void { fWidth = val; }
        auto Height(double val) -> void { fHeight = val; }
        auto Thickness(double val) -> void { fThickness = val; }
        auto Spacing(double val) -> void { fSpacing = val; }
        auto Count(int val) -> void { fCount = std::max(2, val); }

        auto DetailType() const -> auto { return fDetailType; }
        auto DetailType(ShapeDetailType val) -> void { fDetailType = val; }

        auto Perforated() const -> const auto& { return fPerforated; }
        auto Perforated() -> auto& { return fPerforated; }

        auto VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool;
        auto Contain(const Mustard::Concept::InputVector3D auto& x, bool insideVolume) const -> bool;
        auto DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool;

    private:
        class PerforatedMultiLayer final : public DetailBase<PerforatedMultiLayer> {
        public:
            PerforatedMultiLayer();

            auto HalfExtentZ() const { return fHalfExtentZ; }
            auto ExtentZ() const { return 2 * fHalfExtentZ; }
            auto HalfExtentY() const { return fHalfExtentY; }
            auto ExtentY() const { return 2 * fHalfExtentY; }
            auto Spacing() const { return fSpacing; }
            auto Radius() const { return fRadius; }
            auto Diameter() const { return 2 * fRadius; }
            auto Pitch() const { return fSpacing + Diameter(); }

            auto ExtentZ(double ex) -> void { fHalfExtentZ = std::max(0., ex / 2); }
            auto ExtentY(double ex) -> void { fHalfExtentY = std::max(0., ex / 2); }
            auto Spacing(double spacing) -> void { fSpacing = std::max(0., spacing); }
            auto Diameter(double diameter) -> void { fRadius = std::max(0., diameter / 2); }

            auto DetailContain(const Mustard::Concept::InputVector3D auto& x) const -> bool;
            auto DetailDetectable(const Mustard::Concept::InputVector3D auto&) const -> bool { return false; }

        private:
            double fHalfExtentZ;
            double fHalfExtentY;
            double fSpacing;
            double fRadius;
        };

    private:
        double fWidth;
        double fHeight;
        double fThickness;
        double fSpacing;
        int fCount;

        ShapeDetailType fDetailType;
        PerforatedMultiLayer fPerforated;
    };

    class CylinderTarget final : public ShapeBase<CylinderTarget> {
    public:
        CylinderTarget();

        auto Radius() const -> auto { return fRadius; }
        auto Thickness() const -> auto { return fThickness; }

        auto Radius(double val) -> void { fRadius = val; }
        auto Thickness(double val) -> void { fThickness = val; }

        auto VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool;
        auto Contain(const Mustard::Concept::InputVector3D auto&, bool insideVolume) const -> bool { return insideVolume; }
        auto DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool;

    private:
        double fRadius;
        double fThickness;
    };

private:
    Target();
    ~Target() = default;

public:
    auto ShapeType() const -> auto { return fShapeType; }
    auto ShapeType(TargetShapeType val) -> void { fShapeType = val; }

    auto Cuboid() const -> const auto& { return fCuboid; }
    auto Cuboid() -> auto& { return fCuboid; }
    auto MultiLayer() const -> const auto& { return fMultiLayer; }
    auto MultiLayer() -> auto& { return fMultiLayer; }
    auto Cylinder() const -> const auto& { return fCylinder; }
    auto Cylinder() -> auto& { return fCylinder; }

    auto SilicaAerogelDensity() const -> auto { return fSilicaAerogelDensity; }
    auto EffectiveTemperature() const -> auto { return fEffectiveTemperature; }
    auto FormationProbability() const -> auto { return fFormationProbability; }
    auto MeanFreePath() const -> auto { return fMeanFreePath; }

    auto SilicaAerogelDensity(double val) -> void { fSilicaAerogelDensity = val; }
    auto EffectiveTemperature(double val) -> void { fEffectiveTemperature = val; }
    auto FormationProbability(double val) -> void { fFormationProbability = val; }
    auto MeanFreePath(double val) -> void { fMeanFreePath = val; }

    auto Material() const -> G4Material*;

    /// @brief Return true if inside the target volume (include boundary (closed region), don't consider fine structure).
    auto VolumeContain(const Mustard::Concept::InputVector3D auto& x) const -> bool;
    /// @brief Return true if inside the exact target geometry (considering fine structure).
    auto Contain(const Mustard::Concept::InputVector3D auto& x, bool insideVolume) const -> bool;
    auto Contain(const Mustard::Concept::InputVector3D auto& x) const -> bool { return Contain(x, VolumeContain(x)); }
    /// @brief Return true if the decay position x is detectable (i.e. is not shadowed by target).
    auto DetectableAt(const Mustard::Concept::InputVector3D auto& x) const -> bool;

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    TargetShapeType fShapeType;
    CuboidTarget fCuboid;
    MultiLayerTarget fMultiLayer;
    CylinderTarget fCylinder;

    double fSilicaAerogelDensity;
    double fEffectiveTemperature;
    double fFormationProbability;
    double fMeanFreePath;
};

} // namespace MACE::Detector::Description

#include "MACE/Detector/Description/Target.inl"
