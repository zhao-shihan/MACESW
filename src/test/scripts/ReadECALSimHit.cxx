#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"

#include <algorithm>
#include <any>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

const auto nBinsValueType{100};
auto DataTupleName{"ECALSimHit"};

auto ReadECALSimHit(auto moduleName, auto srcFileName, auto dstFileName) -> int {
    ROOT::RDataFrame df0{Form("G4Run0/%s", DataTupleName), srcFileName};
    const auto dstFile{new TFile(dstFileName, "UPDATE")};
    auto moduleDir{static_cast<TDirectory*>(dstFile->Get(moduleName))};
    if (!moduleDir) {
        std::clog << "DST File: Directory named " << moduleName << " is a new dir, create it." << std::endl;
        moduleDir = dstFile->mkdir(moduleName);
    }
    auto dstDir{moduleDir->mkdir(DataTupleName)};
    if (!dstDir) {
        std::cerr << "Make TDirectory " << DataTupleName << " Failed. ";
        if (moduleDir->Get(DataTupleName)) {
            std::cerr << "Already exist." << std::endl;
            return 1;
        }
    }
    std::clog << "Getting " << DataTupleName << " from module: " << moduleName << std::endl;
    dstFile->cd();
    dstDir->cd();

    auto df{df0.Define("x_0", "x[0]")
                .Define("x_1", "x[1]")
                .Define("x_2", "x[2]")
                .Define("p_0", "p[0]")
                .Define("p_1", "p[1]")
                .Define("p_2", "p[2]")
                .Define("p0_0", "p0[0]")
                .Define("p0_1", "p0[1]")
                .Define("p0_2", "p0[2]")};

    std::vector<std::tuple<std::string, std::any, std::any>> histParameterList{
        {"ModID", 0.,                                                                               640.                                                                                  },
        {"x_0",   df.Min("x_0"),                                                                    df.Max("x_0")                                                                         },
        {"x_1",   df.Min("x_1"),                                                                    df.Max("x_1")                                                                         },
        {"x_2",   df.Min("x_2"),                                                                    df.Max("x_2")                                                                         },
        {"p_0",   df.Min("p_0"),                                                                    df.Max("p_0")                                                                         }, // for SimECAL
        {"p_1",   df.Min("p_1"),                                                                    df.Max("p_1")                                                                         }, // for SimECAL
        {"p_2",   df.Min("p_2"),                                                                    df.Max("p_2")                                                                         }, // for SimECAL
        {"p0_0",  df.Min("p0_0"),                                                                   df.Max("p0_0")                                                                        }, // for SimECAL
        {"p0_1",  df.Min("p0_1"),                                                                   df.Max("p0_1")                                                                        }, // for SimECAL
        {"p0_2",  df.Min("p0_2"),                                                                   df.Max("p0_2")                                                                        }, // for SimECAL
        {"t",     0.,                                                                               std::function([&]() -> double { return *df.Mean("t") + 3 * *df.StdDev("t"); })        },
        {"t0",    0.,                                                                               std::function([&]() -> double { return *df.Mean("t0") + 3 * *df.StdDev("t0"); })      },
        {"TrkID", 0.,                                                                               std::function([&]() -> double { return *df.Mean("TrkID") + 2 * *df.StdDev("TrkID"); })},
        {"Edep",  std::function([&]() -> double { return *df.Mean("Edep") - *df.StdDev("Edep"); }), std::function([&]() -> double { return *df.Mean("Edep") + *df.StdDev("Edep"); })      }, // for SimECAL
        {"Ek0",   std::function([&]() -> double { return *df.Mean("Ek0") - *df.StdDev("Ek0"); }),   std::function([&]() -> double { return *df.Mean("Ek0") + *df.StdDev("Ek0"); })        }, // for SimECAL
        {"Ek",    std::function([&]() -> double { return *df.Mean("Ek") - *df.StdDev("Ek"); }),     std::function([&]() -> double { return *df.Mean("Ek") + *df.StdDev("Ek"); })          }  // for SimECAL
        // {"Edep",  0.,                                                                                 std::function([&]() -> double { return *df.Mean("Edep") + 0.5 * *df.StdDev("Edep"); })},// for SimPhaseI
        // {"Ek0",   0.,                                                                                 std::function([&]() -> double { return *df.Mean("Ek0") + 0.5 * *df.StdDev("Ek0"); })  },// for SimPhaseI
        // {"Ek",    0.,                                                                                 std::function([&]() -> double { return *df.Mean("Ek") + 0.5 * *df.StdDev("Ek"); })    },// for SimPhaseI
        // {"p_0",    std::function([&]() -> double { return *df.Mean("p_0") - 2 * *df.StdDev("p_0"); }),   std::function([&]() -> double { return *df.Mean("p_0") + 2 * *df.StdDev("p_0"); })      },// for SimPhaseI
        // {"p_1",    std::function([&]() -> double { return *df.Mean("p_1") - 2 * *df.StdDev("p_1"); }),   std::function([&]() -> double { return *df.Mean("p_1") + 2 * *df.StdDev("p_1"); })      },// for SimPhaseI
        // {"p_2",    std::function([&]() -> double { return *df.Mean("p_2") - 2 * *df.StdDev("p_2"); }),   std::function([&]() -> double { return *df.Mean("p_2") + 2 * *df.StdDev("p_2"); })      },// for SimPhaseI
        // {"p0_0",   std::function([&]() -> double { return *df.Mean("p0_0") - 2 * *df.StdDev("p0_0"); }), std::function([&]() -> double { return *df.Mean("p0_0") + 2 * *df.StdDev("p0_0"); })    },// for SimPhaseI
        // {"p0_1",   std::function([&]() -> double { return *df.Mean("p0_1") - 2 * *df.StdDev("p0_1"); }), std::function([&]() -> double { return *df.Mean("p0_1") + 2 * *df.StdDev("p0_1"); })    },// for SimPhaseI
        // {"p0_2",   std::function([&]() -> double { return *df.Mean("p0_2") - 2 * *df.StdDev("p0_2"); }), std::function([&]() -> double { return *df.Mean("p0_2") + 2 * *df.StdDev("p0_2"); })    },// for SimPhaseI
    };

    std::vector<ROOT::RDF::RResultPtr<TH1>> histList;
    std::ranges::transform(histParameterList, std::back_inserter(histList),
                           [&](auto&& parameter) -> ROOT::RDF::RResultPtr<TH1> {
                               auto&& [expression, xLow, xUp]{parameter};
                               constexpr auto Cast{[](std::any& any) -> double {
                                   if (const auto x{std::any_cast<ROOT::RDF::RResultPtr<double>>(&any)}) {
                                       return **x;
                                   } else if (const auto x{std::any_cast<double>(&any)}) {
                                       return *x;
                                   } else if (const auto x{std::any_cast<std::function<double()>>(&any)}) {
                                       return (*x)();
                                   }
                                   return 0;
                               }};
                               std::clog << "Hist: " << expression << std::endl;
                               const auto xMin{Cast(xLow)};
                               const auto xMax{Cast(xUp)};
                               const auto margin{(xMax - xMin) / nBinsValueType};
                               return df.Histo1D({expression.data(), expression.data(), nBinsValueType, xMin - margin, xMax + margin}, expression);
                           });

    for (auto aHist : histList) {
        aHist->Write();
    }
    dstDir->Write();
    dstFile->Close();

    return 0;
}
