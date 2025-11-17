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

class MMSField final : public Mustard::Detector::Description::DescriptionBase<MMSField> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MMSField();
    ~MMSField() override = default;

public:
    ///////////////////////////////////////////////////////////
    // Geometry
    ///////////////////////////////////////////////////////////

    auto Radius() const -> auto { return fRadius; }
    auto Length() const -> auto { return fLength; }

    auto Radius(double val) -> void { fRadius = val; }
    auto Length(double val) -> void { fLength = val; }

    ///////////////////////////////////////////////////////////
    // Field
    ///////////////////////////////////////////////////////////

    auto FastField() const -> auto { return fFastField; }

    auto FastField(double v) -> void { fFastField = v; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    ///////////////////////////////////////////////////////////
    // Geometry
    ///////////////////////////////////////////////////////////

    double fRadius;
    double fLength;

    ///////////////////////////////////////////////////////////
    // Field
    ///////////////////////////////////////////////////////////

    double fFastField;
};

} // namespace MACE::Detector::Description
