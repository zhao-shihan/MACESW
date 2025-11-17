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

class SolenoidShield final : public Mustard::Detector::Description::DescriptionBase<SolenoidShield> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    SolenoidShield();
    ~SolenoidShield() override = default;

public:
    // Geometry

    auto InnerRadius() const -> auto { return fInnerRadius; }
    auto OuterRadius() const -> auto { return fOuterRadius; }

    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto OuterRadius(double val) -> void { fOuterRadius = val; }

    // Material

    auto MaterialName() const -> const auto& { return fMaterialName; }

    auto MaterialName(std::string val) -> void { fMaterialName = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fInnerRadius;
    double fOuterRadius;

    // Material

    std::string fMaterialName;
};

} // namespace MACE::Detector::Description
