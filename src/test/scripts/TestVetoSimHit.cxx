#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TKey.h"
#include "TPad.h"
#include "TSystem.h"

#include <iostream>
#include <string>

const std::string dataTupleName{"VetoSimHit"};

const void Judge(double pValue) {
    const std::string boldInvert{"\x1B[1m\x1B[7m"};
    const std::string boldRedInvert{boldInvert + "\x1B[38;5;9m"};
    const std::string boldYellowInvert{boldInvert + "\x1B[38;5;11m"};
    const std::string boldGreenInvert{boldInvert + "\x1B[38;5;10m"};
    const std::string reset{"\x1B[0m"};

    auto normQuantile2End{TMath::NormQuantile(1 - pValue / 2)};
    if (normQuantile2End > 5) {
        std::cout << boldRedInvert << "FAILED" << reset << "( Z = " << normQuantile2End << "sigma )" << std::endl;
    } else if (normQuantile2End > 3) {
        std::cout << boldYellowInvert << "SUSPICIOUS" << reset << "( Z = " << normQuantile2End << "sigma )" << std::endl;
    } else if (normQuantile2End > 0) {
        std::cout << boldGreenInvert << "PASSED" << reset << "( Z = " << normQuantile2End << "sigma )" << std::endl;
    } else {
        std::cout << boldGreenInvert << "IDENTICAL" << reset << "( Z = " << normQuantile2End << "sigma )" << std::endl;
    }
}

// int main()
auto TestVetoSimHit(std::string moduleName, std::string testFileName, std::string sampleFileName) {
    gROOT->SetBatch(kTRUE);

    auto SaveRegressionResult{
        [&](TH1D* h1, TH1D* h2, std::string histName) {
            auto i1{h1->Integral()};
            auto i2{h2->Integral()};
            if (i1 != 0) {
                h1->Scale(1 / i1);
            } else {
                std::cerr << "Warning: Histogram h1 has zero integral, skipping scaling." << std::endl;
            }
            if (i2 != 0) {
                h2->Scale(1 / i2);
            } else {
                std::cerr << "Warning: Histogram h2 has zero integral, skipping scaling." << std::endl;
            }
            // pull

            auto nBins{h1->GetNbinsX()};
            auto totalBins{nBins + 2};
            auto xLow{h1->GetXaxis()->GetXmin()};
            auto xUp{h1->GetXaxis()->GetXmax()};
            auto pull{new TH1D("pull", "pull", nBins, xLow, xUp)};
            pull->SetMarkerStyle(kPlus);

            for (int i = 0; i < totalBins; i++) {
                auto diff{h1->GetBinContent(i) - h2->GetBinContent(i)};
                auto err1{h1->GetBinError(i)};
                auto err2{h2->GetBinError(i)};
                auto err{std::hypot(err1, err2)};
                if (err == 0) {
                    pull->SetBinContent(i, 0);
                } else {
                    pull->SetBinContent(i, diff / err);
                }
                pull->SetBinError(i, 1);
            }
            // draw
            auto canvasName{moduleName + "_VetoSimHit_" + histName};
            auto pad1Name{"hist"};
            auto pad2Name{"pull"};
            TCanvas* c1 = new TCanvas(canvasName.data(), canvasName.data(), 800, 600);
            TPad* pad1 = new TPad(pad1Name, pad1Name, 0, 0.4, 1, 1.0);
            TPad* pad2 = new TPad(pad2Name, pad2Name, 0, 0.0, 1, 0.4);

            pad1->SetBottomMargin(0.06);
            pad2->SetTopMargin(0.06);
            pad2->SetBottomMargin(0.2);

            pad1->Draw();
            pad2->Draw();
            h1->SetLineColor(kRed);
            h1->SetLineWidth(1);
            h2->SetLineColor(kBlue);
            h2->SetLineWidth(1);
            h1->SetStats(false);
            h2->SetStats(false);

            c1->cd();
            pad1->cd();
            h2->DrawClone();
            h1->DrawClone("SAME");
            pad2->cd();
            pull->Draw();
            auto file{TFile("regression_report.root", "update")};
            c1->Write();
        }};

    gErrorIgnoreLevel = kError;

    ROOT::RDataFrame df0("G4Run0/" + dataTupleName, testFileName);
    auto sampleFile{new TFile(sampleFileName.data(), "READ")};
    if (!sampleFile->IsOpen()) {
        std::cerr << "Err: Sample file not found or could not be opened: " << sampleFileName << std::endl;
        return 1;
    }
    auto sampleDir{static_cast<TDirectory*>(sampleFile->Get((moduleName + "/" + dataTupleName).data()))};
    if (!sampleDir) {
        std::cerr << "Err: data tuple not found: " << (moduleName + "/" + dataTupleName) << std::endl;
        return 1;
    }

    std::clog << "[Note] testing. Module: " << moduleName << "; " << "DataTuple: " << dataTupleName << std::endl;

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

    auto histKeyList{sampleDir->GetListOfKeys()};
    std::clog << "Get " << histKeyList->GetEntries() << " entries from sample file." << std::endl;
    int idx{};
    for (auto&& obj : *histKeyList) {
        const auto key{static_cast<TKey*>(obj)};
        const auto hist{static_cast<TH1D*>(key->ReadObj())};
        const auto branchName{hist->GetName()};
        const auto xMax{hist->GetXaxis()->GetXmax()};
        const auto xMin{hist->GetXaxis()->GetXmin()};
        const auto nBins{hist->GetNbinsX()};

        auto testHist{df.Histo1D({"", "", nBins, xMin, xMax}, branchName)};
        std::cout << "\n"
                  << "(#" << ++idx << ") " << " Column " << branchName << std::endl;
        auto pValue{hist->Chi2Test(testHist.GetPtr(), "P")};
        Judge(pValue);
        SaveRegressionResult(hist, testHist.GetPtr(), branchName);
        std::cout << "\n";
    }
    std::clog << "Test " << moduleName << " " << dataTupleName << " end." << std::endl;
    return 0;
}
