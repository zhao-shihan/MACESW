#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include "muc/math"

#include <string>
#include <utility>

namespace MACE::Detector::Description {

class Collimator final : public Mustard::Detector::Description::DescriptionBase<Collimator> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Collimator();
    ~Collimator() override = default;

public:
    // Geometry

    auto Enabled() const -> auto { return fEnabled; }
    auto Length() const -> auto { return fLength; }
    auto Radius() const -> auto { return fRadius; }
    auto Thickness() const -> auto { return fThickness; }
    auto Pitch() const -> auto { return fPitch; }
    auto Count() const -> auto { return muc::lltrunc((2 * fRadius - fThickness) / fPitch) + 1; }

    auto Enabled(bool val) -> void { fEnabled = val; }
    auto Length(auto val) -> void { fLength = val; }
    auto Radius(auto val) -> void { fRadius = val; }
    auto Thickness(auto val) -> void { fThickness = val; }
    auto Pitch(auto val) -> void { fPitch = val; }

    // Material

    auto MaterialName() const -> const auto& { return fMaterialName; }

    auto MaterialName(std::string val) { fMaterialName = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    bool fEnabled;
    double fLength;
    double fRadius;
    double fThickness;
    double fPitch;

    // Material

    std::string fMaterialName;
};

} // namespace MACE::Detector::Description
