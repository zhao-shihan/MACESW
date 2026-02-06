#include "MACE/Detector/Definition/ECALCrystal.h++"
#include "MACE/Detector/Definition/ECALPhotoSensor.h++"
#include "MACE/Detector/Definition/Target.h++"
#include "MACE/Detector/Description/ECAL.h++"
#include "MACE/MakeGeometry/MakeGeometry.h++"
#include "MACE/PhaseI/Detector/Definition/CentralBeamPipe.h++"
#include "MACE/PhaseI/Detector/Definition/MRPC.h++"
#include "MACE/PhaseI/Detector/Definition/SciFiTracker.h++"
#include "MACE/PhaseI/Detector/Definition/TTC.h++"
#include "MACE/PhaseI/Detector/Definition/World.h++"
#include "MACE/PhaseI/Detector/Description/UsePhaseIDefault.h++"
#include "MACE/PhaseI/MakeGeometry/MakeGeometry.h++"

#include "Mustard/CLI/BasicCLI.h++"
#include "Mustard/Detector/Description/DescriptionIO.h++"
#include "Mustard/Env/BasicEnv.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"

#include "TGeoManager.h"

#include "fmt/std.h"

#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>

namespace MACE::PhaseI::MakeGeometry {

using namespace std::string_literals;

MakeGeometry::MakeGeometry() :
    Subprogram{"MakeGeometry", "Construct detector geometry and create geometry files."} {}

auto MakeGeometry::Main(int argc, char* argv[]) const -> int {

    Mustard::CLI::BasicCLI<> cli;
    cli->add_argument("-o", "--output").help("Set output directory path.").default_value("macephasei_geometry"s).required().nargs(1);
    cli->add_argument("-c", "--opaque").help("Set geometry opacity.").default_value(false).required().nargs(1);
    Mustard::Env::BasicEnv env(argc, argv, cli);

    const std::filesystem::path outputPath{cli->get("--output")};
    if (std::filesystem::exists(outputPath)) {
        Mustard::Throw<std::runtime_error>(fmt::format("{} already exists", outputPath));
    }
    std::filesystem::create_directories(outputPath);

    ////////////////////////////////////////////////////////////////
    // Construct volumes
    ////////////////////////////////////////////////////////////////

    using namespace Detector::Definition;

    constexpr auto fCheckOverlap{false};
    Detector::Description::UsePhaseIDefault();
    // 0
    const auto fWorld{std::make_unique_for_overwrite<World>()};

    // 1

    auto& ecalCrystal{fWorld->NewDaughter<MACE::Detector::Definition::ECALCrystal>(fCheckOverlap)};
    auto& ecalPhotoSensor{fWorld->NewDaughter<MACE::Detector::Definition::ECALPhotoSensor>(fCheckOverlap)};
    auto& centralBeamPipe{fWorld->NewDaughter<PhaseI::Detector::Definition::CentralBeamPipe>(fCheckOverlap)};
    auto& ttc{fWorld->NewDaughter<MACE::PhaseI::Detector::Definition::TTC>(fCheckOverlap)};
    auto& sciFiTracker{fWorld->NewDaughter<PhaseI::Detector::Definition::SciFiTracker>(fCheckOverlap)};

    centralBeamPipe.NewDaughter<MACE::Detector::Definition::Target>(fCheckOverlap);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // Mustard::Detector::Description::DescriptionIO::ExportInstantiated("test.yaml");

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    fWorld->Export(outputPath / "macephasei.gdml");
    // mms.Get<CDCSenseLayer>().RemoveDaughter<CDCCell>(); // ROOT does not support twisted tube.
    // fWorld->Export(outputPath / "mace_root_compatible.gdml");

    const auto geoManager{std::unique_ptr<TGeoManager>{TGeoManager::Import((outputPath / "macephasei.gdml").generic_string().c_str())}};
    // geoManager->SetNameTitle("MACEPhaseIGeometry", "MACEPhaseI Geometry");
    geoManager->SetMaxVisNodes(0);

    // if (cli->get<bool>("--opaque")) {
    //     // set transparency for jsroot display
    //     // see form https://github.com/root-project/jsroot/blob/master/docs/JSROOT.md#geometry-viewer
    //     geoManager->GetVolume(fWorld->LogicalVolume()->GetName())->SetInvisible();
    //     using Mustard::Detector::Definition::DefinitionBase;
    //     for (auto&& entity : std::initializer_list<std::reference_wrapper<const DefinitionBase>>{
    //              sciFiTracker
    //,
    // ecalPhotoSensor,
    // ecalCrystal,
    // mrpc,
    // centralBeamPipe
    // }) {
    // geoManager->GetVolume(entity.get().LogicalVolume()->GetName())->SetTransparency(60);
    // }
    // }

    geoManager->Export((outputPath / "macephasei.root").generic_string().c_str());

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    return EXIT_SUCCESS;
}

} // namespace MACE::PhaseI::MakeGeometry
