#include <TEventList.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TCut.h>
#include <TH1F.h>
#include <TH2F.h>

#include <iostream>
#include <fstream>
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
    TString target;
    Double_t hclogCurrent; // [uA] What did hclog claim?
    Double_t myCurrent;    // [uA] What do I estimate?
    Double_t ps1Rate;      // [#/s]
    Double_t ps2Rate;      // [#/s]
    Double_t goodEvents;   // [#]
    Double_t charge;       // [uC]
    Double_t scalerChargeCut; // [uC]
    Double_t scalerCharge;    // [uC]
    Double_t time;         // [s]
    Double_t yield;        // [#/uC]
    Double_t trackShould;  // [#]
    Double_t trackDid;     // [#]
    Double_t pshShould;    // [#]
    Double_t pshDid;       // [#]
    Double_t calShould;    // [#]
    Double_t calDid;       // [#]
    Double_t cerShould;    // [#]
    Double_t cerDid;       // [#]
    Int_t ps1;
    Int_t ps2;
    Double_t trackingEfficiency;
    Double_t trackingEfficiencyUncertainty;
    Double_t cerEfficiency;
    Double_t cerEfficiencyUncertainty;
    Double_t calEfficiency;
    Double_t calEfficiencyUncertainty;
    Double_t pshEfficiency;
    Double_t pshEfficiencyUncertainty;
    Double_t clta;
    Double_t cltaUncertainty;
    Double_t physScalerCount;
    Double_t physTriggerCount;
    Double_t lte;
    Double_t lteUncertainty;
    Double_t edtmScalerCount;
    Double_t edtmTriggerCount;
    TEventList* trackShouldList;
    TEventList* trackDidList;
    TEventList* pshShouldList;
    TEventList* pshDidList;
    TEventList* calShouldList;
    TEventList* calDidList;
    TEventList* cerShouldList;
    TEventList* cerDidList;
    TEventList* goodList;
};

// Branches needed for yield and livetime
TString bcmScalerChargeBranch = "P.BCM4A.scalerCharge";
TString bcmScalerChargeCutBranch = "P.BCM4A.scalerChargeCut";
TString bcmScalerCurrentBranch = "P.BCM4A.scalerCurrent";
TString oneMHzScalerBranch = "P.1MHz.scalerTime";
TString ps1RateScalerBranch = "P.pTRIG1.scalerRate";
TString ps2RateScalerBranch = "P.pTRIG2.scalerRate";
TString pTRIG1ScalerBranch = "P.pTRIG1.scaler";
TString pTRIG2ScalerBranch = "P.pTRIG2.scaler";
TString pTRIG1TDCBranch = "T.shms.pTRIG1_tdcTimeRaw";
TString pTRIG2TDCBranch = "T.shms.pTRIG2_tdcTimeRaw";
TString edtmScalerBranch = "P.EDTM.scaler";
TString edtmTDCBranch = "T.shms.pEDTM_tdcTimeRaw";
TString bcmBranch = "P.bcm.bcm4a.AvgCurrent";

// "Beam on" condition:
// How many uA must we be within the nominal current to count events?
Double_t bcmCurrentCutDelta = 3.0;

// Functions called in main()
void targetboiling();
std::map<Int_t, Run*> load();
std::vector<Int_t> getRunNumbers(std::map<Int_t, Run*> runs);
void plot(std::map<Int_t, Run*> runs);
TH1F* drawTH1(TTree *T, TString branch, TString histoname, Int_t bins, Double_t lo, Double_t hi, TCut cut);
TH2F* drawTH2(TTree *T, TString xbranchstring, TString ybranchstring, TString histoname,
              Int_t xbins, Double_t xlo, Double_t xhi,
              Int_t ybins, Double_t ylo, Double_t yhi,
              TCut cut);
void calculateScalers(std::map<Int_t, Run*> runs);
void calculateLivetime(std::map<Int_t, Run*> runs);
void calculateTrackingEfficiency(std::map<Int_t, Run*> runs);
void calculatePIDEfficiency(std::map<Int_t, Run*> runs);
void calculateYield(std::map<Int_t, Run*> runs);
void print(std::map<Int_t, Run*> runs);
Double_t getPrescaleFactor(Int_t ps);
Double_t correctForPrescale(Double_t rate, Int_t ps);

//--------------------------------------------------------------------------
// Cuts to be used below
// Should replace this with something NOT global
TCut insideDipoleExit = "P.dc.InsideDipoleExit==1";
TCut betaCut = "P.gtr.beta > 0.6 && P.gtr.beta < 1.4";
TCut deltaCut = "P.gtr.dp > -10 && P.gtr.dp < 12";
TCut hodostartCut = "P.hod.goodstarttime==1";

TCut calCut = "P.cal.etottracknorm > 0.7 && P.cal.eprtracknorm > 0.035";
TCut cerCut = "P.ngcer.npeSum > 5.0";
TCut pidCut = calCut && cerCut;

TCut qualityCut = betaCut && hodostartCut && cerCut && calCut && insideDipoleExit;

TCut pScinGood = "P.hod.goodscinhit==1";
TCut pGoodBeta = " P.hod.betanotrack < 1.4";
TCut pDC1NoLarge = "(P.dc.1x1.nhit + P.dc.1u2.nhit + P.dc.1u1.nhit + P.dc.1v1.nhit + P.dc.1x2.nhit + P.dc.1v2.nhit) < 25";
TCut pDC2NoLarge = "(P.dc.2x1.nhit + P.dc.2u2.nhit + P.dc.2u1.nhit + P.dc.2v1.nhit + P.dc.2x2.nhit + P.dc.2v2.nhit) < 25";
TCut pDCNoLarge = pDC1NoLarge && pDC2NoLarge;
TCut pScinShould = pScinGood && pGoodBeta && pDCNoLarge;
TCut pScinDid    = pScinShould && "P.dc.ntrack>0";


// Deepak's tracking cuts
TCut DeepakBetaCut = "P.gtr.beta < 1.4 && P.gtr.beta > 0.8";
TCut DeepakCerCut = "P.ngcer.npeSum > 5.0";
TCut DeepakCalCut = "P.cal.eprtracknorm > 0.035 && P.cal.etottracknorm > 0.7";
TCut tightAcceptanceCut = "P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp < 20";
TCut DeepakCalShould = "P.gtr.beta < 1.4 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.beta > 0.8 && P.ngcer.npeSum > 5.0 && P.cal.eprtracknorm > 0.035 && P.dc.InsideDipoleExit==1 && P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp > -25 && P.dc.x_fp < 20 && P.hod.goodscinhit==1 ";
TCut DeepakCalDid    = "P.gtr.beta < 1.4 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.beta > 0.8 && P.ngcer.npeSum > 5.0 && P.cal.eprtracknorm > 0.035 && P.dc.InsideDipoleExit==1 && P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp > -25 && P.dc.x_fp < 20 && P.hod.goodscinhit==1  && P.cal.etottracknorm > 0.7";
TCut DeepakPshShould = "P.gtr.beta < 1.4 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.beta > 0.8 && P.cal.etottracknorm > 0.85 && P.dc.InsideDipoleExit==1 && P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp > -25 && P.dc.x_fp < 20 && P.ngcer.npeSum > 5.0 && P.hod.goodscinhit==1 ";
TCut DeepakPshDid    = "P.gtr.beta < 1.4 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.beta > 0.8 && P.cal.etottracknorm > 0.85 && P.dc.InsideDipoleExit==1 && P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp > -25 && P.dc.x_fp < 20 && P.ngcer.npeSum > 5.0 && P.hod.goodscinhit==1 && P.cal.eprtracknorm > 0.035";
TCut DeepakCerShould = "P.gtr.beta < 1.4 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.beta > 0.8 && P.cal.etottracknorm > 0.85 && P.cal.eprtracknorm > 0.035 && P.dc.InsideDipoleExit==1 && P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp > -25 && P.dc.x_fp < 20 && P.hod.goodscinhit==1 ";
TCut DeepakCerDid    = "P.gtr.beta < 1.4 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.beta > 0.8 && P.cal.etottracknorm > 0.85 && P.cal.eprtracknorm > 0.035 && P.dc.InsideDipoleExit==1 && P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp > -25 && P.dc.x_fp < 20 && P.hod.goodscinhit==1  && P.ngcer.npeSum > 2.0";

TCut calShouldCut = DeepakCalShould;
TCut calDidCut    = DeepakCalDid;
TCut pshShouldCut = DeepakPshShould;
TCut pshDidCut    = DeepakPshDid;
TCut cerShouldCut = DeepakCerShould;
TCut cerDidCut    = DeepakCerDid;

// TCut calShouldCut = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && cerCut;
// TCut calDidCut    = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && cerCut && calCut;
// TCut cerShouldCut = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && calCut;
// TCut cerDidCut    = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && calCut && cerCut;

TCut DeepakTrackCer = "P.ngcer.npeSum > 5.0";
TCut DeepakTrackCal = "P.cal.etotnorm > 0.7";
TCut DeepakBetanotrack = "P.hod.betanotrack < 1.4";

TCut DeepakTrackShould = DeepakTrackCer && DeepakTrackCal && DeepakBetanotrack && pDC1NoLarge && pDC2NoLarge && pScinGood && hodostartCut && insideDipoleExit;

TCut DeepakDelta4 = "-20 < P.gtr.dp && P.gtr.dp < 20";
TCut DeepakDelta5 = "-15 < P.gtr.dp && P.gtr.dp < 15";
TCut DeepakDelta7 = "-15 < P.gtr.dp && P.gtr.dp < 15";
TCut DeepakY = "-5 < P.gtr.y && P.gtr.y < 5";
TCut DeepakAngle = "-0.2 < P.gtr.th && P.gtr.th < 0.2 && -0.2 < P.gtr.ph && P.gtr.ph< 0.2 ";
TCut fewNegADChits = "P.hod.1x.totNumGoodNegAdcHits<5 && P.hod.1y.totNumGoodNegAdcHits<5 && P.hod.2x.totNumGoodNegAdcHits<5 && P.hod.2y.totNumGoodNegAdcHits<5";
TCut goodFpTime = "-10 < P.hod.1x.fptime && P.hod.1x.fptime < 50 && -10 < P.hod.1y.fptime && P.hod.1y.fptime < 50 && -10 < P.hod.2x.fptime && P.hod.2x.fptime < 50 && -10 < P.hod.2y.fptime && P.hod.2y.fptime < 50";

TCut oneTrack = "P.dc.ntrack==1";
TCut multiTrack = "P.dc.ntrack>1";

TCut DeepakTrackDid4 = DeepakTrackShould && ( (oneTrack && DeepakDelta4) || (multiTrack && DeepakDelta4 && DeepakY) );
TCut DeepakTrackDid5 = DeepakTrackShould && ( (oneTrack && DeepakDelta5) || (multiTrack && DeepakDelta5 && DeepakY) );
TCut DeepakTrackDid7 = DeepakTrackShould && ( (oneTrack                ) || (multiTrack && DeepakDelta7 && DeepakY && DeepakAngle && goodFpTime && fewNegADChits) );

// Which tracking cuts should we use?
// TODO: this should be a command line argument instead
TCut trackShouldCut = DeepakTrackShould;
TCut trackDidCut = DeepakTrackDid7;

TCut goodCut = betaCut && deltaCut && hodostartCut && pidCut && insideDipoleExit;

//--------------------------------------------------------------------------
int main() {
    targetboiling();
    return 0;
}

//--------------------------------------------------------------------------
void targetboiling() {

    TString pdfFilename;

    std::map<Int_t, Run*> runs = load();

    // Scalers
    calculateScalers(runs);

    // // Generate quality plots
    // plot(runs);

    // Livetime
    calculateLivetime(runs);

    // Tracking efficiency
    calculateTrackingEfficiency(runs);

    // PID efficiency
    calculatePIDEfficiency(runs);

    // Yield
    calculateYield(runs);

    // Print
    print(runs);

}

//--------------------------------------------------------------------------
std::map<Int_t, Run*> load() {
    std::cout << "Load" << std::endl;

    // TString rootfileFormat = "/volatile/hallc/comm2017/e1206107/ROOTfiles/lumi_scan/lumi_scan/edtm/shms_replay_production_all_%d_-1.root";
    TString rootfileFormat = "/home/jmatter/ROOTfiles/pass5/shms_replay_production_all_%d_1000000.root";

    std::map<Int_t, Run*> runs;

    // I'm only looking at a subset of the full list.
    // These data are from January 2018 right around the time we took
    // our first batch of data.
    std::vector<Int_t> runNumbers = {
        3225, 3224, 3223, 3222,
        3114, 3113, 3112, 3111, 3110, 3109,
        // 2013, 2012, 2010,
        2000, 1999, 1998, 1997, 1996, 1995, // 1994, 1993, 1992,
        3215, 3214, 3213, 3212, 3211, 3210, 3207, 3206
    };

    // Initialize and load each run
    for (auto run: runNumbers) {
        runs[run] = new Run;
        runs[run]->file = new TFile(Form(rootfileFormat.Data(), run));
        runs[run]->file->GetObject("T",   runs[run]->T);
        runs[run]->file->GetObject("TSP", runs[run]->TSP);
    }

    // Add targets from the HCLOG comments
    runs[3225]->target = "C12";
    runs[3224]->target = "C12";
    runs[3223]->target = "C12";
    runs[3222]->target = "C12";
    runs[3114]->target = "C12";
    runs[3113]->target = "C12";
    runs[3112]->target = "C12";
    runs[3111]->target = "C12";
    runs[3110]->target = "C12";
    runs[3109]->target = "C12";
    // runs[2013]->target = "C12";
    // runs[2012]->target = "C12";
    // runs[2010]->target = "C12";
    runs[2000]->target = "C12";
    runs[1999]->target = "C12";
    runs[1998]->target = "C12";
    runs[1997]->target = "C12";
    runs[1996]->target = "C12";
    runs[1995]->target = "C12";
    // runs[1994]->target = "C12";
    // runs[1993]->target = "C12";
    // runs[1992]->target = "C12";
    runs[3206]->target = "LH2";
    runs[3207]->target = "LH2";
    runs[3210]->target = "LH2";
    runs[3211]->target = "LH2";
    runs[3212]->target = "LH2";
    runs[3213]->target = "LH2";
    runs[3214]->target = "LH2";
    runs[3215]->target = "LH2";

    // Add currents from the HCLOG comments
    runs[3225]->hclogCurrent = 35;
    runs[3224]->hclogCurrent = 50;
    runs[3223]->hclogCurrent = 60;
    runs[3222]->hclogCurrent = 60;
    runs[3114]->hclogCurrent = 60;
    runs[3113]->hclogCurrent = 50;
    runs[3112]->hclogCurrent = 25;
    runs[3111]->hclogCurrent = 10;
    runs[3110]->hclogCurrent = 5;
    runs[3109]->hclogCurrent = 2.5;
    // runs[2013]->hclogCurrent = 65;
    // runs[2012]->hclogCurrent = 55;
    // runs[2010]->hclogCurrent = 65;
    runs[2000]->hclogCurrent = 65;
    runs[1999]->hclogCurrent = 60;
    runs[1998]->hclogCurrent = 50;
    runs[1997]->hclogCurrent = 40;
    runs[1996]->hclogCurrent = 30;
    runs[1995]->hclogCurrent = 20;
    // runs[1994]->hclogCurrent = 10;
    // runs[1993]->hclogCurrent = 7;
    // runs[1992]->hclogCurrent = 2;
    runs[3206]->hclogCurrent = 75;
    runs[3207]->hclogCurrent = 70;
    runs[3210]->hclogCurrent = 10;
    runs[3211]->hclogCurrent = 20;
    runs[3212]->hclogCurrent = 35;
    runs[3213]->hclogCurrent = 35;
    runs[3214]->hclogCurrent = 45;
    runs[3215]->hclogCurrent = 55;

    // PS1 from HCLOG
    runs[3225]->ps1 = -1;
    runs[3224]->ps1 = -1;
    runs[3223]->ps1 = -1;
    runs[3222]->ps1 = -1;
    runs[3114]->ps1 = -1;
    runs[3113]->ps1 = -1;
    runs[3112]->ps1 = -1;
    runs[3111]->ps1 = -1;
    runs[3110]->ps1 = -1;
    runs[3109]->ps1 = -1;
    // runs[2013]->ps1 = 6;
    // runs[2012]->ps1 = 5;
    // runs[2010]->ps1 = 0;
    runs[2000]->ps1 = 6;
    runs[1999]->ps1 = 5;
    runs[1998]->ps1 = 5;
    runs[1997]->ps1 = 5;
    runs[1996]->ps1 = 4;
    runs[1995]->ps1 = 4;
    // runs[1994]->ps1 = 2;
    // runs[1993]->ps1 = 1;
    // runs[1992]->ps1 = 0;
    runs[3206]->ps1 = -1;
    runs[3207]->ps1 = -1;
    runs[3210]->ps1 = -1;
    runs[3211]->ps1 = -1;
    runs[3212]->ps1 = -1;
    runs[3213]->ps1 = -1;
    runs[3214]->ps1 = -1;
    runs[3215]->ps1 = -1;


    // PS2 from HCLOG
    runs[3225]->ps2 = 0;
    runs[3224]->ps2 = 0;
    runs[3223]->ps2 = 0;
    runs[3222]->ps2 = 0;
    runs[3114]->ps2 = 7;
    runs[3113]->ps2 = 6;
    runs[3112]->ps2 = 5;
    runs[3111]->ps2 = 4;
    runs[3110]->ps2 = 2;
    runs[3109]->ps2 = 1;
    // runs[2013]->ps2 = -1;
    // runs[2012]->ps2 = -1;
    // runs[2010]->ps2 = -1;
    runs[2000]->ps2 = -1;
    runs[1999]->ps2 = -1;
    runs[1998]->ps2 = -1;
    runs[1997]->ps2 = -1;
    runs[1996]->ps2 = -1;
    runs[1995]->ps2 = -1;
    // runs[1994]->ps2 = -1;
    // runs[1993]->ps2 = -1;
    // runs[1992]->ps2 = -1;
    runs[3206]->ps2 = 0;
    runs[3207]->ps2 = 0;
    runs[3210]->ps2 = 0;
    runs[3211]->ps2 = 0;
    runs[3212]->ps2 = 0;
    runs[3213]->ps2 = 0;
    runs[3214]->ps2 = 0;
    runs[3215]->ps2 = 0;

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
void plot(std::map<Int_t, Run*> runs) {
    std::cout << "Generating quality plots" << std::endl;

    std::vector<Int_t> runNumbers = getRunNumbers(runs);

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
        h2 = drawTH2(T, "P.ngcer.npeSum", "P.cal.etottracknorm", histoname, 75, 0, 1.5, 80, 0, 40, betaCut);
        h2->Write();
        // NGC NPE sum vs etottracknorm
        histoname = Form("run%d_NGC_vs_etottracknorm_deltacut", run);
        h2 = drawTH2(T, "P.ngcer.npeSum", "P.cal.etottracknorm", histoname, 75, 0, 1.5, 80, 0, 40, betaCut && deltaCut);
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
void calculateLivetime(std::map<Int_t, Run*> runs) {
    std::cout << "Calculating livetime" << std::endl;

    std::vector<Int_t> runNumbers = getRunNumbers(runs);

    TString drawMe, histoname;
    TTree *T;
    TCut bcmCut, tdcCut;
    TString trigTDCBranch;
    Int_t ps;
    for (auto run: runNumbers) {
        std::cout << run << std::endl;
        T = runs[run]->T;
        bcmCut = Form("abs(%s-%f) <= %f", bcmBranch.Data(), runs[run]->hclogCurrent, bcmCurrentCutDelta);
        if (runs[run]->hclogCurrent<bcmCurrentCutDelta) {
            bcmCut = Form("abs(%s-%f) <= %f", bcmBranch.Data(), runs[run]->hclogCurrent, 1.0);
        }

        ps = -1;
        if (run>=3109 && run<=3225) {
            ps = runs[run]->ps2;
            trigTDCBranch = pTRIG2TDCBranch;
        }
        if (run>=1992 && run<=2013) {
            ps = runs[run]->ps1;
            trigTDCBranch = pTRIG1TDCBranch;
        }

        // ------------------------------------------
        // Calculate CLT_A
        tdcCut = Form("%s > 0", trigTDCBranch.Data());

        histoname.Form("run%d_physTdcNonZero", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe.Data(), bcmCut && tdcCut);
        runs[run]->physTriggerCount = ((TEventList*)gDirectory->Get(histoname.Data()))->GetN();
        // runs[run]->physTriggerCount = correctForPrescale(runs[run]->physTriggerCount, ps);

        runs[run]->clta = correctForPrescale(runs[run]->physTriggerCount, ps) / (runs[run]->physScalerCount);
        runs[run]->cltaUncertainty = runs[run]->clta * sqrt(1/correctForPrescale(runs[run]->physTriggerCount, ps) + 1/(runs[run]->physScalerCount));

        // ------------------------------------------
        // Calculate LT_E
        tdcCut = Form("%s > 0", edtmTDCBranch.Data());

        histoname.Form("run%d_edtmTdcNonZero", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe.Data(), bcmCut && tdcCut);
        runs[run]->edtmTriggerCount = ((TEventList*)gDirectory->Get(histoname.Data()))->GetN();
        // runs[run]->edtmTriggerCount = correctForPrescale(runs[run]->edtmTriggerCount, ps);

        runs[run]->lte = correctForPrescale(runs[run]->edtmTriggerCount, ps) / (runs[run]->edtmScalerCount);
        runs[run]->lteUncertainty = runs[run]->lte * sqrt(1/correctForPrescale(runs[run]->edtmTriggerCount, ps) + 1/(runs[run]->edtmScalerCount));
    }
}

//--------------------------------------------------------------------------
void calculateTrackingEfficiency(std::map<Int_t, Run*> runs) {
    std::cout << "Calculating tracking efficiency" << std::endl;

    std::vector<Int_t> runNumbers = getRunNumbers(runs);

    TString drawMe, histoname;
    TTree *T;
    for (auto run: runNumbers) {
        T = runs[run]->T;

        // should
        histoname.Form("run%d_trackShould", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, trackShouldCut);
        runs[run]->trackShouldList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->trackShould = runs[run]->trackShouldList->GetN();

        // did
        histoname.Form("run%d_trackDid", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, trackDidCut);
        runs[run]->trackDidList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->trackDid = runs[run]->trackDidList->GetN();

        runs[run]->trackingEfficiency = runs[run]->trackDid/runs[run]->trackShould;

        // uncertainty
        Double_t k = runs[run]->trackDid;
        Double_t N = runs[run]->trackShould;
        runs[run]->trackingEfficiencyUncertainty = sqrt(k*(1-k/N))/N; // binomial error

    }
}

//--------------------------------------------------------------------------
void calculatePIDEfficiency(std::map<Int_t, Run*> runs) {
    std::cout << "Calculating PID efficiency" << std::endl;

    std::vector<Int_t> runNumbers = getRunNumbers(runs);

    TString drawMe, histoname;
    TTree *T;
    for (auto run: runNumbers) {
        T = runs[run]->T;

        //------------------------------
        // NGCER
        // should
        histoname.Form("run%d_cerShould", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, cerShouldCut);
        runs[run]->cerShouldList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->cerShould = runs[run]->cerShouldList->GetN();

        // did
        histoname.Form("run%d_cerDid", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, cerDidCut);
        runs[run]->cerDidList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->cerDid = runs[run]->cerDidList->GetN();

        //------------------------------
        // Calorimeter
        // should
        histoname.Form("run%d_calShould", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, calShouldCut);
        runs[run]->calShouldList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->calShould = runs[run]->calShouldList->GetN();

        // did
        histoname.Form("run%d_calDid", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, calDidCut);
        runs[run]->calDidList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->calDid = runs[run]->calDidList->GetN();

        //------------------------------
        // Preshower
        // should
        histoname.Form("run%d_pshShould", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, pshShouldCut);
        runs[run]->pshShouldList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->pshShould = runs[run]->pshShouldList->GetN();

        // did
        histoname.Form("run%d_pshDid", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, pshDidCut);
        runs[run]->pshDidList = (TEventList*) gDirectory->Get(histoname.Data());
        runs[run]->pshDid = runs[run]->pshDidList->GetN();

        //------------------------------
        // efficiency
        runs[run]->cerEfficiency = (runs[run]->cerDid/runs[run]->cerShould);
        runs[run]->calEfficiency = (runs[run]->calDid/runs[run]->calShould);
        runs[run]->pshEfficiency = (runs[run]->pshDid/runs[run]->pshShould);

        // uncertainty (binomial error)
        Double_t k1 = runs[run]->cerDid;
        Double_t N1 = runs[run]->cerShould;
        Double_t k2 = runs[run]->calDid;
        Double_t N2 = runs[run]->calShould;
        Double_t k3 = runs[run]->pshDid;
        Double_t N3 = runs[run]->pshShould;

        runs[run]->cerEfficiencyUncertainty = sqrt(k1*(1-k1/N1))/N1;
        runs[run]->calEfficiencyUncertainty = sqrt(k2*(1-k2/N2))/N2;
        runs[run]->pshEfficiencyUncertainty = sqrt(k2*(1-k2/N2))/N2;

    }
}

//--------------------------------------------------------------------------
void calculateScalers(std::map<Int_t, Run*> runs) {
    std::cout << "Calculating scalers" << std::endl;

    std::vector<Int_t> runNumbers = getRunNumbers(runs);

    TTree *T;
    TString physScalerBranch;
    Double_t scalerCharge, scalerChargeCut, scalerCurrent, scalerTime, ps1Rate, ps2Rate, physScalerCount, edtmScalerCount;
    for (auto run: runNumbers) {

        if (runs[run]->ps2 > -1) {
            physScalerBranch = pTRIG2ScalerBranch;
        } else {
            if (runs[run]->ps1 > -1) {
                physScalerBranch = pTRIG1ScalerBranch;
            } else {
                std::cout << "!!!!!!!!!! I DONT KNOW WHICH TRIGGER TO USE !!!!!!!!" << std::endl;
            }
        }

        T = runs[run]->TSP;
        T->SetBranchAddress(bcmScalerChargeBranch.Data(),  &scalerCharge);
        T->SetBranchAddress(bcmScalerChargeCutBranch.Data(),  &scalerChargeCut);
        T->SetBranchAddress(bcmScalerCurrentBranch.Data(), &scalerCurrent);
        T->SetBranchAddress(oneMHzScalerBranch.Data(),     &scalerTime);
        T->SetBranchAddress(ps1RateScalerBranch.Data(),    &ps1Rate);
        T->SetBranchAddress(ps2RateScalerBranch.Data(),    &ps2Rate);
        T->SetBranchAddress(physScalerBranch.Data(),       &physScalerCount);
        T->SetBranchAddress(edtmScalerBranch.Data(),       &edtmScalerCount);

        Double_t averagePS1=0;
        Double_t averagePS2=0;
        Double_t averageCurrent=0;
        Double_t totalScalerTime=0;
        Double_t totalScalerCharge=0;
        Double_t totalPhysScalerCounts=0;
        Double_t totalEDTMScalerCounts=0;

        Double_t lastReadPS1=0;
        Double_t lastReadPS2=0;
        Double_t lastReadCurrent=0;
        Double_t lastReadScalerTime=0;
        Double_t lastReadScalerCharge=0;
        Double_t lastReadPhysScalerCounts=0;
        Double_t lastReadEDTMScalerCounts=0;

        Int_t n, beamOnN;
        beamOnN = 0;
        for (n=0; n<T->GetEntries(); n++) {
            T->GetEntry(n);

            if (abs(scalerCurrent-runs[run]->hclogCurrent) <= bcmCurrentCutDelta) {
                beamOnN++;
                averagePS1 += ps1Rate;
                averagePS2 += ps2Rate;
                averageCurrent += scalerCurrent;
                totalScalerTime += (scalerTime-lastReadScalerTime);
                totalScalerCharge += (scalerCharge-lastReadScalerCharge);
                totalPhysScalerCounts += (physScalerCount-lastReadPhysScalerCounts);
                totalEDTMScalerCounts += (edtmScalerCount-lastReadEDTMScalerCounts);
            }

            lastReadScalerTime = scalerTime;
            lastReadScalerCharge = scalerCharge;
            lastReadPhysScalerCounts = physScalerCount;
            lastReadEDTMScalerCounts = edtmScalerCount;

        }

        averagePS1 /= beamOnN;
        averagePS2 /= beamOnN;
        averageCurrent /= beamOnN;

        runs[run]->ps1Rate = averagePS1;
        runs[run]->ps2Rate = averagePS2;
        runs[run]->myCurrent = averageCurrent;
        runs[run]->charge = totalScalerCharge;
        runs[run]->scalerCharge = scalerCharge;
        runs[run]->scalerChargeCut = scalerChargeCut;
        runs[run]->time = totalScalerTime;
        runs[run]->physScalerCount = totalPhysScalerCounts;
        runs[run]->edtmScalerCount = totalEDTMScalerCounts;
    }
}

//--------------------------------------------------------------------------
void calculateYield(std::map<Int_t, Run*> runs) {
    std::cout << "Calculating yield" << std::endl;

    std::vector<Int_t> runNumbers = getRunNumbers(runs);

    TString drawMe, histoname;
    TTree *T;
    Int_t ps;
    for (auto run: runNumbers) {
        T = runs[run]->T;
        histoname.Form("run%d_good", run);
        drawMe.Form(">>%s", histoname.Data());
        T->Draw(drawMe, goodCut);
        runs[run]->goodList = (TEventList*) gDirectory->Get(histoname.Data());

        ps = -1;
        if (run>=3109 && run<=3225) {
            ps = runs[run]->ps2;
        }
        if (run>=1992 && run<=2013) {
            ps = runs[run]->ps1;
        }

        runs[run]->goodEvents = correctForPrescale(runs[run]->goodList->GetN(), ps);

        runs[run]->yield = runs[run]->goodEvents / runs[run]->charge;
        runs[run]->yield /= runs[run]->clta;
        runs[run]->yield /= runs[run]->trackingEfficiency;
        runs[run]->yield /= runs[run]->calEfficiency;
        runs[run]->yield /= runs[run]->pshEfficiency;
        runs[run]->yield /= runs[run]->cerEfficiency;
    }

}

//--------------------------------------------------------------------------
void print(std::map<Int_t, Run*> runs) {
    std::vector<Int_t> runNumbers = getRunNumbers(runs);

    // Open file
    std::ofstream ofs;
    ofs.open("targetboiling.csv");

    ofs << "run, target, hclogCurrent, scalerCurrent, myCharge, scalerCharge, scalerChargeCut, scalerTime,"
        << "clta, cltaUncertainty, physScalerCount, physTriggerCount,"
        << "lte, lteUncertainty, edtmScalerCount, edtmTriggerCount,"
        << "trackingEfficiency, trackingEfficiencyUncertainty, trackDid, trackShould,"
        << "cerEfficiency, cerEfficiencyUncertainty, cerDid, cerShould,"
        << "calEfficiency, calEfficiencyUncertainty, calDid, calShould,"
        << "pshEfficiency, pshEfficiencyUncertainty, pshDid, pshShould,"
        << "goodEvents, uncorrectedYield, correctedYield,"
        << "ps1Rate, ps2Rate, ps1, ps2, ps1Factor, ps2Factor"
        << std::endl;
    for (auto run: runNumbers) {
        ofs << Form("%d,%s,%f,%f,%f,%f,%f,%f,"
                    "%f,%f,%d,%d,"
                    "%f,%f,%d,%d,"
                    "%f,%f,%d,%d,"
                    "%f,%f,%d,%d,"
                    "%f,%f,%d,%d,"
                    "%f,%f,%d,%d,"
                    "%d,%f,%f,"
                    "%f,%f,%d,%d,%d,%d",
                    run,
                    runs[run]->target.Data(),
                    runs[run]->hclogCurrent,
                    runs[run]->myCurrent,
                    runs[run]->charge,
                    runs[run]->scalerCharge,
                    runs[run]->scalerChargeCut,
                    runs[run]->time,
                    runs[run]->clta,
                    runs[run]->cltaUncertainty,
                    int(runs[run]->physScalerCount),
                    int(runs[run]->physTriggerCount),
                    runs[run]->lte,
                    runs[run]->lteUncertainty,
                    int(runs[run]->edtmScalerCount),
                    int(runs[run]->edtmTriggerCount),
                    runs[run]->trackingEfficiency,
                    runs[run]->trackingEfficiencyUncertainty,
                    int(runs[run]->trackDid),
                    int(runs[run]->trackShould),
                    runs[run]->cerEfficiency,
                    runs[run]->cerEfficiencyUncertainty,
                    int(runs[run]->calDid),
                    int(runs[run]->cerShould),
                    runs[run]->calEfficiency,
                    runs[run]->calEfficiencyUncertainty,
                    int(runs[run]->calDid),
                    int(runs[run]->calShould),
                    runs[run]->calEfficiency,
                    runs[run]->calEfficiencyUncertainty,
                    int(runs[run]->calDid),
                    int(runs[run]->calShould),
                    int(runs[run]->goodEvents),
                    (runs[run]->goodEvents/runs[run]->charge),
                    runs[run]->yield,
                    runs[run]->ps1Rate,
                    runs[run]->ps2Rate,
                    runs[run]->ps1,
                    runs[run]->ps2,
                    int(getPrescaleFactor(runs[run]->ps1)),
                    int(getPrescaleFactor(runs[run]->ps2))
                   )
            << std::endl;
    }
    ofs.close();
}

//--------------------------------------------------------------------------
Double_t getPrescaleFactor(Int_t ps) {

    // Lookup table
    std::map<Int_t,Int_t> psfactor;
    psfactor[-1] = 0;
    psfactor[0] = 1;
    psfactor[1] = 2;
    psfactor[2] = 3;
    psfactor[3] = 5;
    psfactor[4] = 9;
    psfactor[5] = 17;
    psfactor[6] = 33;
    psfactor[7] = 65;
    psfactor[8] = 129;
    psfactor[9] = 257;
    psfactor[10] = 513;
    psfactor[11] = 1025;
    psfactor[12] = 2049;
    psfactor[13] = 4097;
    psfactor[14] = 8193;
    psfactor[15] = 16385;
    psfactor[16] = 32769;

    return psfactor[ps];
}

//--------------------------------------------------------------------------
Double_t correctForPrescale(Double_t x, Int_t ps) {
    return x * getPrescaleFactor(ps);
}
