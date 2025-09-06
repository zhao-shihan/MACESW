#pragma once

#include "MACE/Data/SimVertex.h++"
#include "MACE/SimPTS/Hit/VirtualHit.h++"
#include "MACE/SimPTS/Messenger/AnalysisMessenger.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/Memory/PassiveSingleton.h++"

#include "G4Types.hh"

#include "muc/ptrvec"

#include "gsl/gsl"

#include <filesystem>
#include <memory>
#include <utility>

class TFile;

namespace MACE::SimPTS {

class Analysis final : public Mustard::Env::Memory::PassiveSingleton<Analysis> {
public:
    Analysis();

    auto FilePath(std::filesystem::path path) -> void { fFilePath = std::move(path); }
    auto FileMode(std::string mode) -> void { fFileMode = std::move(mode); }

    auto RunBegin(G4int runID) -> void;

    auto SubmitPrimaryVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>& data) -> void { fPrimaryVertex = &data; }
    auto SubmitDecayVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>& data) -> void { fDecayVertex = &data; }
    auto SubmitVirtualHC(const std::vector<VirtualHit*>& hc) -> void { fVirtualHit = &hc; }
    auto EventEnd() -> void;

    auto RunEnd(Option_t* option = {}) -> void;

private:
    std::filesystem::path fFilePath;
    std::string fFileMode;

    std::filesystem::path fLastUsedFullFilePath;

    gsl::owner<TFile*> fFile;
    std::optional<Mustard::Data::Output<Data::SimPrimaryVertex>> fPrimaryVertexOutput;
    std::optional<Mustard::Data::Output<Data::SimDecayVertex>> fDecayVertexOutput;
    std::optional<Mustard::Data::Output<VirtualHitModel>> fVirtualHitOutput;

    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>* fPrimaryVertex;
    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>* fDecayVertex;
    const std::vector<VirtualHit*>* fVirtualHit;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::SimPTS
