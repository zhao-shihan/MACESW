#pragma once

#include "Mustard/Application/Subprogram.h++"

namespace MACE::SimTTC {

class SimTTC : public Mustard::Application::Subprogram {
public:
    SimTTC();
    auto Main(int argc, char* argv[]) const -> int override;
};

} // namespace MACE::SimTTC
