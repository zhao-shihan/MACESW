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

namespace MACE::SmearMACE {

template<Mustard::Data::TupleModelizable... Ts>
auto Smearer::Smear(std::string_view treeName, const muc::flat_hash_map<std::string, std::string>& smearingConfig) const -> void {
    Mustard::Data::Output<Ts...> output{std::string{treeName}};

    std::vector<std::pair<std::string_view, TF1>> smearAction;
    smearAction.reserve(smearingConfig.size());
    for (auto&& [var, smearFormula] : smearingConfig) {
        smearAction.emplace_back(var, TF1{fmt::format("{}Smearer", var).c_str(), smearFormula.c_str()});
    }

    fProcessor->Process<Ts...>(
        ROOT::RDataFrame{treeName, fInputFile}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            for (auto&& entry : event) {
                for (auto&& [var, smear] : smearAction) {
                    entry->Visit(var, [&](muc::arithmetic auto& x) { x = smear(x); });
                }
                output.Fill(*entry);
            }
        });

    output.Write();
}

} // namespace MACE::SmearMACE
