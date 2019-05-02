#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>

#include <Efficiency1D.h>
#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

void plot_cer_track() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to print to
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/efficiency/coin/distributions/distributions.pdf";

     // Events-per-delta histograms for weighting
    std::map<TString, TH1*> histograms;

    // Keep vector of the keys because I want to loop over it
    // at the end to print all the histos to PDF
    std::vector<TString> keys;

    // Which detectors
    std::vector<TString> detectors = {"pCer","hCer"};

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};
                                       // "LH2_Q2_10_pion_collimator",
                                       // "LH2_Q2_10_large_collimator",
                                       // "LH2_Q2_14_large_collimator",
                                       // "LH2_Q2_14_pion_collimator",
                                       // "C12_Q2_14_pion_collimator",
                                       // "C12_Q2_14_large_collimator"};

    // ------------------------------------------------------------------------
    // Create histograms for each (kinematics,detector) pair.
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            std::cout << k << ", " << d << std::endl;

            // Get data
            TChain* chain = data->GetChain(k.Data());

            // Which spectrometer?
            TString drawBranch;
            Int_t drawBins;
            Double_t drawLo;
            Double_t drawHi;
            switch (d[0]) {
                case 'h':
                    drawBranch = "H.gtr.dp";
                    drawBins = 20;
                    drawLo = -10;
                    drawHi = +10;
                case 'p':
                    drawBranch = "P.gtr.dp";
                    drawBins = 25;
                    drawLo = -10;
                    drawHi = +15;
            }

            // Which cuts?
            TCut shouldCut = cuts->Get("pBetaCut") && cuts->Get("hBetaCut");
            TCut didCut;

            // HMS Cherenkov
            if (d=="hCer") {
                shouldCut = shouldCut && cuts->Get("hCalCut") && cuts->Get("pCerCut");
                didCut = cuts->Get("hCerCut");
            }

            // HMS Calorimeter
            if (d=="hCal") {
                shouldCut = shouldCut && cuts->Get("hCerCut") && cuts->Get("pCerCut");
                didCut = cuts->Get("hCalCut");
            }

            // SHMS Cherenkov
            if (d=="pCer") {
                shouldCut = shouldCut && cuts->Get("hCalCut") && cuts->Get("hCerCut");
                didCut = cuts->Get("pCerCut");
            }

            // For carbon, also include missing energy and momentum cut
            if (k.Contains("C12")) {
                shouldCut = shouldCut && "abs(P.kin.secondary.pmiss)<0.3";
            }


            TString histoName, drawMe;

            // ----------------------
            // Draw should
            histoName = Form("%s_%s_should", k.Data(), d.Data());
            drawMe    = Form("%s>>%s(%d,%f,%f)",
                                  drawBranch.Data(), histoName.Data(),
                                  drawBins, drawLo, drawHi);
            chain->Draw(drawMe.Data(), shouldCut, "goff");

            // Add to map
            histograms[histoName] = (TH1*) gDirectory->Get(histoName.Data());
            keys.push_back(histoName);

            // ----------------------
            // Draw did
            histoName = Form("%s_%s_did", k.Data(), d.Data());
            drawMe    = Form("%s>>%s(%d,%f,%f)",
                                  drawBranch.Data(), histoName.Data(),
                                  drawBins, drawLo, drawHi);
            chain->Draw(drawMe.Data(), shouldCut && didCut, "goff");

            // Add to map
            histograms[histoName] = (TH1*) gDirectory->Get(histoName.Data());
            keys.push_back(histoName);

            // ----------------------
            // Draw didn't
            histoName = Form("%s_%s_didnt", k.Data(), d.Data());
            drawMe    = Form("%s>>%s(%d,%f,%f)",
                                  drawBranch.Data(), histoName.Data(),
                                  drawBins, drawLo, drawHi);
            chain->Draw(drawMe.Data(), shouldCut && !(didCut), "goff");

            // Add to map
            histograms[histoName] = (TH1*) gDirectory->Get(histoName.Data());
            keys.push_back(histoName);
        }
    }

    // ------------------------------------------------------------------------
    // Save to PDF

    // Turn on overflow/underflow stats
    gStyle->SetOptStat(1111111);

    TCanvas* cEff = new TCanvas("cEff", "Efficiency", 1024, 640);

    std::cout << "kinematics,detector,should,did,didnt" << std::endl;
    cEff->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["
    for (auto const &k : kinematics) {
        std::cout << printMe << std::endl;
        for (auto const &d : detectors) {
            TH1* hShould = histograms[Form("%s_%s_should", k.Data(), d.Data())];
            TH1* hDid    = histograms[Form("%s_%s_did", k.Data(), d.Data())];
            TH1* hDidnt  = histograms[Form("%s_%s_didnt", k.Data(), d.Data())];

            TString printMe = Form("%s,%s,%d,%d,%d", k.Data(), d.Data(),
                                    hShould->GetSum(), hDid->GetSum(), hDidnt->GetSum());
            std::cout << printMe << std::endl;

            hShould->SetLineColor(kBlack);
            hDid->SetLineColor(kGreen+2);
            hDidnt->SetLineColor(kRed+2);

            hShould->SetFillColor(kBlack);
            hDid->SetFillColor(kGreen+2);
            hDidnt->SetFillColor(kRed+2);

            hShould->SetFillStyle(3305);
            hDid->SetFillStyle(3345);
            hDidnt->SetFillStyle(3354);

            hShould->Draw();
            hDid->Draw("same");
            hDidnt->Draw("same");

            cEff->Print(pdfFilename.Data()); // write page to PDF
        }
    }
    cEff->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"
}
