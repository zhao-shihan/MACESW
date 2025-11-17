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

namespace MACE::Detector::Description {

class BeamMonitor final : public Mustard::Detector::Description::DescriptionBase<BeamMonitor> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    BeamMonitor();
    ~BeamMonitor() override = default;

public:
    auto Enabled() const -> auto { return fEnabled; }
    auto Width() const -> auto { return fWidth; }
    auto Thickness() const -> auto { return fThickness; }
    auto DistanceToTarget() const -> auto { return fDistanceToTarget; }

    auto Enabled(bool val) -> void { fEnabled = val; }
    auto Width(double val) -> void { fWidth = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto DistanceToTarget(double val) -> void { fDistanceToTarget = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    bool fEnabled;
    double fWidth;
    double fThickness;
    double fDistanceToTarget;
};

} // namespace MACE::Detector::Description
