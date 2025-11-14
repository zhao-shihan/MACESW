#pragma once

#include "Mustard/Application/Subprogram.h++"

namespace MACE::GenBkgM2ENNE {

class GenBkgM2ENNE : public Mustard::Application::Subprogram {
public:
    GenBkgM2ENNE();
    auto Main(int argc, char* argv[]) const -> int override;
};

} // namespace MACE::GenBkgM2ENNE
