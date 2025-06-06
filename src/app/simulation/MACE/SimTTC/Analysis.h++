#pragma once

#include "MACE/Data/MMSTrack.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/Data/SensorHit.h++"
#include "MACE/Data/SimVertex.h++"
#include "MACE/SimTTC/Messenger/AnalysisMessenger.h++"
#include "MACE/Simulation/Analysis/MMSTruthTracker.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Simulation/AnalysisBase.h++"

#include "gsl/gsl"

#include <filesystem>
#include <memory>
#include <utility>

class TFile;

namespace MACE::inline Simulation::inline Hit {
class TTCHit;
class TTCSiPMHit;
} // namespace MACE::inline Simulation::inline Hit

namespace MACE::SimTTC {

class Analysis final : public Mustard::Simulation::AnalysisBase<Analysis, "SimTTC"> {
public:
    Analysis();

    auto SaveTTCHitData(bool val) -> void { fSaveTTCHitData = val; }
    auto SaveTTCSiPMHitData(bool val) -> void { fSaveTTCSiPMHitData = val; }

    auto SubmitPrimaryVertexData(const std::vector<std::unique_ptr<Mustard::Data::Tuple<Data::SimPrimaryVertex>>>& data) -> void { fPrimaryVertex = &data; }
    auto SubmitDecayVertexData(const std::vector<std::unique_ptr<Mustard::Data::Tuple<Data::SimDecayVertex>>>& data) -> void { fDecayVertex = &data; }
    auto SubmitTTCHC(const std::vector<gsl::owner<TTCHit*>>& hc) -> void { fTTCHit = &hc; }
    auto SubmitTTCSiPMHC(const std::vector<gsl::owner<TTCSiPMHit*>>& hc) -> void { fTTCSiPMHit = &hc; }

private:
    auto RunBeginUserAction(int runID) -> void override;
    auto EventEndUserAction() -> void override;
    auto RunEndUserAction(int) -> void override;

private:
    bool fSaveTTCHitData;
    bool fSaveTTCSiPMHitData;

    std::optional<Mustard::Data::Output<Data::SimPrimaryVertex>> fPrimaryVertexOutput;
    std::optional<Mustard::Data::Output<Data::SimDecayVertex>> fDecayVertexOutput;
    std::optional<Mustard::Data::Output<Data::TTCSimHit>> fTTCSimHitOutput;
    std::optional<Mustard::Data::Output<Data::TTCSiPMHit>> fTTCSiPMHitOutput;

    const std::vector<std::unique_ptr<Mustard::Data::Tuple<Data::SimPrimaryVertex>>>* fPrimaryVertex;
    const std::vector<std::unique_ptr<Mustard::Data::Tuple<Data::SimDecayVertex>>>* fDecayVertex;
    const std::vector<gsl::owner<TTCHit*>>* fTTCHit;
    const std::vector<gsl::owner<TTCSiPMHit*>>* fTTCSiPMHit;
    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::SimTTC
