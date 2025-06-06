#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/GenMuonICDecay/GenMuonICDecay.h++"

#include "Mustard/Env/CLI/MonteCarloCLI.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Extension/CLHEPX/Random/Xoshiro.h++"
#include "Mustard/Extension/Geant4X/DecayChannel/MuonInternalConversionDecayChannel.h++"
#include "Mustard/Extension/MPIX/Execution/Executor.h++"
#include "Mustard/Extension/MPIX/ParallelizePath.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/PhysicalConstant.h++"
#include "Mustard/Utility/UseXoshiro.h++"

#include "TFile.h"
#include "TNtuple.h"

#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4DecayProducts.hh"
#include "G4DynamicParticle.hh"
#include "G4Electron.hh"
#include "G4LorentzVector.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4ParticleTable.hh"
#include "G4Positron.hh"
#include "G4ThreeVector.hh"

#include "muc/numeric"
#include "muc/utility"

#include <bit>
#include <cmath>
#include <string>
#include <type_traits>

namespace MACE::GenMuonICDecay {

using namespace Mustard::LiteralUnit::Energy;
using namespace Mustard::PhysicalConstant;
using namespace std::string_literals;

GenMuonICDecay::GenMuonICDecay() :
    Subprogram{"GenMuonICDecay", "Generate muon internal conversion decay (mu->eeevv) events for physical study or test."} {}

auto GenMuonICDecay::Main(int argc, char* argv[]) const -> int {
    Mustard::Env::CLI::MonteCarloCLI<> cli;
    cli->add_argument("n").help("Number of events to generate.").nargs(1).scan<'i', unsigned long long>();
    cli->add_argument("-o", "--output").help("Output file path.").default_value("mu2eeevv.root"s).required().nargs(1);
    cli->add_argument("--output-mode").help("Output file creation mode (see ROOT documentation for details).").default_value("NEW"s).required().nargs(1);
    cli->add_argument("--compression-level").help("Output file compression level (see ROOT documentation for details).").default_value(muc::to_underlying(ROOT::RCompressionSetting::EDefaults::kUseGeneralPurpose)).required().nargs(1).scan<'i', std::underlying_type_t<ROOT::RCompressionSetting::EDefaults::EValues>>();
    cli->add_argument("-t", "--output-tree").help("Output tree name.").default_value("eeevv"s).required().nargs(1);
    cli->add_argument("-d", "--metropolis-delta").help("State step in Metropolis-Hasting sampling.").required().nargs(1).scan<'g', double>();
    cli->add_argument("-s", "--metropolis-discard").help("Number of states discarded between two samples in Metropolis-Hasting sampling.").required().nargs(1).scan<'i', int>();
    auto& cliMG0{cli->add_mutually_exclusive_group()};
    cliMG0.add_argument("-l", "--ep-ek-upper-bound").help("Add upper bound for energetic positron kinetic energy.").nargs(1).scan<'g', double>();
    cliMG0.add_argument("-b", "--bias").help("Enable bias (importance sampling).").flag();
    cli->add_argument("-p", "--pxy-softening-factor").help("Softening factor for transverse momentum soft comparision in bias.").default_value(0.2_MeV).required().nargs(1).scan<'g', double>();
    cli->add_argument("-c", "--cos-theta-softening-factor").help("Softening factor for momentum cosine soft comparision in bias.").default_value(0.05).required().nargs(1).scan<'g', double>();
    cli->add_argument("-e", "--ep-ek-soft-cut").help("Soft kinetic energy upper bound for energetic positron track in bias.").default_value(5_keV).required().nargs(1).scan<'g', double>();
    cli->add_argument("-f", "--ep-ek-softening-factor").help("Softening factor for energetic positron kinetic energy soft comparision in bias.").default_value(0.5_keV).required().nargs(1).scan<'g', double>();
    Mustard::Env::MPIEnv env{argc, argv, cli};

    Mustard::UseXoshiro<256> random;
    cli.SeedRandomIfFlagged();

    const auto filePath{Mustard::MPIX::ParallelizePath(cli->get("--output")).generic_string()};
    TFile file{filePath.c_str(), cli->get("--output-mode").c_str(), "",
               std::bit_cast<ROOT::RCompressionSetting::EDefaults::EValues>(
                   cli->get<std::underlying_type_t<ROOT::RCompressionSetting::EDefaults::EValues>>("--compression-level"))};
    if (not file.IsOpen()) {
        return EXIT_FAILURE;
    }
    const auto treeName{cli->get("--output-tree")};
    TNtuple nTuple{treeName.c_str(), treeName.c_str(), "Ek1:pT1:eta1:Ek2:pT2:eta2:Ek3:pT3:eta3:w"};

    G4ParticleTable::GetParticleTable()->SetReadiness();
    G4AntiNeutrinoE::Definition();
    G4AntiNeutrinoMu::Definition();
    G4Electron::Definition();
    G4MuonMinus::Definition();
    G4MuonPlus::Definition();
    G4NeutrinoE::Definition();
    G4NeutrinoMu::Definition();
    G4Positron::Definition();

    Mustard::Geant4X::MuonInternalConversionDecayChannel icDecay{"mu+", 1};
    icDecay.MetropolisDelta(cli->get<double>("--metropolis-delta"));
    icDecay.MetropolisDiscard(cli->get<int>("--metropolis-discard"));
    if (cli->present("--ep-ek-upper-bound")) {
        icDecay.Bias(
            [epEkUpperBound = cli->get<double>("--ep-ek-upper-bound")](auto&& event) {
                const auto& [p, p1, p2, k1, k2]{event};
                return p.e() < electron_mass_c2 + epEkUpperBound;
            });
    } else if (cli["--bias"] == true) {
        icDecay.Bias(
            [scPxy = muc::soft_cmp{cli->get<double>("--pxy-softening-factor")},
             scCos = muc::soft_cmp{cli->get<double>("--cos-theta-softening-factor")},
             epEkCut = cli->get<double>("--ep-ek-soft-cut"),
             scEk = muc::soft_cmp{cli->get<double>("--ep-ek-softening-factor")}](auto&& event) {
                const auto& cdc{Detector::Description::CDC::Instance()};
                const auto& ttc{Detector::Description::TTC::Instance()};
                const auto mmsB{Detector::Description::MMSField::Instance().FastField()};
                const auto inPxyCut{scPxy((cdc.GasInnerRadius() / 2) * mmsB * c_light)};
                const auto outPxyCut{scPxy((ttc.Radius() / 2) * mmsB * c_light)};
                const auto cosCut{scCos(1 / muc::hypot(2 * cdc.GasOuterRadius() / cdc.GasOuterLength(), 1.))};
                const auto lowEkCut{scEk(epEkCut)};

                // .         e+ e-  e+  v   v
                const auto& [p, p1, p2, k1, k2]{event};
                const auto p1Seen{scPxy(muc::hypot(p1.x(), p1.y())) > outPxyCut and scCos(muc::abs(p1.cosTheta())) < cosCut};
                const auto pMiss{scPxy(muc::hypot(p.x(), p.y())) < inPxyCut or scCos(muc::abs(p.cosTheta())) > cosCut};
                const auto p2Miss{scPxy(muc::hypot(p2.x(), p2.y())) < inPxyCut or scCos(muc::abs(p2.cosTheta())) > cosCut};
                const auto pLow{scEk(p.e() - electron_mass_c2) < lowEkCut};
                const auto p2Low{scEk(p2.e() - electron_mass_c2) < lowEkCut};
                return p1Seen and ((pMiss and p2Low) or (p2Miss and pLow));
            });
    }

    const auto Eta{
        [](const G4DynamicParticle* product) {
            const auto p{product->GetMomentum().mag()};
            const auto pz{product->GetMomentum().z()};
            return std::log((p + pz) / (p - pz)) / 2;
        }};

    Mustard::MPIX::Executor<unsigned long long> executor;
    icDecay.Initialize();
    executor.Execute(
        cli->get<unsigned long long>("n"),
        [&](auto) {
            const auto product{icDecay.DecayIt(muon_mass_c2)};
            const auto v2{product->PopProducts()};
            const auto v1{product->PopProducts()};
            const auto e3{product->PopProducts()};
            const auto e2{product->PopProducts()};
            const auto e1{product->PopProducts()};
            nTuple.Fill(e1->GetKineticEnergy(),
                        e1->GetMomentum().perp(),
                        Eta(e1),
                        e2->GetKineticEnergy(),
                        e2->GetMomentum().perp(),
                        Eta(e2),
                        e3->GetKineticEnergy(),
                        e3->GetMomentum().perp(),
                        Eta(e3),
                        icDecay.Weight());
            delete v2;
            delete v1;
            delete e3;
            delete e2;
            delete e1;
            delete product;
        });
    executor.PrintExecutionSummary();

    nTuple.Write();

    return EXIT_SUCCESS;
}

} // namespace MACE::GenMuonICDecay
