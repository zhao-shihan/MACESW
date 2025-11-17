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

class Vacuum final : public Mustard::Detector::Description::DescriptionBase<Vacuum> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    Vacuum();
    ~Vacuum() override = default;

public:
    auto Name() const -> auto { return fName; }
    auto Pressure() const -> auto { return fPressure; }
    auto Density() const -> double;

    auto Name(std::string val) -> void { fName = std::move(val); }
    auto Pressure(double val) -> void { fPressure = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    std::string fName;
    double fPressure;
};

} // namespace MACE::Detector::Description
