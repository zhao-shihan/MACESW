#include "MACE/Detector/Definition/CDCCell.h++"
#include "MACE/Detector/Description/CDC.h++"

#include "Mustard/Utility/LiteralUnit.h++"

#include "CLHEP/Vector/RotationZ.h"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4TwistedTubs.hh"
#include "G4TwoVector.hh"

#include "muc/math"
#include "muc/numeric"

#include "fmt/format.h"

#include <cmath>
#include <memory>
#include <utility>

namespace MACE::Detector::Definition {

auto CDCCell::Construct(G4bool checkOverlaps) -> void {
    using namespace Mustard::LiteralUnit::MathConstantSuffix;

    const auto& cdc{Description::CDC::Instance()};
    const auto cellName{cdc.Name() + "Cell"};
    const auto svName{cdc.Name() + "SensitiveVolume"};
    const auto fwName{cdc.Name() + "FieldWire"};
    const auto swName{cdc.Name() + "SenseWire"};

    const auto nist{G4NistManager::Instance()};

    for (auto&& super : std::as_const(cdc.LayerConfiguration())) {
        for (auto&& sense : super.sense) {
            //
            // cell volume
            //

            const auto cellRIn{sense.innerRadius};
            const auto cellROut{&sense != &super.sense.back() ?
                                    sense.outerRadius :
                                    sense.outerRadius + cdc.FieldWireDiameter()};
            const auto solidCell{[&] {
                if (super.isAxial) {
                    return static_cast<G4VSolid*>(Make<G4Tubs>(
                        cellName,
                        cellRIn,
                        cellROut,
                        sense.halfLength,
                        -super.cellAzimuthWidth / 2,
                        super.cellAzimuthWidth));
                }
                return static_cast<G4VSolid*>(Make<G4TwistedTubs>(
                    cellName,
                    sense.stereoAzimuthAngle,
                    cellRIn / std::cos(sense.stereoAzimuthAngle / 2),
                    cellROut / std::cos(sense.stereoAzimuthAngle / 2),
                    sense.halfLength,
                    super.cellAzimuthWidth));
            }()};
            const auto logicalCell{Make<G4LogicalVolume>(
                solidCell,
                cdc.GasMaterial(),
                cellName)};
            const auto rFW{cdc.FieldWireDiameter() / 2};
            const auto phiFWFront{std::asin((rFW / std::cos(sense.stereoAzimuthAngle / 2)) / (cellRIn + rFW))};
            for (auto&& cell : sense.cell) {
                // G4PVReplica seems buggy here
                Make<G4PVPlacement>(
                    G4Transform3D{CLHEP::HepRotationZ{cell.centerAzimuth - phiFWFront}, {}},
                    logicalCell,
                    fmt::format("{}_{}", cellName, cell.cellID),
                    Mother().LogicalVolume(sense.senseLayerID),
                    false,
                    cell.cellID,
                    checkOverlaps);
            }

            //
            // sensitive volume
            //

            const auto svRIn{sense.innerRadius + cdc.FieldWireDiameter()};
            const auto svROut{sense.outerRadius};
            const auto phiFWBack{std::asin(rFW / (cellRIn + rFW))};
            const auto phiSV{super.cellAzimuthWidth - phiFWBack - phiFWFront};
            const auto solidSV{[&] {
                if (super.isAxial) {
                    return static_cast<G4VSolid*>(Make<G4Tubs>(
                        svName,
                        svRIn,
                        svROut,
                        sense.halfLength,
                        -phiSV / 2,
                        phiSV));
                }
                return static_cast<G4VSolid*>(Make<G4TwistedTubs>(
                    svName,
                    sense.stereoAzimuthAngle,
                    svRIn / std::cos(sense.stereoAzimuthAngle / 2),
                    svROut / std::cos(sense.stereoAzimuthAngle / 2),
                    sense.halfLength,
                    phiSV));
            }()};
            const auto logicalSV{Make<G4LogicalVolume>(
                solidSV,
                cdc.GasMaterial(),
                svName)};
            Make<G4PVPlacement>(
                G4Transform3D{CLHEP::HepRotationZ{(phiFWBack - phiFWFront) / 2 + phiFWFront}, {}},
                logicalSV,
                svName,
                logicalCell,
                false,
                sense.senseLayerID,
                checkOverlaps);

            //
            // field wire volume
            //

            const auto solidFW{Make<G4Tubs>(
                fwName,
                0,
                rFW,
                sense.halfLength * sense.SecStereoZenithAngle(cellRIn) -
                    cdc.FieldWireDiameter(), // prevent protrusion
                0,
                2_pi)};
            const auto logicalFW{Make<G4LogicalVolume>(
                solidFW,
                nist->FindOrBuildMaterial("G4_Al"),
                fwName)};
            const auto placeFW{
                [&](int copyNo, double r, double phi0) {
                    return Make<G4PVPlacement>( // clang-format off
                        G4Transform3D{CLHEP::HepRotationZ{phi0 + phiFWFront}, {}} * // clang-format on
                            G4Transform3D{CLHEP::HepRotationX{-sense.StereoZenithAngle(r)}, {r, 0, 0}},
                        logicalFW,
                        fwName,
                        logicalCell,
                        false,
                        copyNo,
                        checkOverlaps);
                }};
            const auto rInnerWire{sense.innerRadius + rFW};
            const auto rOuterWire{sense.outerRadius + rFW};
            const auto rCenterWire{muc::midpoint(rInnerWire, rOuterWire)};
            placeFW(0, rInnerWire, -super.cellAzimuthWidth / 2);
            placeFW(1, rInnerWire, 0);
            placeFW(2, rCenterWire, -super.cellAzimuthWidth / 2);
            if (&sense == &super.sense.back()) {
                placeFW(3, rOuterWire, -super.cellAzimuthWidth / 2);
                placeFW(4, rOuterWire, 0);
            }

            //
            // sense wire volume
            //

            const auto solidSW{Make<G4Tubs>(
                swName,
                0,
                cdc.SenseWireDiameter() / 2,
                sense.halfLength * sense.SecStereoZenithAngle(rCenterWire) -
                    cdc.SenseWireDiameter(), // prevent protrusion
                0,
                2_pi)};
            const auto logicalSW{Make<G4LogicalVolume>(
                solidSW,
                nist->FindOrBuildMaterial("G4_W"),
                swName)};
            Make<G4PVPlacement>( // clang-format off
                G4Transform3D{CLHEP::HepRotationX{-sense.StereoZenithAngle(rCenterWire)}, {rCenterWire, 0, 0}}, // clang-format on
                logicalSW,
                swName,
                logicalSV,
                false,
                sense.senseLayerID,
                checkOverlaps);
        }
    }
}

} // namespace MACE::Detector::Definition
