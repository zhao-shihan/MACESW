#include "MACE/Detector/Description/CDC.h++"
#include "MACE/Detector/Description/MMSField.h++"
#include "MACE/Detector/Description/TTC.h++"
#include "MACE/SimMACE/Action/DetectorConstruction.h++"
#include "MACE/SimMACE/PhysicsList.h++"

#include "Mustard/Geant4X/DecayChannel/MuonInternalConversionDecayChannel.h++"
#include "Mustard/Geant4X/DecayChannel/MuoniumInternalConversionDecayChannel.h++"
#include "Mustard/Geant4X/Particle/Muonium.h++"
#include "Mustard/IO/PrettyLog.h++"
#include "Mustard/Utility/LiteralUnit.h++"
#include "Mustard/Utility/PhysicalConstant.h++"

#include "G4DecayTable.hh"
#include "G4MuonPlus.hh"

#include "muc/numeric"

#include "fmt/core.h"

#include <stdexcept>
#include <typeinfo>

namespace MACE::SimMACE {

using namespace Mustard::LiteralUnit::Energy;

PhysicsList::PhysicsList() :
    PassiveSingleton{this},
    StandardPhysicsListBase{},
    fMACEBiasPxySofteningFactor{0.5_MeV},
    fMACEBiasCosSofteningFactor{0.1},
    fMACEBiasEkLow{5_keV},
    fMACEBiasEkSofteningFactor{1_keV},
    fMessengerRegister{this} {}

auto PhysicsList::ApplyMACEBias(bool apply) -> void {
    auto& muonPlusICDecay{FindICDecayChannel<Mustard::Geant4X::MuonInternalConversionDecayChannel>(G4MuonPlus::Definition())};
    auto& muoniumICDecay{FindICDecayChannel<Mustard::Geant4X::MuoniumInternalConversionDecayChannel>(Mustard::Geant4X::Muonium::Definition())};
    if (apply) {
        const auto Bias{
            [this](auto&& event) {
                using Mustard::PhysicalConstant::c_light;
                using Mustard::PhysicalConstant::electron_mass_c2;

                const muc::soft_cmp scPxy{fMACEBiasPxySofteningFactor};
                const muc::soft_cmp scCos{fMACEBiasCosSofteningFactor};
                const muc::soft_cmp scEk{fMACEBiasEkSofteningFactor};

                const auto& cdc{Detector::Description::CDC::Instance()};
                const auto& ttc{Detector::Description::TTC::Instance()};
                const auto mmsB{Detector::Description::MMSField::Instance().FastField()};
                const auto inPxyCut{scPxy((cdc.GasInnerRadius() / 2) * mmsB * c_light)};
                const auto outPxyCut{scPxy((ttc.Radius() / 2) * mmsB * c_light)};
                const auto cosCut{scCos(1 / muc::hypot(2 * cdc.GasOuterRadius() / cdc.GasOuterLength(), 1.))};
                const auto lowEkCut{scEk(fMACEBiasEkLow)};

                // .         e+ e-  e+  v   v
                const auto& [p, p1, p2, k1, k2]{event};
                const auto p1Seen{scPxy(muc::hypot(p1.x(), p1.y())) > outPxyCut and scCos(muc::abs(p1.cosTheta())) < cosCut};
                const auto pMiss{scPxy(muc::hypot(p.x(), p.y())) < inPxyCut or scCos(muc::abs(p.cosTheta())) > cosCut};
                const auto p2Miss{scPxy(muc::hypot(p2.x(), p2.y())) < inPxyCut or scCos(muc::abs(p2.cosTheta())) > cosCut};
                const auto pLow{scEk(p.e() - electron_mass_c2) < lowEkCut};
                const auto p2Low{scEk(p2.e() - electron_mass_c2) < lowEkCut};
                return p1Seen and ((pMiss and p2Low) or (p2Miss and pLow));
            }};
        muonPlusICDecay.Bias(Bias);
        muoniumICDecay.Bias(Bias);
    } else {
        muonPlusICDecay.Bias([](auto&&) { return 1; });
        muoniumICDecay.Bias([](auto&&) { return 1; });
    }
}

template<std::derived_from<G4VDecayChannel> C, std::derived_from<G4ParticleDefinition> P>
auto PhysicsList::FindICDecayChannel(const P* particle) -> C& {
    const auto decayTable{particle->GetDecayTable()};
    C* ippDecay{};
    for (int i{}; i < decayTable->entries(); ++i) {
        if (const auto decay{dynamic_cast<C*>(decayTable->GetDecayChannel(i))}; decay) {
            ippDecay = decay;
            break;
        }
    }
    if (ippDecay == nullptr) {
        Mustard::Throw<std::logic_error>(fmt::format(
            "No decay channel '{}' in particle '{}'",
            typeid(C).name(), typeid(P).name()));
    }
    return *ippDecay;
}

} // namespace MACE::SimMACE
