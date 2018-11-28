#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>
#include <TH1.h>

#include <CTData.h>
#include <CTCuts.h>

void hcal_diagnostic() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    std::map<TString, TH1*> histograms;

    //--------------------------------------------------------------------------
    // Fill histograms
    for (auto const& k : kinematics) {
        TString histoName = Form("h_%s", k.Data());
        TString drawStr = Form("H.cal.etottracknorm>>%s(260,0,1.3)",histoName.Data());
        data->GetChain(k)->Draw(drawStr.Data(), cuts->Get("hCalShould"));
        histograms[k] = (TH1*) gDirectory->Get(histoName.Data());
    }

    //--------------------------------------------------------------------------
    // Draw histograms
    std::vector<Int_t> colors = {kRed+1, kGreen+2, kCyan+1, kViolet-3};
    histograms["LH2_Q2_8"]->SetLineColor(colors[0]);
    histograms["LH2_Q2_10"]->SetLineColor(colors[1]);
    histograms["LH2_Q2_12"]->SetLineColor(colors[2]);
    histograms["LH2_Q2_14"]->SetLineColor(colors[3]);
    histograms["C12_Q2_8"]->SetLineColor(colors[0]);
    histograms["C12_Q2_10"]->SetLineColor(colors[1]);
    histograms["C12_Q2_12"]->SetLineColor(colors[2]);
    histograms["C12_Q2_14"]->SetLineColor(colors[3]);

    TCanvas *c1  = new TCanvas("c1", "canvas", 800, 600); // problems here

    c1->Divide(2,1);
    c1->cd(1);
    histograms["LH2_Q2_10"]->Draw();
    histograms["LH2_Q2_8"]->Draw("SAME");
    histograms["LH2_Q2_12"]->Draw("SAME");
    histograms["LH2_Q2_14"]->Draw("SAME");

    c1->cd(2);
    histograms["C12_Q2_10"]->Draw();
    histograms["C12_Q2_8"]->Draw("SAME");
    histograms["C12_Q2_12"]->Draw("SAME");
    histograms["C12_Q2_14"]->Draw("SAME");
}
