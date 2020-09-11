#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH2F.h>

#include <Efficiency1D.h>
#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

void plot_cer_track() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to print to
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/efficiency/coin/distributions/distributions_cer_track.pdf";

     // Events-per-delta histograms for weighting
    std::map<TString, TH2F*> histograms;

    // Keep vector of the keys because I want to loop over it
    // at the end to print all the histos to PDF
    std::vector<TString> keys;

    // Which detectors
    // std::vector<TString> detectors = {"pNGCer","hCer"};
    std::vector<TString> detectors = {"hCer"};
    TString detectorBranch;

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // ------------------------------------------------------------------------
    // Create histograms for each (kinematics,detector) pair.
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            std::cout << k << ", " << d << std::endl;

            // Get data
            TChain* chain = data->GetChain(k.Data());

            // Which spectrometer?
            TString spec = d[0];
            spec.ToUpper();

            // Which cuts?
            TCut shouldCut;
            TCut didCut;

            // HMS Cherenkov
            if (d=="hCer") {
                shouldCut = cuts->Get("hCerShould");
                didCut = cuts->Get("hCerDid");
                detectorBranch = "cer";
            }

            // SHMS NG Cherenkov
            if (d=="pNGCer") {
                shouldCut = cuts->Get("pNGCerShould");
                didCut = cuts->Get("pNGCerCut");
                detectorBranch = "ngcer";
            }

            // For carbon, also include missing energy and momentum cut
            if (k.Contains("C12")) {
                shouldCut = shouldCut && cuts->Get("pC12EMissPMissCut");
            }

            // Define branches, limits
            TString drawXBranch, drawYBranch;
            Int_t drawXBins, drawYBins;
            Double_t drawXLo, drawYLo;
            Double_t drawXHi, drawYHi;

            drawXBranch = Form("%s.%s.yAtCer", spec.Data(), detectorBranch.Data());
            drawXBins = 100;
            drawXLo = -50;
            drawXHi = +50;

            drawYBranch = Form("%s.%s.xAtCer", spec.Data(), detectorBranch.Data());
            drawYBins = 120;
            drawYLo = -60;
            drawYHi = +60;

            TString histoName, drawMe;

            // ----------------------
            // Draw should
            histoName = Form("%s_%s_should", k.Data(), d.Data());
            drawMe    = Form("%s:%s>>%s(%d,%f,%f,%d,%f,%f)",
                                  drawYBranch.Data(), drawXBranch.Data(), histoName.Data(),
                                  drawXBins, drawXLo, drawXHi,
                                  drawYBins, drawYLo, drawYHi);
            chain->Draw(drawMe.Data(), shouldCut, "goff");

            // Add to map
            histograms[histoName] = (TH2F*) gDirectory->Get(histoName.Data());
            keys.push_back(histoName);

            // ----------------------
            // Draw did
            histoName = Form("%s_%s_did", k.Data(), d.Data());
            drawMe    = Form("%s:%s>>%s(%d,%f,%f,%d,%f,%f)",
                                  drawYBranch.Data(), drawXBranch.Data(), histoName.Data(),
                                  drawXBins, drawXLo, drawXHi,
                                  drawYBins, drawYLo, drawYHi);
            chain->Draw(drawMe.Data(), shouldCut && didCut, "goff");

            // Add to map
            histograms[histoName] = (TH2F*) gDirectory->Get(histoName.Data());
            keys.push_back(histoName);

            // ----------------------
            // Draw didn't
            histoName = Form("%s_%s_didnt", k.Data(), d.Data());
            drawMe    = Form("%s:%s>>%s(%d,%f,%f,%d,%f,%f)",
                                  drawYBranch.Data(), drawXBranch.Data(), histoName.Data(),
                                  drawXBins, drawXLo, drawXHi,
                                  drawYBins, drawYLo, drawYHi);
            chain->Draw(drawMe.Data(), shouldCut && !(didCut), "goff");

            // Add to map
            histograms[histoName] = (TH2F*) gDirectory->Get(histoName.Data());
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
        for (auto const &d : detectors) {

            TString shouldKey = Form("%s_%s_should", k.Data(), d.Data());
            TString didKey = Form("%s_%s_did", k.Data(), d.Data());
            TString didntKey = Form("%s_%s_didnt", k.Data(), d.Data());

            TH2F* hShould = histograms[shouldKey];
            TH2F* hDid    = histograms[didKey];
            TH2F* hDidnt  = histograms[didntKey];

            hShould->SetTitle(shouldKey);
            hDid->SetTitle(didKey);
            hDidnt->SetTitle(didntKey);

            TString printMe = Form("%s,%s,%.0f,%.0f,%.0f", k.Data(), d.Data(),
                                    hShould->GetSum(), hDid->GetSum(), hDidnt->GetSum());
            std::cout << printMe << std::endl;

            hShould->Draw("colz");
            cEff->Print(pdfFilename.Data()); // write page to PDF

            hDid->Draw("colz");
            cEff->Print(pdfFilename.Data()); // write page to PDF

            hDidnt->Draw("colz");
            cEff->Print(pdfFilename.Data()); // write page to PDF
        }
    }
    cEff->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"
}
