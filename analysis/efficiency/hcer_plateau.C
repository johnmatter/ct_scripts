#include <algorithm>
#include <utility>
#include <vector>
#include <tuple>
#include <map>

#include <TGraphAsymmErrors.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TCut.h>

#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

// This macro calculates the efficiency of the HMS Cherenkov detector as
// a function of cuts on npeSum for electron events.

void hcer_plateau() {
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");
    std::vector<TString> targets = {"LH2","C12"};
    std::vector<Int_t> Q2s = {8, 10, 12, 14};

    // This map takes <target,Q^2> as a key
    std::map<std::pair<TString, Int_t>, TGraphAsymmErrors*> efficiencyGraphs;

    // This map takes <target,Q^2,cutParam> as a key
    std::map<std::tuple<TString, Int_t, Double_t>, Efficiency0D*> efficiencyCalculators;

    // Set up our cuts; we'll be scanning a cut threshold
    TCut cutShould = cuts->Get("hCerShould") && cuts->Get("hDeltaCut");
    TString cutDidString = "H.cer.npeSum > %f";
    std::vector<Double_t> cutParams = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    Double_t minParam = *std::min_element(cutParams.begin(), cutParams.end());
    Double_t maxParam = *std::max_element(cutParams.begin(), cutParams.end());
    Int_t    numParam = cutParams.size();

    // Calculate efficiencyGraphs for <target,Q^2> pairs
    for (auto const &t : targets) {
        for (auto const &q : Q2s) {
            // These will be used to construct our TGraphAsymmErrors
            std::vector<Double_t> e;       // efficiency
            std::vector<Double_t> eUp;     // upper error bar
            std::vector<Double_t> eLow;    // lower error bar
            std::vector<Double_t> eZeros;  // zeros to git rid of X error bars

            // Loop over each value of cut threshold and calculate efficiency
            for (auto const &p : cutParams) {
                auto key = std::make_tuple(t,q,p);

                TString efficiencyName = Form("teff_%s_%d_%.2f", t.Data(), q, p);
                efficiencyCalculators[key] = new Efficiency0D(efficiencyName.Data());

                // Set chain
                TString dataKey = Form("%s_Q2_%d", t.Data(), q);
                TChain* chain = data->GetChain(dataKey);
                efficiencyCalculators[key]->SetChain(chain);

                // Set cuts
                efficiencyCalculators[key]->SetShouldCut(cutShould);
                TCut cutDid = Form(cutDidString, p);
                efficiencyCalculators[key]->SetDidCut(cutDid);

                TString status = Form("-------\nSTATUS: %s, Q^2=%d, %s",
                                      t.Data(), q, cutDid.GetTitle());
                std::cout << status << std::endl;

                // Calculate
                efficiencyCalculators[key]->SetEvents(-1);
                efficiencyCalculators[key]->Init();
                efficiencyCalculators[key]->Calculate();

                // Get efficiencies and error, then add to our vectors
                e.push_back(efficiencyCalculators[key]->GetEfficiency());
                eUp.push_back(efficiencyCalculators[key]->GetEfficiencyErrorUp());
                eLow.push_back(efficiencyCalculators[key]->GetEfficiencyErrorLow());
                eZeros.push_back(0);
            }

            // Create graph
            std::pair<TString, Int_t> key = std::make_pair(t,q);

            // &v[0] "convertings" a vector<double> named v into a double*
            efficiencyGraphs[key] = new TGraphAsymmErrors(numParam, &cutParams[0], &e[0], &eZeros[0], &eZeros[0], &eLow[0], &eUp[0]);

            TString title = Form("%s Q^{2}=%d GeV^{2}", t.Data(), q);
            efficiencyGraphs[key]->SetTitle(title);
        }
    }

    // Plot efficiencyGraphs
    std::vector<Int_t> colors = {46, 44, 30, 38};
    TCanvas* cHcer = new TCanvas("cHcer", "HCer Efficiency", 1024, 640);
    cHcer->Print("hcer_plateau.pdf["); // open PDF

    TMultiGraph *mgLH2 = new TMultiGraph("mgLH2", "Efficiency");
    TString t = "LH2";
    Int_t n=0; // color
    TLegend *legLH2= new TLegend(0.1,0.1,0.3,0.3);
    for (auto const &q : Q2s) {
        // Make it pretty
        std::pair<TString, Int_t> key = std::make_pair(t,q);
        efficiencyGraphs[key]->SetMarkerStyle(21);
        efficiencyGraphs[key]->SetMarkerColor(colors[n++]);

        // Add to multigraph
        mgLH2->Add(efficiencyGraphs[key]);

        // Legend
        TString dataKey = Form("%s_Q2_%d", t.Data(), q);
        TString label = Form("%s, Q^2=%.1f", t.Data(), data->GetQ2(dataKey));
        legLH2->AddEntry(efficiencyGraphs[key], label.Data(), "lp");
    }
    mgLH2->Draw("ALP");
    mgLH2->GetXaxis()->SetTitle("H.cer.npeSum cut");
    mgLH2->GetYaxis()->SetTitle("Efficiency");
    legLH2->Draw();
    cHcer->Print("hcer_plateau.pdf"); // write page to PDF

    TMultiGraph *mgC12 = new TMultiGraph("mgC12", "Efficiency");
    t = "C12";
    n=0; // color
    TLegend *legC12= new TLegend(0.1,0.1,0.3,0.3);
    for (auto const &q : Q2s) {
        // Make it pretty
        std::pair<TString, Int_t> key = std::make_pair(t,q);
        efficiencyGraphs[key]->SetMarkerStyle(21);
        efficiencyGraphs[key]->SetMarkerColor(colors[n++]);

        // Add to multigraph
        mgC12->Add(efficiencyGraphs[key]);

        // Legend
        TString dataKey = Form("%s_Q2_%d", t.Data(), q);
        TString label = Form("%s, Q^2=%.1f", t.Data(), data->GetQ2(dataKey));
        legC12->AddEntry(efficiencyGraphs[key], label.Data(), "lp");
    }
    mgC12->Draw("ALP");
    mgC12->GetXaxis()->SetTitle("H.cer.npeSum cut");
    mgC12->GetYaxis()->SetTitle("Efficiency");
    legC12->Draw();
    cHcer->Print("hcer_plateau.pdf"); // write page to PDF

    cHcer->Print("hcer_plateau.pdf]"); // close fPDF
}
