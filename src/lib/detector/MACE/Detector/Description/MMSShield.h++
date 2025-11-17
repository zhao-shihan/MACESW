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

#include <string>
#include <utility>

namespace MACE::Detector::Description {

class MMSShield final : public Mustard::Detector::Description::DescriptionBase<MMSShield> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MMSShield();
    ~MMSShield() override = default;

public:
    // Geometry

    auto InnerRadius() const -> auto { return fInnerRadius; }
    auto InnerLength() const -> auto { return fInnerLength; }
    auto Thickness() const -> auto { return fThickness; }
    auto WindowRadius() const -> auto { return fWindowRadius; }
    auto BeamSlantAngle() const -> auto { return fBeamSlantAngle; }

    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto InnerLength(double val) -> void { fInnerLength = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto WindowRadius(double val) -> void { fWindowRadius = val; }
    auto BeamSlantAngle(double val) -> void { fBeamSlantAngle = val; }

    // Material

    auto MaterialName() const -> const auto& { return fMaterialName; }

    auto MaterialName(std::string val) -> void { fMaterialName = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fInnerRadius;
    double fInnerLength;
    double fThickness;
    double fWindowRadius;
    double fBeamSlantAngle;

    // Material

    std::string fMaterialName;
};

} // namespace MACE::Detector::Description
