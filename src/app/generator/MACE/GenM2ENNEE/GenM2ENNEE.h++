#pragma once

#include "Mustard/Application/Subprogram.h++"

namespace MACE::GenM2ENNEE {

class GenM2ENNEE : public Mustard::Application::Subprogram {
public:
    GenM2ENNEE();
    auto Main(int argc, char* argv[]) const -> int override;
};

} // namespace MACE::GenM2ENNEE
