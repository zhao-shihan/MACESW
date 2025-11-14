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
auto DataTupleName{"TTCSimHit"};

auto ReadTTCSimHit(auto moduleName, auto srcFileName, auto dstFileName) -> int {
    ROOT::RDataFrame df0{Form("G4Run0/%s", DataTupleName), srcFileName};
    const auto dstFile{new TFile(dstFileName, "UPDATE")};
    auto moduleDir{static_cast<TDirectory*>(dstFile->Get(moduleName))};
    if (!moduleDir) {
        std::clog << "DST File: Directory named " << moduleName << " is a new dir, create it." << std::endl;
        moduleDir = dstFile->mkdir(moduleName);
    }
    auto dstDir{moduleDir->mkdir(DataTupleName)};
    if (!dstDir) {
        std::cout << "Make TDirectory " << DataTupleName << " Failed. ";
        if (moduleDir->Get(DataTupleName)) {
            std::cout << "Already exist." << std::endl;
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
                .Define("x0_0", "x0[0]")
                .Define("x0_1", "x0[1]")
                .Define("x0_2", "x0[2]")
                .Define("p0_0", "p0[0]")
                .Define("p0_1", "p0[1]")
                .Define("p0_2", "p0[2]")};

    std::vector<std::tuple<std::string, std::any, std::any>> histParameterList{
        // {"TileID",  0.,            4000.                                                                                     },
        {"Ek",   0., df.Max("Ek")                                                                        },
        // {"Ek0",     0.,            df.Max("Ek0")                                                                             },
        // {"x_0",      df.Min("x_0"),  df.Max("x_0")                                                                              },
        // {"x_1",      df.Min("x_1"),  df.Max("x_1")                                                                              },
        // {"x_2",      df.Min("x_2"),  df.Max("x_2")                                                                              },
        // {"p_0",      df.Min("p_0"),  df.Max("p_0")                                                                              },
        // {"p_1",      df.Min("p_1"),  df.Max("p_1")                                                                              },
        // {"p_2",      df.Min("p_2"),  df.Max("p_2")                                                                              },
        // {"p0_0",     df.Min("p0_0"), df.Max("p0_0")                                                                             },
        // {"p0_1",     df.Min("p0_1"), df.Max("p0_1")                                                                             },
        // {"p0_2",     df.Min("p0_2"), df.Max("p0_2")                                                                             },
        // {"t",       0,             std::function([&]() -> double { return *df.Mean("t") + 5 * *df.StdDev("t"); })            },
        // {"nOptPho", 0,             std::function([&]() -> double { return *df.Mean("nOptPho") + 5 * *df.StdDev("nOptPho"); })},
        {"Edep", 0., std::function([&]() -> double { return *df.Mean("Edep") + 5 * *df.StdDev("Edep"); })}
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
