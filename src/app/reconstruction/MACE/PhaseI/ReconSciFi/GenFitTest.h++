#include "Mustard/Application/Subprogram.h++"

namespace MACE::PhaseI::ReconSciFi {

class GenFitTest : public Mustard::Application::Subprogram {
public:
    GenFitTest();

    auto Main(int argc, char* argv[]) const -> int override;
};

} // namespace MACE::PhaseI::ReconSciFi