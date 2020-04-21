#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>

#include <vector>
#include <map>

// This script does the following:
// 1) Produces quality plots to make sure we have some reasonable data
// 2) Calculates yield for each run
// 3) Plots yield vs beam current
//
// We expect tracking efficiency and rate to depend on beam current, but if
// we're correctly estimating tracking efficiency, the charge-normalized yield
// should be independent of beam current.
//
// Per-run quality plots include:
// - beta
// - delta
// - xfp vs yfp
// - beta vs delta
// - etottracknorm vs delta
// - NGC NPE sum vs etottracknorm
// - BCM4A current over time
//
// Analysis plots include:
// - BCM4A average current vs run (to verify the info in HCLOG is current)
// - Trigger rate vs run (another "is our replay reasonable?" check)
// - Charge-normalized yield vs beam current (the final desired result)



struct Run {
    TTree   *T;
    TTree   *TSP;
    TFile   *file;
    Double_t hclogCurrent; // [uA] What did hclog claim?
    Double_t myCurrent;    // [uA] What do I estimate?
    Double_t eventRate;    // [#/s]
    Double_t count;        // [#]
    Double_t charge;       // [uC]
    Double_t yield;        // [#/uC]
    Double_t trackingEfficiency;
};

TH1F* drawTH1(TTree *T, TString branch, TString histoname, Int_t bins, Double_t lo, Double_t hi, TCut cut) {
    TString drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(), histoname.Data(), bins, lo, hi);
    T->Draw(drawMe.Data(), cut, "goff");
    return (TH1F*) gDirectory->Get(histoname);
}

TH2F* drawTH2(TTree *T, TString branchstring, TString histoname,
              Int_t xbins, Double_t xlo, Double_t xhi,
              Int_t ybins, Double_t ylo, Double_t yhi,
              TCut cut) {
    TString drawMe = Form("%s>>%s(%d,%f,%f,%d,%f,%f)", branchstring.Data(), histoname.Data(), xbins, xlo, xhi, ybins, ylo, yhi);
    T->Draw(drawMe.Data(), cut, "goff");
    return (TH2F*) gDirectory->Get(histoname);
}

void carbonboiling() {
    TString pdfFilename;
    TString rootfileFormat = "/Volumes/ssd750/ct/pass2/carbonboiling/shms_replay_production_%d_-1.root";

    std::map<Int_t, TFile*> file;
    std::map<Int_t, TTree*> tree;

    // I'm only looking at a subset of the full list.
    // These data are from January 2018 right around the time we took
    // our first batch of data.
    std::vector<Int_t> runNumbers = {
        2013, 2012, 2011, 2010, 2009,
        2000, 1999, 1998, 1997, 1996, 1995, 1994, 1993, 1992
    };

    // Initialize and load each run
    std::map<Int_t, Run*> runs;
    for (auto run: runNumbers) {
        std::cout << "Load " << run << std::endl;
        runs[run] = new Run;
        runs[run]->file = new TFile(Form(rootfileFormat.Data(), run));
        runs[run]->file->GetObject("T",   runs[run]->T);
        runs[run]->file->GetObject("TSP", runs[run]->TSP);
    }

    // Add currents from the HCLOG comments
    // runs[3225]->hclogCurrent = 35;
    // runs[3224]->hclogCurrent = 50;
    // runs[3223]->hclogCurrent = 60;
    // runs[3222]->hclogCurrent = 65;
    // runs[3114]->hclogCurrent = 60;
    // runs[3113]->hclogCurrent = 50;
    // runs[3112]->hclogCurrent = 25;
    // runs[3111]->hclogCurrent = 10;
    // runs[3110]->hclogCurrent = 5;
    // runs[3109]->hclogCurrent = 2.5;
    runs[2013]->hclogCurrent = 65;
    runs[2012]->hclogCurrent = 55;
    runs[2011]->hclogCurrent = 55;
    runs[2010]->hclogCurrent = 65;
    runs[2009]->hclogCurrent = 55;
    runs[2000]->hclogCurrent = 65;
    runs[1999]->hclogCurrent = 60;
    runs[1998]->hclogCurrent = 50;
    runs[1997]->hclogCurrent = 40;
    runs[1996]->hclogCurrent = 30;
    runs[1995]->hclogCurrent = 20;
    runs[1994]->hclogCurrent = 10;
    runs[1993]->hclogCurrent = 7;
    runs[1992]->hclogCurrent = 2;

    // Generate quality plots
    TFile fWrite("carbon_boiling_quality_plots.root", "recreate");
    TTree *T, *TSP;
    TH1F *h1;
    TH2F *h2;
    TString histoname;
    for (auto run: runNumbers) {
        T = runs[run]->T;
        TSP = runs[run]->TSP;

        // beta
        histoname = Form("run%d_beta", run);
        h1 = drawTH1(T, "P.gtr.beta", histoname, 150, 0, 1.5, "");
        h1->Write();

        // delta
        histoname = Form("run%d_delta", run);
        h1 = drawTH1(T, "P.gtr.dp", histoname, 640, -10, 22, "");
        h1->Write();

        // beta vs delta
        histoname = Form("run%d_beta_delta", run);
        h2 = drawTH1(T, "P.gtr.beta:P.gtr.delta", histoname, 150, 0, 1.5, 640, -10, 22, "");
        h2->Write();

        // xfp vs yfp
        histoname = Form("run%d_xfp_yfp", run);
        h2 = drawTH1(T, "P.dc.x[0]:P.dc.y[0]", histoname, 200, -50, 50, 200, -50, 50, "");
        h2->Write();

        // etottracknorm vs delta
        histoname = Form("run%d_etottracknorm_delta", run);
        h2 = drawTH1(T, "P.cal.etottracknorm:P.gtr.dp", histoname, 640, -10, 22, 150, 0, 1.5, "");
        h2->Write();

        // NGC NPE sum vs etottracknorm

        // BCM4A current over time

    }

    fWrite.Close();
}

int main() {
    carbonboiling();
    return 0;
}
