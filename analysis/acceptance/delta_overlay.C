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
void delta_overlay() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    TCut cutShould = cuts->Get("hBetaCut") && cuts->Get("hms_acceptanceCut")
                    && cuts->Get("hWCut") && cuts->Get("hHodoTimeCut");

    std::vector<TString> kinematics = {"LH2_Q2_8", "LH2_Q2_10", "LH2_Q2_12", "LH2_Q2_14",
                                       "C12_Q2_8", "C12_Q2_10", "C12_Q2_12", "C12_Q2_14"};

    // Plot
    for (auto const &k: kinematics) {
        std::cout << "Calculating for " << k << std::endl;

        TString drawThis = Form("H.gtr.dp>>%s_dp(80,-20,20)", k.Data());

        data->GetChain(k)->Draw(drawThis.Data(), cutShould);
    }

    // Draw
    gStyle->SetOptStat(0);
    TCanvas* c = new TCanvas("c", "hms_acceptance", 640, 640);
    c->Print("hms_acceptance.pdf["); // open PDF

    TLegend *leg= new TLegend(0.1,0.7,0.3,0.9);

    std::vector<Int_t> colors = {kRed+1, kOrange+8, kOrange, kGreen+2,
                                 kCyan-3, kAzure-4, kBlue-3, kViolet-3};
    Int_t n=0; // color index

    for (auto const &k: kinematics) {
        // Get histogram; make it pretty
        TString hName = Form("%s_dp", k.Data());
        TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
        h->SetLineColor(colors[n++]);
        h->Scale(1/h->Integral());
        h->Draw("same hist");

        // Legend
        leg->AddEntry(h, k.Data(), "l");
    }
    leg->Draw();
    c->Print("hms_acceptance.pdf"); // write page to PDF

    c->Print("hms_acceptance.pdf]"); // close fPDF
}
