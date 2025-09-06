#include "MACE/PhaseI/Detector/Definition/MRPC.h++"
#include "MACE/PhaseI/Detector/Description/MRPC.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"

namespace MACE::PhaseI::Detector::Definition {

using namespace Mustard::LiteralUnit;

auto MRPC::Construct(G4bool checkOverlaps) -> void {
    const auto& mrpc{Description::MRPC::Instance()};

    const auto honeyCombThickness{mrpc.HoneyCombThickness()};
    const auto pcbThickness{mrpc.PCBThickness()};
    const auto mylarThickness{mrpc.MylarThickness()};
    const auto anodeThickness{mrpc.AnodeThickness()};
    const auto outerGlassThickness{mrpc.OuterGlassThickness()};
    const auto innerGlassThickness{mrpc.InnerGlassThickness()};
    const auto gasGapThickness{mrpc.GasGapThickness()};
    const auto gasTankThickness{mrpc.GasTankThickness()};
    auto centralRadius{mrpc.CentralRadius()};
    const auto centralMRPCLength{mrpc.CentralMRPCLength()};
    const auto centralMRPCWidth{mrpc.CentralMRPCWidth()};
    auto cornerRadius{mrpc.CornerRadius()};
    const auto cornerMRPCLength{mrpc.CornerMRPCLength()};
    const auto cornerMRPCWidth{mrpc.CornerMRPCWidth()};

    const auto mrpcTotalThickness{
        2 * (honeyCombThickness + pcbThickness + mylarThickness + anodeThickness + outerGlassThickness) +
        mrpc.NGaps() * gasGapThickness +
        (mrpc.NGaps() - 1) * innerGlassThickness};

    const auto nist{G4NistManager::Instance()};

    const auto aluminum{nist->FindOrBuildMaterial("G4_Al")};
    const auto glass{nist->FindOrBuildMaterial("G4_GLASS_PLATE")};
    const auto graphite{nist->FindOrBuildMaterial("G4_GRAPHITE")};
    const auto pcb{nist->FindOrBuildMaterial("G4_PLEXIGLASS")};
    const auto mylar{nist->FindOrBuildMaterial("G4_MYLAR")};

    const auto hydrogenElement{nist->FindOrBuildElement("H")};
    const auto carbonElement{nist->FindOrBuildElement("C")};
    const auto fluorineElement{nist->FindOrBuildElement("F")};
    const auto sulfurElement{nist->FindOrBuildElement("S")};

    const auto c2h2f4{new G4Material("C2H2F4", 4.16_kg_m3, 3, kStateGas)};
    c2h2f4->AddElement(hydrogenElement, 2);
    c2h2f4->AddElement(carbonElement, 2);
    c2h2f4->AddElement(fluorineElement, 4);

    const auto c4h10{new G4Material("C4H10", 2.37_kg_m3, 2, kStateGas)};
    c4h10->AddElement(carbonElement, 4);
    c4h10->AddElement(hydrogenElement, 10);

    const auto sf6{new G4Material("SF6", 5.96_kg_m3, 2, kStateGas)};
    sf6->AddElement(sulfurElement, 1);
    sf6->AddElement(fluorineElement, 6);

    const auto gas{new G4Material("mrpc_gas", 4.16_kg_m3, 3, kStateGas)};
    gas->AddMaterial(c2h2f4, 0.9);
    gas->AddMaterial(c4h10, 0.05);
    gas->AddMaterial(sf6, 0.05);

    const auto solidCentralGasTank{Make<G4Box>(mrpc.Name() + "GasTank",
                                               (centralMRPCWidth + 1_cm) / 2,
                                               (mrpcTotalThickness + 2_cm) / 2,
                                               (centralMRPCLength + 1_cm) / 2)};
    const auto logicCentralGasTank{Make<G4LogicalVolume>(solidCentralGasTank, aluminum, mrpc.Name() + "GasTank")};

    const auto solidCornerGasTank{Make<G4Box>(mrpc.Name() + "GasTank",
                                              (cornerMRPCWidth + 1_cm) / 2,
                                              (mrpcTotalThickness + 2_cm) / 2,
                                              (cornerMRPCLength + 1_cm) / 2)};
    const auto logicCornerGasTank{Make<G4LogicalVolume>(solidCornerGasTank, aluminum, mrpc.Name() + "GasTank")};

    auto angle{2_pi / mrpc.NMRPCs()};
    centralRadius += (mrpcTotalThickness + 2_cm) / 2;
    cornerRadius += (mrpcTotalThickness + 2_cm) / 2;

    for (int i{}; i < mrpc.NMRPCs(); i++) {
        const auto radius{(i % 2 == 0) ? centralRadius : cornerRadius};
        const auto logicTank{(i % 2 == 0) ? logicCentralGasTank : logicCornerGasTank};

        Make<G4PVPlacement>(G4Translate3D(radius * std::sin(i * angle),
                                          radius * std::cos(i * angle),
                                          0) *
                                G4RotateZ3D(-i * angle),
                            logicTank,
                            mrpc.Name() + "GasTank",
                            Mother().LogicalVolume(),
                            false,
                            i,
                            checkOverlaps);
    }

    const auto solidCentralGas{Make<G4Box>(mrpc.Name() + "Gas",
                                           (centralMRPCWidth + 1_cm - gasTankThickness) / 2,
                                           (mrpcTotalThickness + 2_cm - gasTankThickness) / 2,
                                           (centralMRPCLength + 1_cm - gasTankThickness) / 2)};
    const auto logicCentralGas{Make<G4LogicalVolume>(solidCentralGas, gas, mrpc.Name() + "Gas")};
    Make<G4PVPlacement>(G4Transform3D::Identity,
                        logicCentralGas,
                        mrpc.Name() + "Gas",
                        logicCentralGasTank,
                        false,
                        checkOverlaps);

    const auto solidCornerGas{Make<G4Box>(mrpc.Name() + "Gas",
                                          (cornerMRPCWidth + 1_cm - gasTankThickness) / 2,
                                          (mrpcTotalThickness + 2_cm - gasTankThickness) / 2,
                                          (cornerMRPCLength + 1_cm - gasTankThickness) / 2)};
    const auto logicCornerGas{Make<G4LogicalVolume>(solidCornerGas, gas, mrpc.Name() + "Gas")};
    Make<G4PVPlacement>(G4Transform3D::Identity,
                        logicCornerGas,
                        mrpc.Name() + "Gas",
                        logicCornerGasTank,
                        false,
                        checkOverlaps);

    const auto solidCentralInnerGlass{Make<G4Box>(mrpc.Name() + "InnerGlass",
                                                  centralMRPCWidth / 2,
                                                  innerGlassThickness / 2,
                                                  centralMRPCLength / 2)};
    const auto logicCentralInnerGlass{Make<G4LogicalVolume>(solidCentralInnerGlass, glass, mrpc.Name() + "InnerGlass")};
    Make<G4PVPlacement>(G4Transform3D::Identity,
                        logicCentralInnerGlass,
                        mrpc.Name() + "InnerGlass",
                        logicCentralGas,
                        false,
                        checkOverlaps);

    const auto solidCornerInnerGlass{Make<G4Box>(mrpc.Name() + "InnerGlass",
                                                 cornerMRPCWidth / 2,
                                                 innerGlassThickness / 2,
                                                 cornerMRPCLength / 2)};
    const auto logicCornerInnerGlass{Make<G4LogicalVolume>(solidCornerInnerGlass, glass, mrpc.Name() + "InnerGlass")};
    Make<G4PVPlacement>(G4Transform3D::Identity,
                        logicCornerInnerGlass,
                        mrpc.Name() + "InnerGlass",
                        logicCornerGas,
                        false,
                        checkOverlaps);

    for (int i{1}; i < 0.5 * mrpc.NGaps(); i++) {
        // Central InnerGlass
        Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY, G4ThreeVector(0, i * (gasGapThickness + innerGlassThickness), 0)),
                            logicCentralInnerGlass,
                            mrpc.Name() + "InnerGlass",
                            logicCentralGas,
                            false,
                            checkOverlaps);
        Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY, -G4ThreeVector(0, i * (gasGapThickness + innerGlassThickness), 0)),
                            logicCentralInnerGlass,
                            mrpc.Name() + "InnerGlass",
                            logicCentralGas,
                            false,
                            checkOverlaps);

        // Corner InnerGlass
        Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY, G4ThreeVector(0, i * (gasGapThickness + innerGlassThickness), 0)),
                            logicCornerInnerGlass,
                            mrpc.Name() + "InnerGlass",
                            logicCornerGas,
                            false,
                            checkOverlaps);
        Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY, -G4ThreeVector(0, i * (gasGapThickness + innerGlassThickness), 0)),
                            logicCornerInnerGlass,
                            mrpc.Name() + "InnerGlass",
                            logicCornerGas,
                            false,
                            checkOverlaps);
    }
    // Central OuterGlass
    const auto solidCentralOuterGlass{Make<G4Box>(mrpc.Name() + "OuterGlass",
                                                  centralMRPCWidth / 2,
                                                  outerGlassThickness / 2,
                                                  centralMRPCLength / 2)};
    const auto logicCentralOuterGlass{Make<G4LogicalVolume>(solidCentralOuterGlass, glass, mrpc.Name() + "OuterGlass")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + outerGlassThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness),
                                                    0)),
                        logicCentralOuterGlass,
                        mrpc.Name() + "OuterGlass",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + outerGlassThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness),
                                                     0)),
                        logicCentralOuterGlass,
                        mrpc.Name() + "OuterGlass",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    // Corner OuterGlass
    const auto solidCornerOuterGlass{Make<G4Box>(mrpc.Name() + "OuterGlass",
                                                 cornerMRPCWidth / 2,
                                                 outerGlassThickness / 2,
                                                 cornerMRPCLength / 2)};
    const auto logicCornerOuterGlass{Make<G4LogicalVolume>(solidCornerOuterGlass, glass, mrpc.Name() + "OuterGlass")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + outerGlassThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness),
                                                    0)),
                        logicCornerOuterGlass,
                        mrpc.Name() + "OuterGlass",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + outerGlassThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness),
                                                     0)),
                        logicCornerOuterGlass,
                        mrpc.Name() + "OuterGlass",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    // Central Anode
    const auto solidCentralAnode{Make<G4Box>(mrpc.Name() + "Anode",
                                             centralMRPCWidth / 2,
                                             anodeThickness / 2,
                                             centralMRPCLength / 2)};
    const auto logicCentralAnode{Make<G4LogicalVolume>(solidCentralAnode, graphite, mrpc.Name() + "Anode")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + anodeThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness,
                                                    0)),
                        logicCentralAnode,
                        mrpc.Name() + "Anode",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + anodeThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness,
                                                     0)),
                        logicCentralAnode,
                        mrpc.Name() + "Anode",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    // Corner Anode
    const auto solidCornerAnode{Make<G4Box>(mrpc.Name() + "Anode",
                                            cornerMRPCWidth / 2,
                                            anodeThickness / 2,
                                            cornerMRPCLength / 2)};
    const auto logicCornerAnode{Make<G4LogicalVolume>(solidCornerAnode, graphite, mrpc.Name() + "Anode")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + anodeThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness,
                                                    0)),
                        logicCornerAnode,
                        mrpc.Name() + "Anode",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + anodeThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness,
                                                     0)),
                        logicCornerAnode,
                        mrpc.Name() + "Anode",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    // Central Mylar
    const auto solidCentralMylar{Make<G4Box>(mrpc.Name() + "Mylar",
                                             centralMRPCWidth / 2,
                                             mylarThickness / 2,
                                             centralMRPCLength / 2)};
    const auto logicCentralMylar{Make<G4LogicalVolume>(solidCentralMylar, mylar, mrpc.Name() + "Mylar")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + mylarThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness,
                                                    0)),
                        logicCentralMylar,
                        mrpc.Name() + "Mylar",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + mylarThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness,
                                                     0)),
                        logicCentralMylar,
                        mrpc.Name() + "Mylar",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    // Corner Mylar
    const auto solidCornerMylar{Make<G4Box>(mrpc.Name() + "Mylar",
                                            cornerMRPCWidth / 2,
                                            mylarThickness / 2,
                                            cornerMRPCLength / 2)};
    const auto logicCornerMylar{Make<G4LogicalVolume>(solidCornerMylar, mylar, mrpc.Name() + "Mylar")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + mylarThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness,
                                                    0)),
                        logicCornerMylar,
                        mrpc.Name() + "Mylar",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + mylarThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness,
                                                     0)),
                        logicCornerMylar,
                        mrpc.Name() + "Mylar",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    // Central PCB
    const auto solidCentralPCB{Make<G4Box>(mrpc.Name() + "PCB",
                                           centralMRPCWidth / 2,
                                           pcbThickness / 2,
                                           centralMRPCLength / 2)};
    const auto logicCentralPCB{Make<G4LogicalVolume>(solidCentralPCB, pcb, mrpc.Name() + "PCB")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + pcbThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness,
                                                    0)),
                        logicCentralPCB,
                        mrpc.Name() + "PCB",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + pcbThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness,
                                                     0)),
                        logicCentralPCB,
                        mrpc.Name() + "PCB",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    // Corner PCB
    const auto solidCornerPCB{Make<G4Box>(mrpc.Name() + "PCB",
                                          cornerMRPCWidth / 2,
                                          pcbThickness / 2,
                                          cornerMRPCLength / 2)};
    const auto logicCornerPCB{Make<G4LogicalVolume>(solidCornerPCB, pcb, mrpc.Name() + "PCB")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + pcbThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness,
                                                    0)),
                        logicCornerPCB,
                        mrpc.Name() + "PCB",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + pcbThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness,
                                                     0)),
                        logicCornerPCB,
                        mrpc.Name() + "PCB",
                        logicCornerGas,
                        false,
                        checkOverlaps);

    // Central HoneyComb
    const auto solidCentralHoneyComb{Make<G4Box>(mrpc.Name() + "HoneyComb",
                                                 centralMRPCWidth / 2,
                                                 honeyCombThickness / 2,
                                                 centralMRPCLength / 2)};
    const auto logicCentralHoneyComb{Make<G4LogicalVolume>(solidCentralHoneyComb, mylar, mrpc.Name() + "HoneyComb")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + honeyCombThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness + pcbThickness,
                                                    0)),
                        logicCentralHoneyComb,
                        mrpc.Name() + "HoneyComb",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + honeyCombThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness + pcbThickness,
                                                     0)),
                        logicCentralHoneyComb,
                        mrpc.Name() + "HoneyComb",
                        logicCentralGas,
                        false,
                        checkOverlaps);
    // Corner HoneyComb
    const auto solidCornerHoneyComb{Make<G4Box>(mrpc.Name() + "HoneyComb",
                                                cornerMRPCWidth / 2,
                                                honeyCombThickness / 2,
                                                cornerMRPCLength / 2)};
    const auto logicCornerHoneyComb{Make<G4LogicalVolume>(solidCornerHoneyComb, mylar, mrpc.Name() + "HoneyComb")};
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      G4ThreeVector(0,
                                                    0.5 * (innerGlassThickness + honeyCombThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness + pcbThickness,
                                                    0)),
                        logicCornerHoneyComb,
                        mrpc.Name() + "HoneyComb",
                        logicCornerGas,
                        false,
                        checkOverlaps);
    Make<G4PVPlacement>(G4Transform3D(G4RotationMatrix::IDENTITY,
                                      -G4ThreeVector(0,
                                                     0.5 * (innerGlassThickness + honeyCombThickness + mrpc.NGaps() * gasGapThickness + (mrpc.NGaps() - 2) * innerGlassThickness) + outerGlassThickness + anodeThickness + mylarThickness + pcbThickness,
                                                     0)),
                        logicCornerHoneyComb,
                        mrpc.Name() + "HoneyComb",
                        logicCornerGas,
                        false,
                        checkOverlaps);
}
} // namespace MACE::PhaseI::Detector::Definition
