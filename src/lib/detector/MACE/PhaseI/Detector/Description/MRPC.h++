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

#include "Mustard/Detector/Description/DescriptionBase.h++"

namespace MACE::PhaseI::Detector::Description {

class MRPC final : public Mustard::Detector::Description::DescriptionBase<MRPC> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    MRPC();
    ~MRPC() override = default;

public:
    auto HoneyCombThickness() const -> auto { return fHoneyCombThickness; }
    auto PCBThickness() const -> auto { return fPCBThickness; }
    auto MylarThickness() const -> auto { return fMylarThickness; }
    auto AnodeThickness() const -> auto { return fAnodeThickness; }
    auto OuterGlassThickness() const -> auto { return fOuterGlassThickness; }
    auto InnerGlassThickness() const -> auto { return fInnerGlassThickness; }
    auto NGaps() const -> auto { return fNGaps; }
    auto GasGapThickness() const -> auto { return fGasGapThickness; }
    auto GasTankThickness() const -> auto { return fGasTankThickness; }
    auto NMRPCs() const -> auto { return fNMRPCs; }
    auto CentralRadius() const -> auto { return fCentralRadius; }
    auto CentralMRPCLength() const -> auto { return fCentralMRPCLength; }
    auto CentralMRPCWidth() const -> auto { return fCentralMRPCWidth; }
    auto CornerRadius() const -> auto { return fCornerRadius; }
    auto CornerMRPCLength() const -> auto { return fCornerMRPCLength; }
    auto CornerMRPCWidth() const -> auto { return fCornerMRPCWidth; }
    auto TimeResolutionFWHM() const -> auto { return fTimeResolutionFWHM; }
    auto SpacialResolutionFWHM() const -> auto { return fSpacialResolutionFWHM; }

    auto HoneyCombThickness(double val) -> void { fHoneyCombThickness = val; }
    auto PCBThickness(double val) -> void { fPCBThickness = val; }
    auto MylarThickness(double val) -> void { fMylarThickness = val; }
    auto AnodeThickness(double val) -> void { fAnodeThickness = val; }
    auto OuterGlassThickness(double val) -> void { fOuterGlassThickness = val; }
    auto InnerGlassThickness(double val) -> void { fInnerGlassThickness = val; }
    auto NGaps(int val) -> void { fNGaps = val; }
    auto GasGapThickness(double val) -> void { fGasGapThickness = val; }
    auto GasTankThickness(double val) -> void { fGasTankThickness = val; }
    auto NMRPCs(int val) -> void { fNMRPCs = val; }
    auto CentralRadius(double val) -> void { fCentralRadius = val; }
    auto CentralMRPCLength(double val) -> void { fCentralMRPCLength = val; }
    auto CentralMRPCWidth(double val) -> void { fCentralMRPCWidth = val; }
    auto CornerRadius(double val) -> void { fCornerRadius = val; }
    auto CornerMRPCLength(double val) -> void { fCornerMRPCLength = val; }
    auto CornerMRPCWidth(double val) -> void { fCornerMRPCWidth = val; }
    auto TimeResolutionFWHM(double val) -> void { fTimeResolutionFWHM = val; }
    auto SpacialResolutionFWHM(double val) -> void { fSpacialResolutionFWHM = val; }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    double fHoneyCombThickness;
    double fPCBThickness;
    double fMylarThickness;
    double fAnodeThickness;
    double fOuterGlassThickness;
    double fInnerGlassThickness;
    int fNGaps;
    double fGasGapThickness;
    double fGasTankThickness;
    int fNMRPCs;
    double fCentralRadius;
    double fCentralMRPCLength;
    double fCentralMRPCWidth;
    double fCornerRadius;
    double fCornerMRPCLength;
    double fCornerMRPCWidth;
    double fTimeResolutionFWHM;
    double fSpacialResolutionFWHM;
};

} // namespace MACE::PhaseI::Detector::Description
