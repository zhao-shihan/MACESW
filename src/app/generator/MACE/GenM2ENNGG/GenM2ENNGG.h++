#pragma once

#include "Mustard/Application/Subprogram.h++"

namespace MACE::GenM2ENNGG {

class GenM2ENNGG : public Mustard::Application::Subprogram {
public:
    GenM2ENNGG();
    auto Main(int argc, char* argv[]) const -> int override;
};

} // namespace MACE::GenM2ENNGG
