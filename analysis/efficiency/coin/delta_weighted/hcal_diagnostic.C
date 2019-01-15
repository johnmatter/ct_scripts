#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>

#include <CTData.h>
#include <CTCuts.h>

// Plot and fit distributions of HMS calorimeter energy deposition.
// I'm interested in whether or not the calorimeter is properly calibrated.
// In addition, I'm wondering if there are pion events "leaking" into our
// sample of what should be electrons based on HMS Cherenkov cut.
// That is, are there a lot of etottracknorm=0 events?
void hcal_diagnostic() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    std::map<TString, TH1*> histograms;
    std::map<TString, TF1*> fits;

    //--------------------------------------------------------------------------
    // Fill histograms
    for (auto const& k : kinematics) {
        TString histoName = Form("h_%s", k.Data());
        TString drawStr = Form("H.cal.etottracknorm>>%s(260,0,1.3)",histoName.Data());
        data->GetChain(k)->Draw(drawStr.Data(), cuts->Get("hCalShould"));
        histograms[k] = (TH1*) gDirectory->Get(histoName.Data());
    }

    //--------------------------------------------------------------------------
    // Fit histograms; assume range of interest is 0.5 to 1,5
    for (auto const& k : kinematics) {
        std::cout << "Fitting " << k << std::endl;
        TString fitName = Form("%s_fit",k.Data());
        fits[k] = new TF1(fitName.Data(),"gaus",0.5,1.5);
        histograms[k]->Fit(fits[k],"R0");
        std::cout << std::endl;
    }

    // Print results
    std::cout << "target,Q2,mean,meanError,sigma,sigmaError" << std::endl;
    for (auto const& k : kinematics) {
        Double_t mean, meanError, sigma, sigmaError;
        mean       = fits[k]->GetParameter(1);
        meanError  = fits[k]->GetParError(1);
        sigma      = fits[k]->GetParameter(2);
        sigmaError = fits[k]->GetParError(2);

        std::cout << data->GetTarget(k) << ","
                  << data->GetQ2(k)     << ","
                  << mean               << ","
                  << meanError          << ","
                  << sigma              << ","
                  << sigmaError
                  << std::endl;
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
    histograms["C12_Q2_10"]->Draw();
    histograms["C12_Q2_8"]->Draw("SAME");
    histograms["C12_Q2_12"]->Draw("SAME");
    histograms["C12_Q2_14"]->Draw("SAME");

    c1->cd(2);
    histograms["LH2_Q2_10"]->Draw();
    histograms["LH2_Q2_8"]->Draw("SAME");
    histograms["LH2_Q2_12"]->Draw("SAME");
    histograms["LH2_Q2_14"]->Draw("SAME");
}
