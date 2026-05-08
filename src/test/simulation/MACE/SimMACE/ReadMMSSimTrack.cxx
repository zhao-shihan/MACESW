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
auto DataTupleName{"MMSSimTrack"};

auto ReadMMSSimTrack(auto moduleName, auto srcFileName, auto dstFileName) -> int {
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

    auto df{df0.Define("x0_0", "x0[0]")
                .Define("x0_1", "x0[1]")
                .Define("x0_2", "x0[2]")
                .Define("c0_0", "c0[0]")
                .Define("c0_1", "c0[1]")
                .Define("p0_0", "p0[0]")
                .Define("p0_1", "p0[1]")
                .Define("p0_2", "p0[2]")};

    std::vector<std::tuple<std::string, std::any, std::any>> histParameterList{
        {"phi0",   df.Min("phi0"),   df.Max("phi0")  },
        {"z0",     df.Min("z0"),     df.Max("z0")    },
        {"theta0", df.Min("theta0"), df.Max("theta0")},
        {"chi2",   0.,               df.Max("chi2")  },
        {"Ek0",    0.,               df.Max("Ek0")   },
        {"r0",     0.,               df.Max("r0")    },
        {"t0",     0.,               df.Max("t0")    },
        {"p0_0",   df.Min("p0_0"),   df.Max("p0_0")  },
        {"p0_1",   df.Min("p0_1"),   df.Max("p0_1")  },
        {"p0_2",   df.Min("p0_2"),   df.Max("p0_2")  },
        {"x0_0",   df.Min("x0_0"),   df.Max("x0_0")  },
        {"x0_1",   df.Min("x0_1"),   df.Max("x0_1")  },
        {"x0_2",   df.Min("x0_2"),   df.Max("x0_2")  },
        {"c0_0",   df.Min("c0_0"),   df.Max("c0_0")  },
        {"c0_1",   df.Min("c0_1"),   df.Max("c0_1")  }
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
