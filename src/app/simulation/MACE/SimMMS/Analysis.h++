#pragma once

#include "MACE/Data/MMSTrack.h++"
#include "MACE/Data/SensorHit.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/Data/SimVertex.h++"
#include "MACE/SimMMS/Messenger/AnalysisMessenger.h++"
#include "MACE/Simulation/Analysis/MMSTruthTracker.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Simulation/AnalysisBase.h++"

#include "muc/ptrvec"

#include "gsl/gsl"

#include <filesystem>
#include <utility>
#include <vector>

class TFile;

namespace MACE::inline Simulation::inline Hit {
class CDCHit;
class TTCHit;
class TTCSiPMHit;
} // namespace MACE::inline Simulation::inline Hit

namespace MACE::SimMMS {

class Analysis final : public Mustard::Simulation::AnalysisBase<Analysis, "SimMMS"> {
public:
    Analysis();

    auto SaveCDCHitData(bool val) -> void { fSaveCDCHitData = val; }
    auto SaveTTCHitData(bool val) -> void { fSaveTTCHitData = val; }
    auto SaveTTCSiPMHitData(bool val) -> void { fSaveTTCSiPMHitData = val; }

    auto SubmitPrimaryVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>& data) -> void { fPrimaryVertex = &data; }
    auto SubmitDecayVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>& data) -> void { fDecayVertex = &data; }
    auto SubmitCDCHC(const std::vector<gsl::owner<CDCHit*>>& hc) -> void { fCDCHit = &hc; }
    auto SubmitTTCHC(const std::vector<gsl::owner<TTCHit*>>& hc) -> void { fTTCHit = &hc; }
    auto SubmitTTCSiPMHC(const std::vector<gsl::owner<TTCSiPMHit*>>& hc) -> void { fTTCSiPMHit = &hc; }

private:
    auto RunBeginUserAction(int runID) -> void override;
    auto EventEndUserAction() -> void override;
    auto RunEndUserAction(int) -> void override;

private:
    bool fSaveCDCHitData;
    bool fSaveTTCHitData;
    bool fSaveTTCSiPMHitData;

    std::optional<Mustard::Data::Output<Data::SimPrimaryVertex>> fPrimaryVertexOutput;
    std::optional<Mustard::Data::Output<Data::SimDecayVertex>> fDecayVertexOutput;
    std::optional<Mustard::Data::Output<Data::TTCSimHit>> fTTCSimHitOutput;
    std::optional<Mustard::Data::Output<Data::TTCSiPMHit>> fTTCSiPMHitOutput;
    std::optional<Mustard::Data::Output<Data::CDCSimHit>> fCDCSimHitOutput;
    std::optional<Mustard::Data::Output<Data::MMSSimTrack>> fMMSSimTrackOutput;

    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimPrimaryVertex>>* fPrimaryVertex;
    const muc::unique_ptrvec<Mustard::Data::Tuple<Data::SimDecayVertex>>* fDecayVertex;
    const std::vector<gsl::owner<CDCHit*>>* fCDCHit;
    const std::vector<gsl::owner<TTCHit*>>* fTTCHit;
    const std::vector<gsl::owner<TTCSiPMHit*>>* fTTCSiPMHit;

    Simulation::Analysis::MMSTruthTracker fMMSTruthTracker;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::SimMMS
