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

namespace MACE::PhaseI::Detector::Description {

class World final : public Mustard::Detector::Description::DescriptionBase<World> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    World();
    ~World() override = default;

public:
    auto XExtent() const -> auto { return fXExtent; }
    auto YExtent() const -> auto { return fYExtent; }
    auto ZExtent() const -> auto { return fZExtent; }

    auto XExtent(double val) -> void { fXExtent = val; }
    auto YExtent(double val) -> void { fYExtent = val; }
    auto ZExtent(double val) -> void { fZExtent = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fXExtent;
    double fYExtent;
    double fZExtent;
};

} // namespace MACE::PhaseI::Detector::Description
