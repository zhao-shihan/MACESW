#include "MACE/SmearMACE/Smearer.h++"

namespace MACE::SmearMACE {

Smearer::Smearer(std::vector<std::string> inputFile, Mustard::Data::Processor<>& processor) :
    fInputFile{std::move(inputFile)},
    fProcessor{&processor} {}

} // namespace MACE::SmearMACE
