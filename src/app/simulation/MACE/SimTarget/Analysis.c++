#include "MACE/Detector/Description/Target.h++"
#include "MACE/SimTarget/Action/PrimaryGeneratorAction.h++"
#include "MACE/SimTarget/Analysis.h++"
#include "MACE/SimTarget/Messenger/AnalysisMessenger.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Geant4X/Utility/ConvertGeometry.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"

#include "TFile.h"
#include "TMacro.h"

#include "G4Run.hh"

#include "mplr/mplr.hpp"

#include "fmt/format.h"

#include <algorithm>
#include <functional>
#include <stdexcept>

namespace MACE::SimTarget {

Analysis::Analysis() :
    PassiveSingleton{this},
    fFilePath{"SimTarget_untitled"},
    fFileMode{"NEW"},
    fEnableYieldAnalysis{true},
    fThisRun{},
    fResultFile{},
    fMuoniumTrack{},
    fYieldFile{},
    fMessengerRegister{this} {}

Analysis::~Analysis() {
    Close();
}

auto Analysis::RunBegin(gsl::not_null<const G4Run*> run) -> void {
    fThisRun = run;
    const auto runID{fThisRun->GetRunID()};
    if (runID == 0) {
        Open();
    }
    const auto runDirectory{fmt::format("G4Run{}", runID)};
    fResultFile->mkdir(runDirectory.c_str());
    fResultFile->cd(runDirectory.c_str());
}

auto Analysis::RunEnd() -> void {
    Write();
}

auto Analysis::Open() -> void {
    OpenResultFile();
    if (fEnableYieldAnalysis) {
        OpenYieldFile();
    }
}

auto Analysis::Write() -> void {
    WriteResult();
    if (fEnableYieldAnalysis) {
        AnalysisAndWriteYield();
    }
    fMuoniumTrack.clear();
}

auto Analysis::Close() -> void {
    CloseResultFile();
    if (fEnableYieldAnalysis) {
        CloseYieldFile();
    }
}

auto Analysis::OpenResultFile() -> void {
    const auto fullFilePath{Mustard::Parallel::ProcessSpecificPath(fFilePath).replace_extension(".root").generic_string()};
    fResultFile = TFile::Open(fullFilePath.c_str(), fFileMode.c_str(),
                              "", ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose);
    if (fResultFile == nullptr) {
        Mustard::Throw<std::runtime_error>(fmt::format("Cannot open file '{}' with mode '{}'",
                                                       fullFilePath, fFileMode));
    }
    if (mplr::comm_world().rank() == 0) {
        Mustard::Geant4X::ConvertGeometryToTMacro("SimTarget_gdml", "SimTarget.gdml")->Write();
    }
}

auto Analysis::WriteResult() -> void {
    Mustard::Data::Output<MuoniumTrack> output{"MuoniumTrack"};
    output.Fill(fMuoniumTrack);
    output.Write();
}

auto Analysis::CloseResultFile() -> void {
    if (fResultFile == nullptr) {
        return;
    }
    fResultFile->Close();
    delete fResultFile;
}

auto Analysis::OpenYieldFile() -> void {
    if (mplr::comm_world().rank() == 0) {
        fYieldFile = std::fopen(std::string{fFilePath}.append("_yield.csv").c_str(), "w");
        fmt::println(fYieldFile, "runID,nMuon,nMFormed,nMTargetDecay,nMVacuumDecay,nMDetectableDecay");
    }
}

auto Analysis::AnalysisAndWriteYield() -> void {
    std::array<unsigned long long, 5> yieldData;
    auto& [nMuon, nFormed, nTargetDecay, nVacuumDecay, nDetectableDecay]{yieldData};
    nMuon = static_cast<unsigned long long>(PrimaryGeneratorAction::Instance().NVertex()) *
            static_cast<unsigned long long>(fThisRun->GetNumberOfEvent());
    nFormed = fMuoniumTrack.size();
    nTargetDecay = 0;
    nVacuumDecay = 0;
    nDetectableDecay = 0;

    const auto& target{Detector::Description::Target::Instance()};
    for (auto&& track : std::as_const(fMuoniumTrack)) {
        const auto& decayPosition{Get<"x">(*track).As<muc::array3d>()};
        if (target.Contain(decayPosition)) {
            ++nTargetDecay;
        } else {
            ++nVacuumDecay;
            if (target.DetectableAt(decayPosition)) {
                ++nDetectableDecay;
            }
        }
    }

    const auto& worldComm{mplr::comm_world()};
    worldComm.reduce(
        [](const std::array<unsigned long long, 5>& a, const std::array<unsigned long long, 5>& b) {
            std::array<unsigned long long, 5> c;
            std::ranges::transform(a, b, c.begin(), std::plus{});
            return c;
        },
        0, yieldData);
    if (worldComm.rank() == 0) {
        const auto& [nMuon, nFormed, nTargetDecay, nVacuumDecay, nDetectableDecay]{yieldData};
        fmt::println(fYieldFile, "{},{},{},{},{},{}", fThisRun->GetRunID(), nMuon, nFormed, nTargetDecay, nVacuumDecay, nDetectableDecay);
    }
}

auto Analysis::CloseYieldFile() -> void {
    if (fYieldFile == nullptr) {
        return;
    }
    std::fclose(fYieldFile);
}

} // namespace MACE::SimTarget
