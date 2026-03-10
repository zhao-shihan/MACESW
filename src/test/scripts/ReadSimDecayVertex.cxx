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
auto DataTupleName{"SimDecayVertex"};

auto ReadSimDecayVertex(auto moduleName, auto srcFileName, auto dstFileName) -> int {
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
                .Define("x_2", "x[2]")};
    std::vector<std::tuple<std::string, std::any, std::any>> histParameterList{
        {"t",   0.,                                                                                 std::function([&]() -> double { return *df.Mean("t") + 3 * *df.StdDev("t"); })    },
        {"x_0", std::function([&]() -> double { return *df.Mean("x_0") - 3 * *df.StdDev("x_0"); }), std::function([&]() -> double { return *df.Mean("x_0") + 3 * *df.StdDev("x_0"); })},
        {"x_1", std::function([&]() -> double { return *df.Mean("x_1") - 3 * *df.StdDev("x_1"); }), std::function([&]() -> double { return *df.Mean("x_1") + 3 * *df.StdDev("x_1"); })},
        {"x_2", std::function([&]() -> double { return *df.Mean("x_2") - 3 * *df.StdDev("x_2"); }), std::function([&]() -> double { return *df.Mean("x_2") + 3 * *df.StdDev("x_2"); })}
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
