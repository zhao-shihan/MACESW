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

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/TupleModel.h++"

#include "ROOT/RDataFrame.hxx"
#include "TF1.h"

#include "muc/concepts"
#include "muc/hash_map"

#include "fmt/core.h"

#include <string>
#include <vector>

namespace MACE::SmearMACE {

class Smearer {
public:
    Smearer(std::vector<std::string> inputFile, Mustard::Data::Processor<>& processor);

    template<Mustard::Data::TupleModelizable... Ts>
    auto Smear(std::string_view treeName, const muc::flat_hash_map<std::string, std::string>& smearingConfig) const -> void;

private:
    std::vector<std::string> fInputFile;

    Mustard::Data::Processor<>* fProcessor;
};

} // namespace MACE::SmearMACE

#include "MACE/SmearMACE/Smearer.inl"
