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

namespace MACE::PhaseI::Detector::Description {

class CentralBeamPipe final : public Mustard::Detector::Description::DescriptionBase<CentralBeamPipe> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    CentralBeamPipe();
    ~CentralBeamPipe() override = default;

public:
    // Geometry

    auto Length() const -> auto { return fLength; }
    auto InnerRadius() const -> auto { return fInnerRadius; }
    auto BerylliumLength() const -> auto { return fBerylliumLength; }
    auto BerylliumThickness() const -> auto { return fBerylliumThickness; }
    auto AluminiumThickness() const -> auto { return fAluminiumThickness; }

    auto Length(double val) -> void { fLength = val; }
    auto InnerRadius(double val) -> void { fInnerRadius = val; }
    auto BerylliumLength(double val) -> void { fBerylliumLength = val; }
    auto BerylliumThickness(double val) -> void { fBerylliumThickness = val; }
    auto AluminiumThickness(double val) -> void { fAluminiumThickness = val; }

    // Material

    auto VacuumPressure() const -> auto { return fVacuumPressure; }
    auto VacuumDensity() const -> double;

    auto VacuumPressure(double val) -> void { fVacuumPressure = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    double fLength;
    double fInnerRadius;
    double fBerylliumLength;
    double fBerylliumThickness;
    double fAluminiumThickness;

    // Material

    double fVacuumPressure;
};

} // namespace MACE::PhaseI::Detector::Description
