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

    Mustard::Data::Processor<>& fProcessor;
};

} // namespace MACE::SmearMACE

#include "MACE/SmearMACE/Smearer.inl"
