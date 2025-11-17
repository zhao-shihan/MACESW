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
