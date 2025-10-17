#pragma once

#include "Mustard/Application/Subprogram.h++"

namespace MACE::GenM2ENNE {

class GenM2ENNE : public Mustard::Application::Subprogram {
public:
    GenM2ENNE();
    auto Main(int argc, char* argv[]) const -> int override;
};

} // namespace MACE::GenM2ENNE
