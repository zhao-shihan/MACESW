// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Mustard/Detector/Description/DescriptionWithCacheBase.h++"

#include "muc/array"

#include <utility>
#include <vector>

namespace MACE::PhaseI::Detector::Description {

class TTC final : public Mustard::Detector::Description::DescriptionWithCacheBase<TTC> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    TTC();
    ~TTC() override = default;

public:
    // Geometry

    auto Length() const -> auto { return *fLength; }
    auto Width() const -> auto { return *fWidth; }
    auto Thickness() const -> auto { return *fThickness; }
    auto Radius() const -> const auto& { return *fRadius; }
    auto SlantAngle() const -> auto { return *fSlantAngle; }
    auto NAlongPhi() const -> const auto& { return *fNAlongPhi; }
    auto ZPosition() const -> auto { return *fZPosition; }
    auto BarrelLength() const -> auto { return *fBarrelLength; }
    auto PCBLength() const -> auto { return *fPCBLength; }
    auto PCBWidth() const -> auto { return *fPCBWidth; }
    auto PCBThickness() const -> auto { return *fPCBThickness; }
    auto WindowLength() const -> auto { return *fWindowLength; }
    auto WindowWidth() const -> auto { return *fWindowWidth; }
    auto WindowThickness() const -> auto { return *fWindowThickness; }
    auto LightCouplerLength() const -> auto { return *fLightCouplerLength; }
    auto LightCouplerWidth() const -> auto { return *fLightCouplerWidth; }
    auto LightCouplerThickness() const -> auto { return *fLightCouplerThickness; }
    auto SiliconeLength() const -> auto { return *fSiliconeLength; }
    auto SiliconeWidth() const -> auto { return *fSiliconeWidth; }
    auto SiliconeThickness() const -> auto { return *fSiliconeThickness; }
    auto NSiPM() const -> auto { return *fNSiPM; }

    auto Length(double val) -> void { fLength = val; }
    auto Width(double val) -> void { fWidth = val; }
    auto Thickness(double val) -> void { fThickness = val; }
    auto Radius(std::vector<double> val) -> void { fRadius = std::move(val); }
    auto SlantAngle(double val) -> void { fSlantAngle = val; }
    auto NAlongPhi(std::vector<int> val) -> void { fNAlongPhi = std::move(val); }
    auto ZPosition(std::vector<double> val) -> void { fZPosition = std::move(val); }
    auto BarrelLength(int val) -> void { fBarrelLength = val; }
    auto PCBLength(double val) -> void { fPCBLength = val; }
    auto PCBWidth(double val) -> void { fPCBWidth = val; }
    auto PCBThickness(double val) -> void { fPCBThickness = val; }
    auto WindowLength(double val) -> void { fWindowLength = val; }
    auto WindowWidth(double val) -> void { fWindowWidth = val; }
    auto WindowThickness(double val) -> void { fWindowThickness = val; }
    auto LightCouplerLength(double val) -> void { fLightCouplerLength = val; }
    auto LightCouplerWidth(double val) -> void { fLightCouplerWidth = val; }
    auto LightCouplerThickness(double val) -> void { fLightCouplerThickness = val; }
    auto SiliconeLength(double val) -> void { fSiliconeLength = val; }
    auto SiliconeWidth(double val) -> void { fSiliconeWidth = val; }
    auto SiliconeThickness(double val) -> void { fSiliconeThickness = val; }
    auto NSiPM(int val) -> void { fNSiPM = val; }

    // Material

    auto Density() const -> auto { return *fDensity; }
    auto RIndexEnergyBin() const -> const auto& { return *fRIndexEnergyBin; }
    auto RIndex() const -> const auto& { return *fRIndex; }
    auto AbsLengthEnergyBin() const -> const auto& { return *fAbsLengthEnergyBin; }
    auto AbsLength() const -> const auto& { return *fAbsLength; }
    auto ScintillationComponent1EnergyBin() const -> const auto& { return *fScintillationComponent1EnergyBin; }
    auto ScintillationComponent1() const -> const auto& { return *fScintillationComponent1; }
    auto ScintillationYield() const -> auto { return *fScintillationYield; }
    auto ScintillationRiseTimeConstant1() const -> auto { return *fScintillationRiseTimeConstant1; }
    auto ScintillationDecayTimeConstant1() const -> auto { return *fScintillationDecayTimeConstant1; }
    auto ResolutionScale() const -> auto { return *fResolutionScale; }
    auto LightCouplerDensity() const -> auto { return *fLightCouplerDensity; }
    auto LightCouplerCarbonElement() const -> auto { return *fLightCouplerCarbonElement; }
    auto LightCouplerHydrogenElement() const -> auto { return *fLightCouplerHydrogenElement; }
    auto LightCouplerOxygenElement() const -> auto { return *fLightCouplerOxygenElement; }
    auto LightCouplerSiliconElement() const -> auto { return *fLightCouplerSiliconElement; }
    auto LightCouplerRIndex() const -> const auto& { return *fLightCouplerRIndex; }
    auto LightCouplerAbsLength() const -> const auto& { return *fLightCouplerAbsLength; }
    auto WindowDensity() const -> auto { return *fWindowDensity; }
    auto WindowCarbonElement() const -> auto { return *fWindowCarbonElement; }
    auto WindowHydrogenElement() const -> auto { return *fWindowHydrogenElement; }
    auto WindowOxygenElement() const -> auto { return *fWindowOxygenElement; }
    auto WindowRIndex() const -> const auto& { return *fWindowRIndex; }
    auto SiPMEnergyBin() const -> const auto& { return *fSiPMEnergyBin; }
    auto SiPMEfficiency() const -> const auto& { return *fSiPMEfficiency; }
    auto ReflectorReflectivity() const -> const auto& { return *fReflectorReflectivity; }
    auto CouplerTransmittance() const -> const auto& { return *fCouplerTransmittance; }
    auto AirPaintReflectivity() const -> const auto& { return *fAirPaintReflectivity; }
    auto CathodeSurface() const -> const auto& { return *fCathodeSurface; }

    auto Density(double val) -> void { fDensity = val; }
    auto RIndexEnergyBin(std::vector<double> val) -> void { fRIndexEnergyBin = std::move(val); }
    auto RIndex(std::vector<double> val) -> void { fRIndex = std::move(val); }
    auto AbsLengthEnergyBin(std::vector<double> val) -> void { fAbsLengthEnergyBin = std::move(val); }
    auto AbsLength(std::vector<double> val) -> void { fAbsLength = std::move(val); }
    auto ScintillationComponent1EnergyBin(std::vector<double> val) -> void { fScintillationComponent1EnergyBin = std::move(val); }
    auto ScintillationComponent1(std::vector<double> val) -> void { fScintillationComponent1 = std::move(val); }
    auto ScintillationYield(double val) -> void { fScintillationYield = val; }
    auto ScintillationRiseTimeConstant1(double val) -> void { fScintillationRiseTimeConstant1 = val; }
    auto ScintillationDecayTimeConstant1(double val) -> void { fScintillationDecayTimeConstant1 = val; }
    auto ResolutionScale(double val) -> void { fResolutionScale = val; }
    auto LightCouplerDensity(double val) -> void { fLightCouplerDensity = val; }
    auto LightCouplerCarbonElement(int val) -> void { fLightCouplerCarbonElement = val; }
    auto LightCouplerHydrogenElement(int val) -> void { fLightCouplerHydrogenElement = val; }
    auto LightCouplerOxygenElement(int val) -> void { fLightCouplerOxygenElement = val; }
    auto LightCouplerSiliconElement(int val) -> void { fLightCouplerSiliconElement = val; }
    auto LightCouplerRIndex(std::vector<double> val) -> void { fLightCouplerRIndex = std::move(val); }
    auto LightCouplerAbsLength(std::vector<double> val) -> void { fLightCouplerAbsLength = std::move(val); }
    auto WindowDensity(double val) -> void { fWindowDensity = val; }
    auto WindowCarbonElement(double val) -> void { fWindowCarbonElement = val; }
    auto WindowHydrogenElement(double val) -> void { fWindowHydrogenElement = val; }
    auto WindowOxygenElement(double val) -> void { fWindowOxygenElement = val; }
    auto WindowRIndex(std::vector<double> val) -> void { fWindowRIndex = std::move(val); }
    auto SiPMEnergyBin(std::vector<double> val) -> void { fSiPMEnergyBin = std::move(val); }
    auto SiPMEfficiency(std::vector<double> val) -> void { fSiPMEfficiency = std::move(val); }
    auto ReflectorReflectivity(std::vector<double> val) -> void { fReflectorReflectivity = std::move(val); }
    auto CouplerTransmittance(std::vector<double> val) -> void { fCouplerTransmittance = std::move(val); }
    auto AirPaintReflectivity(std::vector<double> val) -> void { fAirPaintReflectivity = std::move(val); }
    auto CathodeSurface(std::vector<double> val) -> void { fCathodeSurface = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    // Geometry

    Simple<double> fLength;
    Simple<double> fWidth;
    Simple<double> fThickness;
    Simple<std::vector<double>> fRadius;
    Simple<double> fSlantAngle;
    Simple<std::vector<int>> fNAlongPhi;
    Simple<std::vector<double>> fZPosition;
    Simple<double> fBarrelLength;
    Simple<double> fPCBLength;
    Simple<double> fPCBWidth;
    Simple<double> fPCBThickness;
    Simple<double> fWindowLength;
    Simple<double> fWindowWidth;
    Simple<double> fWindowThickness;
    Simple<double> fLightCouplerLength;
    Simple<double> fLightCouplerWidth;
    Simple<double> fLightCouplerThickness;
    Simple<double> fSiliconeLength;
    Simple<double> fSiliconeWidth;
    Simple<double> fSiliconeThickness;
    Simple<int> fNSiPM;

    // Material

    Simple<double> fDensity;
    Simple<std::vector<double>> fRIndexEnergyBin;
    Simple<std::vector<double>> fRIndex;
    Simple<std::vector<double>> fAbsLengthEnergyBin;
    Simple<std::vector<double>> fAbsLength;
    Simple<std::vector<double>> fScintillationComponent1EnergyBin;
    Simple<std::vector<double>> fScintillationComponent1;
    Simple<double> fScintillationYield;
    Simple<double> fScintillationRiseTimeConstant1;
    Simple<double> fScintillationDecayTimeConstant1;
    Simple<double> fResolutionScale;
    Simple<double> fLightCouplerDensity;
    Simple<int> fLightCouplerCarbonElement;
    Simple<int> fLightCouplerHydrogenElement;
    Simple<int> fLightCouplerOxygenElement;
    Simple<int> fLightCouplerSiliconElement;
    Simple<std::vector<double>> fLightCouplerRIndex;
    Simple<std::vector<double>> fLightCouplerAbsLength;
    Simple<double> fWindowDensity;
    Simple<double> fWindowCarbonElement;
    Simple<double> fWindowHydrogenElement;
    Simple<double> fWindowOxygenElement;
    Simple<std::vector<double>> fWindowRIndex;
    Simple<std::vector<double>> fSiPMEnergyBin;
    Simple<std::vector<double>> fSiPMEfficiency;
    Simple<std::vector<double>> fReflectorReflectivity;
    Simple<std::vector<double>> fCouplerTransmittance;
    Simple<std::vector<double>> fAirPaintReflectivity;
    Simple<std::vector<double>> fCathodeSurface;
};

} // namespace MACE::PhaseI::Detector::Description
