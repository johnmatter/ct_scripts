#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TStyle.h>
#include <TCanvas.h>
#include <TH1F.h>

#include <CTData.h>
#include <CTCuts.h>

// Investigating what the distribution of focal plane values look like
// For the old and new rootfiles
void plot_fp_values() {

    // Stats display
    gStyle->SetOptStat("nemruoi");

    CTData *dataOld = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTData *dataNew = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");
    CTCuts *cutsJson = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to print to
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/efficiency/coin/distributions/distributions_fp.pdf";

    // Events-per-delta histograms for weighting
    std::map<TString, TH1F*> histograms;

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Define cuts
    std::vector<TString> cuts = {"open", "should", "did"};

    std::map<TString, TCut> cutMap;
    cutMap["open"]   = TCut("");
    cutMap["should"] = cutsJson->Get("pScinShouldh");
    cutMap["did"]    = cutsJson->Get("pScinDidh");

    // Quantities to plot
    std::vector<TString> branches = {"P.dc.x_fp","P.dc.y_fp","P.dc.xp_fp","P.dc.yp_fp","P.gtr.dp"};
    std::map<TString, Double_t> hiBin;
    std::map<TString, Double_t> loBin;
    std::map<TString, Int_t> nBins;

    TString b;
    b = "P.dc.x_fp";
    nBins[b]=200; loBin[b]=-50; hiBin[b]=50;

    b = "P.dc.y_fp";
    nBins[b]=200; loBin[b]=-50; hiBin[b]=50;

    b = "P.dc.xp_fp";
    nBins[b]=200; loBin[b]=-0.4; hiBin[b]=0.4;

    b = "P.dc.yp_fp";
    nBins[b]=200; loBin[b]=-0.4; hiBin[b]=0.4;

    b = "P.gtr.dp";
    nBins[b]=200; loBin[b]=-50; hiBin[b]=50;

    // Aesthetics
    Int_t oldColor = kBlue-4;
    Int_t newColor = kRed+1;
    Int_t oldFill  = 3345;
    Int_t newFill  = 3354;

    // ------------------------------------------------------------------------
    // Generate histograms
    TString drawMe, hOldName, hNewName;
    Double_t lo, hi;
    Int_t n;

    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout << "kinematics,branch,cut,hcana,entries,integral" << std::endl;
    for (auto const &k : kinematics) {
        for (auto const &branch : branches) {
            for (auto const &cut : cuts) {

                n  = nBins[branch];
                lo = loBin[branch];
                hi = hiBin[branch];

                // Old
                hOldName = Form("old_%s_%s_%s", k.Data(), branch.Data(), cut.Data());
                drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(), hOldName.Data(), n, lo, hi);
                dataOld->GetChain(k)->Draw(drawMe.Data(), cutMap[cut], "goff");
                histograms[hOldName] = (TH1F*) gDirectory->Get(hOldName.Data());

                // New
                hNewName = Form("new_%s_%s_%s", k.Data(), branch.Data(), cut.Data());
                drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(), hNewName.Data(), n, lo, hi);
                dataNew->GetChain(k)->Draw(drawMe.Data(), cutMap[cut], "goff");
                histograms[hNewName] = (TH1F*) gDirectory->Get(hNewName.Data());

                std::cout << k << "," << branch << "," << cut << "," << "old" << ","
                          << histograms[hNewName]->GetEntries() << ","
                          << histograms[hNewName]->GetEffectiveEntries() << std::endl;
                std::cout << k << "," << branch << "," << cut << "," << "new" << ","
                          << histograms[hOldName]->GetEntries() << ","
                          << histograms[hOldName]->GetEffectiveEntries() << std::endl;
            }
        }
    }
    std::cout<<std::endl;
    std::cout<<std::endl;

    // ------------------------------------------------------------------------
    // Save to PDF
    TCanvas* c = new TCanvas("c", "c", 640, 480);

    // open PDF; "filename.pdf["
    c->Print((pdfFilename+"[").Data());

    // Loop over combos of kinematics, branches, and cuts and write histos to PDF
    for (auto const &k : kinematics) {
        for (auto const &branch : branches) {
            std::cout << "Write: " << k << ": " << branch << std::endl;
            for (auto const &cut : cuts) {

                // Form names to get histos
                hOldName = Form("old_%s_%s_%s", k.Data(), branch.Data(), cut.Data());
                hNewName = Form("new_%s_%s_%s", k.Data(), branch.Data(), cut.Data());

                // Get histos
                TH1F* hOld = histograms[hOldName];
                TH1F* hNew = histograms[hNewName];

                // Set title. Only need to set one, since we're overlaying them
                hOld->SetTitle(Form("%s, %s: cut=%s", k.Data(), branch.Data(), cut.Data()));
                hNew->SetTitle(Form("%s, %s: cut=%s", k.Data(), branch.Data(), cut.Data()));

                // Aesthetics
                hOld->SetLineColor(oldColor);
                hOld->SetFillColor(oldColor);
                hOld->SetFillStyle(oldFill);
                hNew->SetLineColor(newColor);
                hNew->SetFillColor(newColor);
                hNew->SetFillStyle(newFill);

                // Draw
                hNew->Draw("hist");
                hOld->Draw("same hist");

                // write page to PDF
                c->Print(pdfFilename.Data());

            } // loop over cuts
        } // loop over branches
    } // loop over kinematics

    // close PDF; "filename.pdf]"
    c->Print((pdfFilename+"]").Data());
}
