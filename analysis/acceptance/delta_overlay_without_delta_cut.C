#include <utility>
#include <vector>
#include <map>

#include <TCut.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>

#include <CTData.h>
#include <CTCuts.h>
void delta_overlay_without_delta_cut() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts_no_pHGC_pCal.json");

    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14",
                                       "LH2_Q2_10_pion_collimator",
                                       "LH2_Q2_10_large_collimator"};

    // Plot
    TString drawThis;
    for (auto const &k: kinematics) {
        std::cout << "Calculating for " << k << std::endl;

        // What cut to use for drawing?
        TCut drawCut;
        if (k.Contains("LH2")) {
            drawCut = cuts->Get("coinCutsLH2noDelta");
        }
        if (k.Contains("C12")) {
            drawCut = cuts->Get("coinCutsC12noDelta");
        }

        drawThis = Form("H.gtr.dp>>%s_hms_dp(120,-15,15)", k.Data());
        data->GetChain(k)->Draw(drawThis.Data(), drawCut);

        drawThis = Form("P.gtr.dp>>%s_shms_dp(180,-25,20)", k.Data());
        data->GetChain(k)->Draw(drawThis.Data(), drawCut);
    }


    // Set up some drawing stuff ----------------------------------------------
    Int_t n=0; // color index
    std::vector<Int_t> colors = {kRed+1, kOrange+8, kOrange, kGreen+2,
                                 kCyan-3, kAzure-4, kBlue-3, kViolet-3,
                                 kOrange+10, kOrange+5};

    // Draw HMS ---------------------------------------------------------------
    gStyle->SetOptStat(0);
    TCanvas* cHMS = new TCanvas("cHMS", "hms_acceptance", 640, 640);
    cHMS->Print("hms_acceptance_without_delta_cut.pdf["); // open PDF

    // HMS overlay
    TLegend *legHMS = new TLegend(0.1,0.7,0.3,0.9);
    n=0; // color index
    for (auto const &k: kinematics) {
        // Get histogram; make it pretty
        TString hName = Form("%s_hms_dp", k.Data());
        TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
        h->SetLineColor(colors[n++]);
        h->Scale(1/h->Integral());
        h->Draw("same hist");

        // Legend
        legHMS->AddEntry(h, k.Data(), "l");
    }
    legHMS->Draw();
    cHMS->Print("hms_acceptance_without_delta_cut.pdf"); // write page to PDF

    // Individual pages
    gStyle->SetOptStat(111111);
    n=0; // color index
    for (auto const &k: kinematics) {
        TString hName = Form("%s_hms_dp", k.Data());
        TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
        h->Draw("");
        cHMS->Print("hms_acceptance_without_delta_cut.pdf"); // write page to PDF
    }

    cHMS->Print("hms_acceptance_without_delta_cut.pdf]"); // close fPDF

    // Draw SHMS ---------------------------------------------------------------
    gStyle->SetOptStat(0);
    TCanvas* cSHMS = new TCanvas("cSHMS", "shms_acceptance", 640, 640);
    cSHMS->Print("shms_acceptance_without_delta_cut.pdf["); // open PDF

    // SHMS overlay
    TLegend *legSHMS = new TLegend(0.1,0.7,0.3,0.9);
    n=0; // color index
    for (auto const &k: kinematics) {
        // Get histogram; make it pretty
        TString hName = Form("%s_shms_dp", k.Data());
        TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
        h->SetLineColor(colors[n++]);
        h->Scale(1/h->Integral());
        h->Draw("same hist");

        // Legend
        legSHMS->AddEntry(h, k.Data(), "l");
    }
    legSHMS->Draw();
    cSHMS->Print("shms_acceptance_without_delta_cut.pdf"); // write page to PDF

    // Individual pages
    gStyle->SetOptStat(111111);
    n=0; // color index
    for (auto const &k: kinematics) {
        TString hName = Form("%s_shms_dp", k.Data());
        TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
        h->Draw("");
        cSHMS->Print("shms_acceptance_without_delta_cut.pdf"); // write page to PDF
    }

    cSHMS->Print("shms_acceptance_without_delta_cut.pdf]"); // close fPDF
}
