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

// This macro calculates the efficiency of the HMS calorimeter as
// a function of cuts on normalized energy for electron events

void hcal_lower_plateau() {
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // This map takes kinematics name as a key
    std::map<TString, TGraphAsymmErrors*> efficiencyGraphs;

    // This map takes pair<kinematicsname, cut parameter> as a key
    std::map<std::pair<TString, Double_t>, Efficiency0D*> efficiencyCalculators;

    // These are the names in our json specifying kinematics settings
    std::vector<TString> kinematics = {"C12_Q2_8_thin",  "C12_Q2_8_thick",
                                       "C12_Q2_10_thin", "C12_Q2_10_thick"};

    // Set up our cuts; we'll be scanning a cut threshold
    TCut hBetaCut  = cuts->Get("hBetaCut");
    TCut hDeltaCut = cuts->Get("hDeltaCut");
    TCut hCerCut   = cuts->Get("hCerCut");
    TCut cutShould = hBetaCut && hDeltaCut && hCerCut;
    TString cutDidString = "H.cal.etottracknorm > %f && H.cal.etottracknorm < 1.5";
    std::vector<Double_t> cutParams = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

    Double_t minParam = *std::min_element(cutParams.begin(), cutParams.end());
    Double_t maxParam = *std::max_element(cutParams.begin(), cutParams.end());
    Int_t    numParam = cutParams.size();

    // Calculate efficiencyGraphs for each kinematics
    for (auto const &k : kinematics) {
        // These will be used to construct our TGraphAsymmErrors
        std::vector<Double_t> e;       // efficiency
        std::vector<Double_t> eUp;     // upper error bar
        std::vector<Double_t> eLow;    // lower error bar
        std::vector<Double_t> eZeros;  // zeros to git rid of X error bars

        // Loop over each value of cut threshold and calculate efficiency
        for (auto const &p : cutParams) {
            auto key = std::make_pair(k,p);

            TString efficiencyName = Form("teff_%s_%.2f", k.Data(), p);
            efficiencyCalculators[key] = new Efficiency0D(efficiencyName.Data());

            // Set chain
            TChain* chain = data->GetChain(k);
            efficiencyCalculators[key]->SetChain(chain);

            // Set cuts
            efficiencyCalculators[key]->SetShouldCut(cutShould);
            TCut cutDid = Form(cutDidString, p);
            efficiencyCalculators[key]->SetDidCut(cutDid);

            TString status = Form("-------\nSTATUS: %s, %s",
                                  k.Data(), cutDid.GetTitle());
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
        // &v[0] "convertings" a vector<double> named v into a double*
        efficiencyGraphs[k] = new TGraphAsymmErrors(numParam, &cutParams[0], &e[0], &eZeros[0], &eZeros[0], &eLow[0], &eUp[0]);
        efficiencyGraphs[k]->SetTitle(k);
    }

    // Plot efficiencyGraphs
    std::vector<Int_t> colors = {46, 44, 30, 38};
    TCanvas* cHcal = new TCanvas("cHcal", "HCal Efficiency", 1024, 640);
    cHcal->Print("hcal_lower_plateau.pdf["); // open PDF

    TMultiGraph *mg = new TMultiGraph("mg", "Efficiency");
    Int_t n=0; // color
    TLegend *leg= new TLegend(0.1,0.1,0.3,0.3);
    for (auto const &k : kinematics) {
        // Make it pretty
        efficiencyGraphs[k]->SetMarkerStyle(21);
        efficiencyGraphs[k]->SetMarkerColor(colors[n++]);

        // Add to multigraph
        mg->Add(efficiencyGraphs[k]);

        // Legend
        leg->AddEntry(efficiencyGraphs[k], k.Data(), "lp");
    }
    mg->Draw("ALP");
    mg->GetXaxis()->SetTitle("H.cal.etottracknorm cut");
    mg->GetYaxis()->SetTitle("Efficiency");
    leg->Draw();
    cHcal->Print("hcal_lower_plateau.pdf"); // write page to PDF

    cHcal->Print("hcal_lower_plateau.pdf]"); // close fPDF
}
