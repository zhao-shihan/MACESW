#pragma once

#include "MACE/Data/SensorHit.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/Data/SimVertex.h++"
#include "MACE/PhaseI/Data/SensorHit.h++"
#include "MACE/PhaseI/Data/SimHit.h++"
#include "MACE/PhaseI/SimMACEPhaseI/Messenger/AnalysisMessenger.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Simulation/AnalysisBase.h++"

#include "G4Types.hh"

#include "muc/ptrvec"

#include "gsl/gsl"

#include <filesystem>
#include <optional>
#include <utility>
#include <vector>

class TFile;

namespace MACE::PhaseI::inline Simulation::inline Hit {
class MRPCHit;
class SciFiHit;
class SciFiSiPMRawHit;
} // namespace MACE::PhaseI::inline Simulation::inline Hit

namespace MACE::inline Simulation::inline Hit {
class ECALHit;
class ECALPMHit;
} // namespace MACE::inline Simulation::inline Hit

namespace MACE::PhaseI::SimMACEPhaseI {

class Analysis final : public Mustard::Simulation::AnalysisBase<Analysis, "SimMACEPhaseI"> {
public:
    Analysis();

    auto CoincidenceWithMRPC(G4bool val) -> void { fCoincidenceWithMRPC = val; }
    auto CoincidenceWithECAL(G4bool val) -> void { fCoincidenceWithECAL = val; }

    auto SubmitPrimaryVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<MACE::Data::SimPrimaryVertex>>& data) -> void { fPrimaryVertex = &data; }
    auto SubmitDecayVertexData(const muc::unique_ptrvec<Mustard::Data::Tuple<MACE::Data::SimDecayVertex>>& data) -> void { fDecayVertex = &data; }
    auto SubmitMRPCHC(const std::vector<gsl::owner<MRPCHit*>>& hc) -> void { fMRPCHit = &hc; }
    auto SubmitECALHC(const std::vector<gsl::owner<ECALHit*>>& hc) -> void { fECALHit = &hc; }
    auto SubmitECALPMHC(const std::vector<gsl::owner<ECALPMHit*>>& hc) -> void { fECALPMHit = &hc; }
    auto SubmitSciFiHC(const std::vector<gsl::owner<SciFiHit*>>& hc) -> void { fSciFiHit = &hc; }
    auto SubmitSciFiSiPMHC(const std::vector<gsl::owner<SciFiSiPMRawHit*>>& hc) -> void { fSciFiSiPMHit = &hc; }

    auto RunBeginUserAction(int runID) -> void override;
    auto EventEndUserAction() -> void override;
    auto RunEndUserAction(int) -> void override;

private:
    G4bool fCoincidenceWithMRPC;
    G4bool fCoincidenceWithECAL;

    std::optional<Mustard::Data::Output<MACE::Data::SimPrimaryVertex>> fPrimaryVertexOutput;
    std::optional<Mustard::Data::Output<MACE::Data::SimDecayVertex>> fDecayVertexOutput;
    std::optional<Mustard::Data::Output<MACE::PhaseI::Data::MRPCSimHit>> fMRPCSimHitOutput;
    std::optional<Mustard::Data::Output<MACE::Data::ECALSimHit>> fECALSimHitOutput;
    std::optional<Mustard::Data::Output<MACE::Data::ECALPMHit>> fECALPMHitOutput;
    std::optional<Mustard::Data::Output<MACE::PhaseI::Data::SciFiSimHit>> fSciFiHitOutput;
    std::optional<Mustard::Data::Output<MACE::PhaseI::Data::SciFiSiPMRawHit>> fSciFiSiPMHitOutput;

    const muc::unique_ptrvec<Mustard::Data::Tuple<MACE::Data::SimPrimaryVertex>>* fPrimaryVertex;
    const muc::unique_ptrvec<Mustard::Data::Tuple<MACE::Data::SimDecayVertex>>* fDecayVertex;
    const std::vector<gsl::owner<MRPCHit*>>* fMRPCHit;
    const std::vector<gsl::owner<ECALHit*>>* fECALHit;
    const std::vector<gsl::owner<ECALPMHit*>>* fECALPMHit;
    const std::vector<gsl::owner<SciFiHit*>>* fSciFiHit;
    const std::vector<gsl::owner<SciFiSiPMRawHit*>>* fSciFiSiPMHit;

    AnalysisMessenger::Register<Analysis> fMessengerRegister;
};

} // namespace MACE::PhaseI::SimMACEPhaseI
