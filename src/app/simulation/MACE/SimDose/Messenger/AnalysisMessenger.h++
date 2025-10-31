#pragma once

#include "Mustard/Geant4X/Interface/SingletonMessenger.h++"

#include <memory>

class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIdirectory;

namespace MACE::SimDose {

class Analysis;

inline namespace Messenger {

class AnalysisMessenger final : public Mustard::Geant4X::SingletonMessenger<AnalysisMessenger,
                                                                            Analysis> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    AnalysisMessenger();
    ~AnalysisMessenger() override;

public:
    auto SetNewValue(G4UIcommand* command, G4String value) -> void override;

private:
    std::unique_ptr<G4UIdirectory> fDirectory;

    std::unique_ptr<G4UIcmdWithAString> fAddMap;
    std::unique_ptr<G4UIcmdWithAnInteger> fMapNBinX;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMapXMin;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMapXMax;
    std::unique_ptr<G4UIcmdWithAnInteger> fMapNBinY;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMapYMin;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMapYMax;
    std::unique_ptr<G4UIcmdWithAnInteger> fMapNBinZ;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMapZMin;
    std::unique_ptr<G4UIcmdWithADoubleAndUnit> fMapZMax;
};

} // namespace Messenger

} // namespace MACE::SimDose
