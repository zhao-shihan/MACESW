#include "MACE/Detector/Description/PDSVeto.h++"

#include "Mustard/Utility/LiteralUnit.h++"

namespace MACE::Detector::Description {

using namespace Mustard::LiteralUnit::Length;

PDSVeto::PDSVeto() :
    DescriptionBase{"PDSVeto"},
    // Geometry
    fTopPSLength{145_cm},
    fSidePSLength{140_cm},
    fCap1PSLength{180_cm},
    fCap2PSLength{35_cm},

    fPSWidth{10_cm},
    fPSThickness{2_cm},
    fPSFiberRadius{0.7_mm},
    fPSHoleRadius{0.75_mm},
    fPSFiberCurvatureRadius{13_cm},

    fInterPSGap{0.3_cm},
    fInterModuleGap{0.5_cm},
    fCap12Gap{1_cm},
    fModuleOffset{1_cm},
    fAlAbsorberThickness{0.6_cm},

    fTopLayer{3},
    fSideLayer{2},
    fBottomLayer{2},
    fCapLayer{2},

    fSiPMThickness{0.2_mm},
    fSiPMCathodeThickness{0.1_mm},
    fSiPMCouplerThickness{0.1_mm},
    fSiPMSize{3_mm},
    // Material
    fPSMaterialName{"G4_PLASTIC_SC_VINYLTOLUENE"} {}

auto PDSVeto::ImportAllValue(const YAML::Node& node) -> void {
    // Geometry
    ImportValue(node, fTopPSLength, "TopPSLength");
    ImportValue(node, fBottomPSLength, "BottomPSLength");
    ImportValue(node, fSidePSLength, "SidePSLength");
    ImportValue(node, fCap1PSLength, "Cap1PSLength");
    ImportValue(node, fCap2PSLength, "Cap2PSLength");
    ImportValue(node, fPSWidth, "PSWidth");
    ImportValue(node, fPSThickness, "PSThickness");
    ImportValue(node, fPSFiberRadius, "PSFiberRadius");
    ImportValue(node, fPSHoleRadius, "PSHoleRadius");
    ImportValue(node, fPSFiberCurvatureRadius, "PSFiberCurvatureRadius");
    ImportValue(node, fInterPSGap, "InterPSGap");
    ImportValue(node, fInterModuleGap, "InterModuleGap");
    ImportValue(node, fCap12Gap, "Cap12Gap");

    ImportValue(node, fModuleOffset, "ModuleOffset");
    ImportValue(node, fAlAbsorberThickness, "AlAbsorberThickness");
    ImportValue(node, fTopLayer, "TopLayer");
    ImportValue(node, fSideLayer, "SideLayer");
    ImportValue(node, fBottomLayer, "BottomLayer");
    ImportValue(node, fCapLayer, "CapLayer");
    ImportValue(node, fPSMaterialName, "PSMaterialName");
    ImportValue(node, fSiPMThickness, "SiPMThickness");
    ImportValue(node, fSiPMCathodeThickness, "SiPMCathodeThickness");
    ImportValue(node, fSiPMCouplerThickness, "SiPMCouplerThickness");
    ImportValue(node, fSiPMSize, "SiPMSize");
}
auto PDSVeto::ExportAllValue(YAML::Node& node) const -> void {
    // Geometry
    ExportValue(node, fTopPSLength, "TopPSLength");
    ExportValue(node, fBottomPSLength, "BottomPSLength");
    ExportValue(node, fSidePSLength, "SidePSLength");
    ExportValue(node, fCap1PSLength, "Cap1PSLength");
    ExportValue(node, fCap2PSLength, "Cap2PSLength");
    ExportValue(node, fPSWidth, "PSWidth");
    ExportValue(node, fPSThickness, "PSThickness");
    ExportValue(node, fPSFiberRadius, "PSFiberRadius");
    ExportValue(node, fPSHoleRadius, "PSHoleRadius");
    ExportValue(node, fPSFiberCurvatureRadius, "PSFiberCurvatureRadius");
    ExportValue(node, fInterPSGap, "InterPSGap");
    ExportValue(node, fInterModuleGap, "InterModuleGap");

    ExportValue(node, fModuleOffset, "ModuleOffset");
    ExportValue(node, fAlAbsorberThickness, "AlAbsorberThickness");
    ExportValue(node, fTopLayer, "TopLayer");
    ExportValue(node, fSideLayer, "SideLayer");
    ExportValue(node, fBottomLayer, "BottomLayer");
    ExportValue(node, fCapLayer, "CapLayer");
    ExportValue(node, fPSMaterialName, "PSMaterialName");
    ExportValue(node, fSiPMThickness, "SiPMThickness");
    ExportValue(node, fSiPMCathodeThickness, "SiPMCathodeThickness");
    ExportValue(node, fSiPMCouplerThickness, "SiPMCouplerThickness");
    ExportValue(node, fSiPMSize, "SiPMSize");
}

} // namespace MACE::Detector::Description