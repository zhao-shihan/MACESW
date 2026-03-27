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

#include "Mustard/CLI/CLI.h++"
#include "Mustard/CLI/Module/ModuleBase.h++"
#include "Mustard/Math/Vector.h++"

#include "muc/ceta_string"

#include "fmt/core.h"

#include <array>
#include <type_traits>
#include <vector>

namespace MACE::Generator {

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
class InitialStateCLIModule : public Mustard::CLI::ModuleBase {
public:
    explicit InitialStateCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli);

    auto Momentum() const -> Mustard::Vector3D
        requires(sizeof...(Ms) == 1);
    auto Momentum() const -> std::array<Mustard::Vector3D, sizeof...(Ms)>
        requires(sizeof...(Ms) >= 2);
    auto Polarization() const -> Mustard::Vector3D
        requires(P == "polarized" and sizeof...(Ms) == 1);
    auto Polarization() const -> std::array<Mustard::Vector3D, sizeof...(Ms)>
        requires(P == "polarized" and sizeof...(Ms) >= 2);

public:
    auto To3Vector(std::string_view option) const -> Mustard::Vector3D;
};

} // namespace MACE::Generator

#include "MACE/Generator/InitialStateCLIModule.inl"
