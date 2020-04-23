#include <TEventList.h>
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
// - shower vs preshower energy
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
    Double_t should;       // [#]
    Double_t did;          // [#]
    Double_t trackingEfficiency;
    Double_t trackingEfficiencyUncertainty;
    TEventList* shouldList;
    TEventList* didList;
};

void carbonboiling();
std::map<Int_t, Run*> load();
std::vector<Int_t> getRunNumbers(std::map<Int_t, Run*> runs);
void plot(std::map<Int_t, Run*> runs, std::vector<Int_t> runNumbers);
TH1F* drawTH1(TTree *T, TString branch, TString histoname, Int_t bins, Double_t lo, Double_t hi, TCut cut);
TH2F* drawTH2(TTree *T, TString xbranchstring, TString ybranchstring, TString histoname,
              Int_t xbins, Double_t xlo, Double_t xhi,
              Int_t ybins, Double_t ylo, Double_t yhi,
              TCut cut);
void calculateTrackingEfficiency(std::map<Int_t, Run*> runs, std::vector<Int_t> runNumbers);

//--------------------------------------------------------------------------
// Cuts to be used below
// Should replace this with something NOT global
TCut betaCut = "P.gtr.beta > 0.6 && P.gtr.beta < 1.4";
TCut deltaCut = "P.gtr.dp > -10 && P.gtr.dp < 12";
TCut hodostartCut = "P.hod.goodstarttime==1";

TCut calCut = "0.2 < P.cal.etottracknorm && P.cal.etottracknorm < 1.2";
TCut cerCut = "P.ngcer.npeSum > 0";
TCut pidCut = calCut && cerCut;

TCut qualityCut = betaCut && hodostartCut && cerCut;

TCut pScinGood = "P.hod.goodscinhit==1";
TCut pGoodBeta = "P.hod.betanotrack > 0.5 && P.hod.betanotrack < 1.4";
TCut pDC1NoLarge = "(P.dc.1x1.nhit + P.dc.1u2.nhit + P.dc.1u1.nhit + P.dc.1v1.nhit + P.dc.1x2.nhit + P.dc.1v2.nhit) < 21";
TCut pDC2NoLarge = "(P.dc.2x1.nhit + P.dc.2u2.nhit + P.dc.2u1.nhit + P.dc.2v1.nhit + P.dc.2x2.nhit + P.dc.2v2.nhit) < 21";
TCut pDCNoLarge = pDC1NoLarge && pDC2NoLarge;
TCut pScinShould = pScinGood && pGoodBeta && pDCNoLarge;
TCut pScinDid    = pScinShould && "P.dc.ntrack>0";

//--------------------------------------------------------------------------
int main() {
    carbonboiling();
    return 0;
}

//--------------------------------------------------------------------------
void carbonboiling() {

    TString pdfFilename;

    std::map<Int_t, Run*> runs = load();
    std::vector<Int_t> runNumbers = getRunNumbers(runs);

    // Generate quality plots
    plot(runs, runNumbers);

    // Tracking efficiency
    calculateTrackingEfficiency(runs, runNumbers);

    std::cout << "run, bcm4aCurrent, trackingEfficiency, uncertainty, did, should" << std::endl;
    for (auto run: runNumbers) {
        std::cout << Form("%d,%f,%f,%f,%d,%d",
                           run,
                           runs[run]->hclogCurrent,
                           runs[run]->trackingEfficiency,
                           runs[run]->trackingEfficiencyUncertainty,
                           int(runs[run]->did),
                           int(runs[run]->should)
                         )
                  << std::endl;
    }
}

//--------------------------------------------------------------------------
std::map<Int_t, Run*> load() {
    TString rootfileFormat = "/Volumes/ssd750/ct/pass5/shms_replay_production_%d_-1.root";

    std::map<Int_t, Run*> runs;

    // I'm only looking at a subset of the full list.
    // These data are from January 2018 right around the time we took
    // our first batch of data.
    std::vector<Int_t> runNumbers = {
        // 2013, 2012, 2011, 2010, 2009,
        2000, 1999, 1998, 1997, 1996, 1995, 1994, 1993, 1992
    };

    // Initialize and load each run
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
    // runs[2013]->hclogCurrent = 65;
    // runs[2012]->hclogCurrent = 55;
    // runs[2011]->hclogCurrent = 55;
    // runs[2010]->hclogCurrent = 65;
    // runs[2009]->hclogCurrent = 55;
    runs[2000]->hclogCurrent = 65;
    runs[1999]->hclogCurrent = 60;
    runs[1998]->hclogCurrent = 50;
    runs[1997]->hclogCurrent = 40;
    runs[1996]->hclogCurrent = 30;
    runs[1995]->hclogCurrent = 20;
    runs[1994]->hclogCurrent = 10;
    runs[1993]->hclogCurrent = 7;
    runs[1992]->hclogCurrent = 2;

    return runs;
}

//--------------------------------------------------------------------------
std::vector<Int_t> getRunNumbers(std::map<Int_t, Run*> runs) {
    std::vector<Int_t> runNumbers;
    for(map<Int_t, Run*>::iterator it = runs.begin(); it != runs.end(); ++it) {
        runNumbers.push_back(it->first);
    }
    return runNumbers;
}

//--------------------------------------------------------------------------
void plot(std::map<Int_t, Run*> runs, std::vector<Int_t> runNumbers) {
    std::cout << "Generating quality plots" << std::endl;

    TFile fWrite("quality_plots.root", "recreate");
    TTree *T, *TSP;
    TH1F *h1;
    TH2F *h2;
    TString histoname;

    for (auto run: runNumbers) {
        std::cout << run << std::endl;

        T = runs[run]->T;
        TSP = runs[run]->TSP;

        // beta
        histoname = Form("run%d_beta", run);
        h1 = drawTH1(T, "P.gtr.beta", histoname, 300, 0, 1.5, hodostartCut);
        h1->Write();

        // delta
        histoname = Form("run%d_delta", run);
        h1 = drawTH1(T, "P.gtr.dp", histoname, 400, -40, 40, qualityCut);
        h1->Write();

        // delta
        histoname = Form("run%d_p", run);
        h1 = drawTH1(T, "P.gtr.p", histoname, 600, 0, 6, qualityCut);
        h1->Write();

        // delta vs beta
        histoname = Form("run%d_delta_vs_beta", run);
        h2 = drawTH2(T, "P.gtr.dp", "P.gtr.beta", histoname, 75, 0, 1.5, 400, -40, 40, hodostartCut);
        h2->Write();

        // xfp and yfp
        histoname = Form("run%d_xfp", run);
        h1 = drawTH1(T, "P.dc.x_fp[0]", histoname, 200, -50, 50, qualityCut);
        h1->Write();
        histoname = Form("run%d_yfp", run);
        h1 = drawTH1(T, "P.dc.y_fp[0]", histoname, 200, -50, 50, qualityCut);
        h1->Write();
        histoname = Form("run%d_xfp_vs_yfp", run);
        h2 = drawTH2(T, "P.dc.x_fp[0]", "P.dc.y_fp[0]", histoname, 200, -50, 50, 200, -50, 50, qualityCut);
        h2->Write();

        // delta vs etottracknorm
        histoname = Form("run%d_delta_vs_etottracknorm", run);
        h2 = drawTH2(T, "P.gtr.dp", "P.cal.etottracknorm", histoname, 75, 0, 1.5, 400, -40, 40, hodostartCut && betaCut && cerCut);
        h2->Write();

        // Check track positions at calorimeter vs energy deposition
        histoname = Form("run%d_cal_xtrack_ytrack", run);
        h2 = drawTH2(T, "P.cal.xtrack", "P.cal.ytrack", histoname, 400, -100, 100, 400, -100, 100, betaCut && cerCut);
        h2->Write();
        histoname = Form("run%d_cal_xtrack_etottracknorm", run);
        h2 = drawTH2(T, "P.cal.xtrack", "P.cal.etottracknorm", histoname, 150, 0, 1.5, 400, -100, 100, betaCut && cerCut);
        h2->Write();
        histoname = Form("run%d_cal_ytrack_etottracknorm", run);
        h2 = drawTH2(T, "P.cal.ytrack", "P.cal.etottracknorm", histoname, 150, 0, 1.5, 400, -100, 100, betaCut && cerCut);
        h2->Write();

        // shower vs preshower energy
        histoname = Form("run%d_preshower_vs_shower", run);
        h2 = drawTH2(T, "P.cal.pr.eplane", "P.cal.fly.earray", histoname, 200, 0, 4, 200, 0, 4, betaCut && cerCut);
        h2->Write();

        // normalized shower vs preshower energy
        histoname = Form("run%d_preshower_vs_shower_norm", run);
        h2 = drawTH2(T, "P.cal.eprtracknorm", "P.cal.etottracknorm-P.cal.eprtracknorm", histoname, 150, 0, 1.5, 150, 0, 1.5, betaCut && cerCut);
        h2->Write();

        // NGC NPE sum vs etottracknorm
        histoname = Form("run%d_NGC_vs_etottracknorm", run);
        h2 = drawTH2(T, "P.ngcer.npeSum", "P.cal.etottracknorm", histoname, 75, 0, 1.5, 80, 0, 40, betaCut && cerCut);
        h2->Write();
        // NGC NPE sum vs etottracknorm
        histoname = Form("run%d_NGC_vs_etottracknorm_deltacut", run);
        h2 = drawTH2(T, "P.ngcer.npeSum", "P.cal.etottracknorm", histoname, 75, 0, 1.5, 80, 0, 40, betaCut && cerCut && deltaCut);
        h2->Write();

        // BCM4A current over time
        histoname = Form("run%d_BCM4A_scalerCurrent", run);
        TSP->Draw(Form("P.BCM4A.scalerCurrent:This->GetReadEntry()>>%s", histoname.Data()));
        h2 = (TH2F*) gDirectory->Get(histoname);
        h2->Write();
    }
    fWrite.Close();
}

//--------------------------------------------------------------------------
TH1F* drawTH1(TTree *T, TString branch, TString histoname, Int_t bins, Double_t lo, Double_t hi, TCut cut) {
    TString drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(), histoname.Data(), bins, lo, hi);
    T->Draw(drawMe.Data(), cut, "goff");
    return (TH1F*) gDirectory->Get(histoname);
}

//--------------------------------------------------------------------------
TH2F* drawTH2(TTree *T, TString xbranchstring, TString ybranchstring, TString histoname,
              Int_t xbins, Double_t xlo, Double_t xhi,
              Int_t ybins, Double_t ylo, Double_t yhi,
              TCut cut) {
    TString drawMe = Form("%s:%s>>%s(%d,%f,%f,%d,%f,%f)",
                           xbranchstring.Data(), ybranchstring.Data(), histoname.Data(),
                           xbins, xlo, xhi, ybins, ylo, yhi);
    T->Draw(drawMe.Data(), cut, "goff");
    return (TH2F*) gDirectory->Get(histoname);
}

//--------------------------------------------------------------------------
void calculateTrackingEfficiency(std::map<Int_t, Run*> runs, std::vector<Int_t> runNumbers) {
    std::cout << "Calculating tracking efficiency" << std::endl;
    TString drawMe, histoname;
    TTree *T;
    for (auto run: runNumbers) {
        T = runs[run]->T;

        // should
        histoname.Form("run%d_should", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, pScinShould);
        runs[run]->shouldList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->should = runs[run]->shouldList->GetN();

        // did
        histoname.Form("run%d_did", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, pScinDid);
        runs[run]->didList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->did = runs[run]->didList->GetN();

        runs[run]->trackingEfficiency = runs[run]->did/runs[run]->should;

        // uncertainty
        Double_t k = runs[run]->did;
        Double_t N = runs[run]->should;
        runs[run]->trackingEfficiencyUncertainty = sqrt(k*(1-k/N))/N; // binomial error

    }
}
