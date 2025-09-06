#pragma once

#include "MACE/SimTarget/Messenger/AnalysisMessenger.h++"

#include "Mustard/Data/Tuple.h++"
#include "Mustard/Data/TupleModel.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "muc/array"
#include "muc/ptrvec"

#include "gsl/gsl"

#include <cstdio>
#include <filesystem>

class G4Run;

class TFile;

namespace MACE::SimTarget {

using MuoniumTrack = Mustard::Data::TupleModel<Mustard::Data::Value<int, "EvtID", "Event ID">,
                                               Mustard::Data::Value<int, "TrkID", "Track ID">,
                                               Mustard::Data::Value<int, "PDGID", "Particle PDG ID">,
                                               Mustard::Data::Value<float, "t0", "Vertex time">,
                                               Mustard::Data::Value<muc::array3f, "x0", "Vertex position">,
                                               Mustard::Data::Value<float, "Ek0", "Vertex kinetic energy">,
                                               Mustard::Data::Value<muc::array3f, "p0", "Vertex momentum">,
                                               Mustard::Data::Value<double, "t", "Decay time">,
                                               Mustard::Data::Value<muc::array3f, "x", "Decay position">,
                                               Mustard::Data::Value<float, "Ek", "Kinetic energy just before decay">,
                                               Mustard::Data::Value<muc::array3f, "p", "Momentum just before decay">>;

class Analysis final : public Mustard::Env::Memory::PassiveSingleton<Analysis> {
public:
    Analysis();
    ~Analysis();

    auto FilePath(std::filesystem::path path) -> void { fFilePath = std::move(path.replace_extension()); }
    auto FileMode(std::string mode) -> void { fFileMode = std::move(mode); }
    auto EnableYieldAnalysis(bool val) -> void { fEnableYieldAnalysis = val; }

    void RunBegin(gsl::not_null<const G4Run*> run);
    auto NewMuoniumTrack() { return fMuoniumTrack.emplace_back(std::make_unique_for_overwrite<Mustard::Data::Tuple<MuoniumTrack>>()).get(); }
    void RunEnd();

private:
    auto Open() -> void;
    auto Write() -> void;
    auto Close() -> void;

    auto OpenResultFile() -> void;
    auto WriteResult() -> void;
    auto CloseResultFile() -> void;

    auto OpenYieldFile() -> void;
    auto AnalysisAndWriteYield() -> void;
    auto CloseYieldFile() -> void;

private:
    std::filesystem::path fFilePath;
    std::string fFileMode;
    bool fEnableYieldAnalysis;

    const G4Run* fThisRun;

    gsl::owner<TFile*> fResultFile;
    muc::unique_ptrvec<Mustard::Data::Tuple<MuoniumTrack>> fMuoniumTrack;

    gsl::owner<std::FILE*> fYieldFile;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::SimTarget
