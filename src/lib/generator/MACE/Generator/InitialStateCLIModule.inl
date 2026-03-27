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

namespace MACE::Generator {

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
InitialStateCLIModule<P, Ms...>::InitialStateCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli) :
    ModuleBase{cli} {
    if constexpr (sizeof...(Ms) == 1) {
        TheCLI()
            ->add_argument("-k", "--momentum")
            .help("Parent momentum")
            .default_value(std::vector{0., 0., 0.})
            .required()
            .nargs(3)
            .template scan<'g', double>();
        if constexpr (P == "polarized") {
            TheCLI()
                ->add_argument("-p", "--polarization")
                .help("Parent polarization vector")
                .default_value(std::vector{0., 0., 0.})
                .required()
                .nargs(3)
                .template scan<'g', double>();
        }
    } else {
        const std::array parent{Ms.sv()...};
        for (std::size_t i{}; i < sizeof...(Ms); ++i) {
            TheCLI()
                ->add_argument(fmt::format("-k{}", i), fmt::format("--momentum-{}", i))
                .help(fmt::format("Parent {} momentum.", parent[i]))
                .required()
                .nargs(3)
                .template scan<'g', double>();
        }
        if constexpr (P == "polarized") {
            for (std::size_t i{}; i < sizeof...(Ms); ++i) {
                TheCLI()
                    ->add_argument(fmt::format("-p{}", i), fmt::format("--polarization-{}", i))
                    .help(fmt::format("Parent {} polarization vector.", parent[i]))
                    .default_value(std::vector{0., 0., 0.})
                    .required()
                    .nargs(3)
                    .template scan<'g', double>();
            }
        }
    }
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Momentum() const -> Mustard::Vector3D
    requires(sizeof...(Ms) == 1)
{
    return To3Vector("--momentum");
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Momentum() const -> std::array<Mustard::Vector3D, sizeof...(Ms)>
    requires(sizeof...(Ms) >= 2)
{
    std::array<Mustard::Vector3D, sizeof...(Ms)> p;
    for (std::size_t i{}; i < sizeof...(Ms); ++i) {
        p[i] = To3Vector(fmt::format("--momentum-{}", i));
    }
    return p;
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Polarization() const -> Mustard::Vector3D
    requires(P == "polarized" and sizeof...(Ms) == 1)
{
    return To3Vector("--polarization");
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Polarization() const -> std::array<Mustard::Vector3D, sizeof...(Ms)>
    requires(P == "polarized" and sizeof...(Ms) >= 2)
{
    std::array<Mustard::Vector3D, sizeof...(Ms)> p;
    for (std::size_t i{}; i < sizeof...(Ms); ++i) {
        p[i] = To3Vector(fmt::format("--polarization-{}", i));
    }
    return p;
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::To3Vector(std::string_view option) const -> Mustard::Vector3D {
    const auto vector{TheCLI()->template get<std::vector<double>>(option)};
    return {vector[0], vector[1], vector[2]};
}

} // namespace MACE::Generator
