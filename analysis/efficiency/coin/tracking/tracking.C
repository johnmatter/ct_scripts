#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCut.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TGraphAsymmErrors.h>

#include <CTData.h>
#include <CTCuts.h>
#include <Efficiency0D.h>

// This plots tracking efficiency for the SHMS and HMS
// as a function of Q^2 for C12 and LH2 targets
//
// The goal is 4 overlaid TGraphs: one per (spectrometer,target) pair
// Each point will be for a particular (spectrometer, target, Q^2).
// So we need:
//      - one container for 2*2*4 TEfficiency calculations
//      - one container for 2*2   TGraphs
//
void tracking() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::vector<TString>  spectrometers = {"HMS","SHMS"};
    std::vector<TString>  targets       = {"LH2","C12"};
    std::vector<Int_t>    Q2s           = {8, 10, 12, 14}; // rounded for filenames
    std::vector<Double_t> Q2vals        = {8, 9.5, 11.5, 14.3}; // actual values

    // This map takes std::tuple<spect,target,Q^2> as a key
    std::map<std::tuple<TString, TString, Int_t>, Efficiency0D*> efficiencyCalculators;

    // This map takes std::tuple<spect,target> as a key
    std::map<std::tuple<TString, TString>, TGraphAsymmErrors*> efficiencyGraphs;

    // Set up our cuts
    TCut cutShould;
    TCut hCutShould = cuts->Get("hScinShoulde");
    TCut pCutShould = cuts->Get("pScinShouldh");
    TCut cutDid;
    TCut hCutDid    = cuts->Get("hScinDide");
    TCut pCutDid    = cuts->Get("pScinDidh");

    // This will contain all our graphs
    // TMultiGraph *multiGraph = new TMultiGraph("multiGraph", "Tracking Efficiency");

    // Save
    TString csvFilename = "tracking.csv";

    // ------------------------------------------------------------------------
    // Calculate and plot for both spectrometers
    for (auto const &s : spectrometers) {
        // Get this spectrometer's cuts
        if (s=="SHMS") {
            cutShould = pCutShould;
            cutDid = pCutDid;
        }
        if (s=="HMS") {
            cutShould = hCutShould;
            cutDid = hCutDid;
        }

        // Loop over <target,Q2> pairs
        for (auto const &t : targets)   {
            // These will be used to construct our TGraphAsymmErrors
            std::vector<Double_t> e;      // efficiency
            std::vector<Double_t> eUp;    // upper error bar
            std::vector<Double_t> eLow;   // lower error bar
            std::vector<Double_t> eZeros; // zeros to get rid of X error bars

            for (auto const &q : Q2s)   {
                auto key = std::make_tuple(s,t,q);

                // Create efficiency object
                TString efficiencyName = Form("teff_%s_%s_%d", s.Data(), t.Data(), q);
                efficiencyCalculators[key] = new Efficiency0D(efficiencyName.Data());

                // Set chain
                TString dataKey = Form("%s_Q2_%d", t.Data(), q);
                TChain* chain = data->GetChain(dataKey);
                efficiencyCalculators[key]->SetChain(chain);

                // Set cuts
                efficiencyCalculators[key]->SetShouldCut(cutShould);
                efficiencyCalculators[key]->SetDidCut(cutDid);

                TString status = Form("-------\nSTATUS: %s, %s, Q^2=%d",
                        s.Data(), t.Data(), q);
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

                // delete efficiencyCalculators[key];
            }

            // Make key for this graph
            auto key = std::make_tuple(s,t);

            // TGraphAsymmErrors expects double*s as arguments.
            // &v[0] "converts" a vector<double> named v into a double*
            efficiencyGraphs[key] = new TGraphAsymmErrors(4, &Q2vals[0], &e[0],
                                              &eZeros[0], &eZeros[0],
                                              &eLow[0], &eUp[0]);

            TString title = Form("%s %s", s.Data(), t.Data());
            efficiencyGraphs[key]->SetTitle(title);
        }
    }

    // ------------------------------------------------------------------------
    // Print efficiencies for other analyses
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    TString printme = Form("spectrometer,target,Q2,efficiency,efficiencyErrorUp,efficiencyErrorLow");
    ofs << printme << std::endl;

    for (auto const &s : spectrometers) {
        for (auto const &t : targets)   {
            for (auto const &q : Q2s)   {
                TString dataKey = Form("%s_Q2_%d", t.Data(), q);
                Double_t thisQ2 = data->GetQ2(dataKey);

                auto key = std::make_tuple(s,t,q);
                Double_t thisE    = efficiencyCalculators[key]->GetEfficiency();
                Double_t thisEUp  = efficiencyCalculators[key]->GetEfficiencyErrorUp();
                Double_t thisELow = efficiencyCalculators[key]->GetEfficiencyErrorLow();

                TString printme = Form("%s,%s,%f,%f,%f,%f", s.Data(), t.Data(), thisQ2,
                                        thisE, thisEUp, thisELow);
                ofs << printme << std::endl;
            }
        }
    }
    ofs.close();

    // ------------------------------------------------------------------------
    // Format and draw
    // TLegend *legend = new TLegend(0.15, 0.3, 0.35, 0.2);

    // TString target, spect;

    // target="LH2"; spect="SHMS";
    // auto key = std::make_tuple(target, spect);
    // efficiencyGraphs[key]->SetMarkerStyle(26);
    // efficiencyGraphs[key]->SetMarkerColor(38);
    // efficiencyGraphs[key]->SetLineStyle(9);
    // efficiencyGraphs[key]->SetLineColor(38);
    // legend->AddEntry(efficiencyGraphs[key],
    //         Form("%s %s",target.Data(), spect.Data()), "lp");

    // target="LH2"; spect="HMS";
    // key = std::make_tuple(target, spect);
    // efficiencyGraphs[key]->SetMarkerStyle(26);
    // efficiencyGraphs[key]->SetMarkerColor(46);
    // efficiencyGraphs[key]->SetLineStyle(9);
    // efficiencyGraphs[key]->SetLineColor(46);
    // legend->AddEntry(efficiencyGraphs[key],Form("%s %s",
    //             target.Data(), spect.Data()), "lp");

    // target="C12"; spect="SHMS";
    // key = std::make_tuple(target, spect);
    // efficiencyGraphs[key]->SetMarkerStyle(20);
    // efficiencyGraphs[key]->SetMarkerColor(38);
    // efficiencyGraphs[key]->SetLineStyle(1);
    // efficiencyGraphs[key]->SetLineColor(38);
    // legend->AddEntry(efficiencyGraphs[key],
    //         Form("%s %s",target.Data(), spect.Data()), "lp");

    // target="C12"; spect="HMS";
    // key = std::make_tuple(target, spect);
    // efficiencyGraphs[key]->SetMarkerStyle(20);
    // efficiencyGraphs[key]->SetMarkerColor(46);
    // efficiencyGraphs[key]->SetLineStyle(1);
    // efficiencyGraphs[key]->SetLineColor(46);
    // legend->AddEntry(efficiencyGraphs[key],
    //         Form("%s %s",target.Data(), spect.Data()), "lp");

    // TCanvas* cTrackEff = new TCanvas("cTrackEff", "Tracking Efficiency", 640, 640);
    // cTrackEff->Print("tracking.pdf["); // open PDF

    // multiGraph->Draw("LZAP");
    // multiGraph->GetXaxis()->SetTitle("Q^{2} (GeV^{2})");
    // multiGraph->GetYaxis()->SetTitle("Efficiency");
    // multiGraph->SetMinimum(0);
    // multiGraph->SetMaximum(1.02);
    // cTrackEff->Modified();
    // legend->Draw();
    // cTrackEff->Print("tracking.pdf"); // print page

    // cTrackEff->Print("tracking.pdf]"); // close PDF
}
