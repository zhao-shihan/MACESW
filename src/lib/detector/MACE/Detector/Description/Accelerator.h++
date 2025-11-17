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

class Accelerator final : public Mustard::Detector::Description::DescriptionBase<Accelerator> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Accelerator();
    ~Accelerator() override = default;

public:
    // Geometry

    auto UpstreamLength() const -> auto { return fUpstreamLength; }
    auto DownstreamLength() const -> auto { return fDownstreamLength; }
    auto ElectrodePitch() const -> auto { return fElectrodePitch; }
    auto ElectrodeInnerRadius() const -> auto { return fElectrodeInnerRadius; }
    auto ElectrodeOuterRadius() const -> auto { return fElectrodeOuterRadius; }
    auto ElectrodeThickness() const -> auto { return fElectrodeThickness; }
    auto FieldRadius() const -> auto { return fFieldRadius; }
    auto DecelerateFieldLength() const -> auto { return fDecelerateFieldLength; }
    auto AccelerateFieldLength() const -> auto { return fAccelerateFieldLength; }
    auto FullLength() const -> auto { return fUpstreamLength + fDownstreamLength; }
    auto NElectrode() const -> auto { return muc::lltrunc((FullLength() - fElectrodeThickness) / fElectrodePitch) + 1; }
    auto UpstreamFieldLength() const -> auto { return fDecelerateFieldLength - fMaxPotentialPosition; }
    auto DownstreamFieldLength() const -> auto { return fAccelerateFieldLength + fMaxPotentialPosition; }
    auto FullFieldLength() const -> auto { return fDecelerateFieldLength + fAccelerateFieldLength; }

    auto UpstreamLength(double val) -> void { fUpstreamLength = val; }
    auto DownstreamLength(double val) -> void { fDownstreamLength = val; }
    auto ElectrodePitch(double val) -> void { fElectrodePitch = val; }
    auto ElectrodeInnerRadius(double val) -> void { fElectrodeInnerRadius = val; }
    auto ElectrodeOuterRadius(double val) -> void { fElectrodeOuterRadius = val; }
    auto ElectrodeThickness(double val) -> void { fElectrodeThickness = val; }
    auto FieldRadius(double val) -> void { fFieldRadius = val; }
    auto DecelerateFieldLength(double val) -> void { fDecelerateFieldLength = val; }
    auto AccelerateFieldLength(double val) -> void { fAccelerateFieldLength = val; }

    // Material

    auto ElectrodeMaterialName() const -> const auto& { return fElectrodeMaterialName; }

    auto ElectrodeMaterialName(std::string val) -> void { fElectrodeMaterialName = std::move(val); }

    // Field

    auto MaxPotential() const -> auto { return fMaxPotential; }
    auto MaxPotentialPosition() const -> auto { return fMaxPotentialPosition; }

    auto MaxPotential(double val) -> void { fMaxPotential = val; }
    auto MaxPotentialPosition(double val) -> void { fMaxPotentialPosition = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fUpstreamLength;
    double fDownstreamLength;
    double fElectrodePitch;
    double fElectrodeInnerRadius;
    double fElectrodeOuterRadius;
    double fElectrodeThickness;
    double fFieldRadius;
    double fDecelerateFieldLength;
    double fAccelerateFieldLength;

    // Material

    std::string fElectrodeMaterialName;

    // Field

    double fMaxPotential;
    double fMaxPotentialPosition;
};

} // namespace MACE::Detector::Description
