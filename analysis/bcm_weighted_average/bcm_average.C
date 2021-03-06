#include <fstream>
#include <vector>
#include <map>

#include <TF1.h>
#include <TH2F.h>
#include <TPad.h>
#include <TLine.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TSQLResult.h>

#include <CTData.h>
#include <CTCuts.h>

// Some of our runs did not have stable beam currents, as drawn below:
//
//
//  B|                         XXXXXXXXXXXX
//  C|                        X           X
//  M|                       X            X
//   |                      X             X
//   |     XXXXXXXX        X              X
//   |    X       X       X               X
//   |   X        X      X                X
//   |  X         X     X                 X
//   | X          XXXXXX                  X
//   +----------------------------------------
//                                        time
//
//
// For hydrogen data, this is a problem because we need to apply a
// boiling correction. To account for beam instability, I calculate
// weighted average for each run's beam current.
//
// We can use a weighted average current because the boiling correction is
// linear. If it were not, it would be better to calculate a correction for
// each region and then average that to get the overall boiling correction
// for the run.
//
// For each region (or entire run if the run is "stable") we do the following:
//   1) Calculate the average current, I
//   2) Count the beam as tripped if the current is 5% below I
//   3) Calculate a new average for non-tripped scaler reads, I'
//   4) Apply

struct bcm_avg_t {
    Int_t run;
    Bool_t weighted;
    Double_t avg;
    Double_t sem;
    Int_t regions; // number of regions = begin.size()
    std::vector<Int_t> begin; // starts of regions to average
    std::vector<Int_t> end;   // ends of regions to average
    std::vector<Int_t> countWithNoCut;   // non-trip scaler reads in regions
    std::vector<Int_t> countWithOneCut;  // non-trip scaler reads in regions
    std::vector<Int_t> countWithTwoCuts; // non-trip scaler reads in regions
    std::vector<Double_t> averagesWithNoCut;
    std::vector<Double_t> averagesWithOneCut;
    std::vector<Double_t> averagesWithTwoCuts;
    std::vector<Double_t> regionUncertainty;
    std::vector<Double_t> weights;
    std::vector<Double_t> rawScalerIncrements;
    std::vector<Double_t> scalerCharges;
    std::vector<Double_t> charges;
    std::vector<Int_t> goodEvents;
    std::vector<Double_t> times;
    Double_t myCharge;
    Double_t time;
    Double_t scalerCharge;
    Double_t scalerChargeCut;
    Double_t goodEventsTotal;
};

void bcm_average();

int main () {
    bcm_average();
    return 0;
}

void bcm_average() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");
    TCut goodCut;

    // Which bcm?
    TString scalerRawBranch       = "P.BCM4A.scaler";
    TString scalerCurrentBranch   = "P.BCM4A.scalerCurrent";
    TString scalerChargeBranch    = "P.BCM4A.scalerCharge";
    TString scalerChargeCutBranch = "P.BCM4A.scalerChargeCut";
    TString scalerTimeBranch      = "P.1MHz.scalerTime";

    // Where should we save diagnostic plots?
    TFile *fWrite = new TFile("/home/jmatter/ct_scripts/analysis/bcm_weighted_average/bcm_average.root", "RECREATE");

    // // Where should we save the calculated averages?
    // TString averageCsvFilename = "/home/jmatter/ct_scripts/analysis/bcm_weighted_average/csv/bcm_average.csv";
    // TString segmentCsvFilename = "/home/jmatter/ct_scripts/analysis/bcm_weighted_average/csv/segments.csv";
    TString averageCsvFilename;
    TString segmentCsvFilename;

    // These are used to indicate on histograms what the average is
    // lines[run][region] = new TF1(etc)
    std::map<Int_t, std::map<Int_t, TF1*>> lines;
    std::map<Int_t, std::map<Int_t, TGraph*>> shades;
    std::map<Int_t, TGraph*> graphs;
    std::map<Int_t, TGraph*> graphs2;

    // Weighting with SEM gives funny results. I haven't thought hard about why,
    // but weighting by number of scaler reads is a reasonable choice so I'm
    // comfortable using it.
    Bool_t weightWithRawScaler   = true;
    Bool_t weightWithScalerReads = false;
    Bool_t weightWithSEM         = false;

    // Do we only want to look at LH2?
    Bool_t lh2Only = false;

    // ------------------------------------------------------------------------
    // Initialize everything to be unweighted.
    // After this we'll change the settings for runs we want to weight.
    std::map<Int_t, bcm_avg_t> bcm_avgs;
    bcm_avg_t bcm_avg;
    for (auto const &k : data->GetNames()) {

        // LH2 only
        if (lh2Only && (data->GetTarget(k) != "LH2"))
            continue;

        for (auto const &run : data->GetRuns(k)) {
            bcm_avg.run = run;
            bcm_avg.weighted = false;
            bcm_avgs[bcm_avg.run] = bcm_avg;
        }
    }

    // ------------------------------------------------------------------------
    // Runs that need a weighted average
    // run   tgt  Q2    coll
    // ----------------------
    // 2278  LH2  9.5   pion
    // 2279  LH2  9.5   pion
    // 2283  LH2  14.3  pion
    // 2406  LH2  14.3  large
    // 2410  LH2  14.3  large
    // 2414  LH2  14.3  large
    // 2421  LH2  14.3  large
    // 2423  LH2  14.3  large
    // 2424  LH2  14.3  large
    // 2425  LH2  14.3  large
    // 2429  LH2  14.3  large
    // 2430  LH2  14.3  large
    // 2431  LH2  14.3  large
    // 2432  LH2  14.3  large
    // 2433  LH2  14.3  large
    // 2452  LH2  11.5  ???
    // 3188  LH2  9.5   large
    // ----------------------

    // 2278
    bcm_avg.run = 2278;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(14);
    bcm_avg.begin.push_back(16);
    bcm_avg.end.push_back(21);
    bcm_avg.begin.push_back(22);
    bcm_avg.end.push_back(43);
    bcm_avg.begin.push_back(54);
    bcm_avg.end.push_back(69);
    bcm_avg.begin.push_back(71);
    bcm_avg.end.push_back(106);
    bcm_avg.begin.push_back(127);
    bcm_avg.end.push_back(162);
    bcm_avg.begin.push_back(173);
    bcm_avg.end.push_back(298);
    bcm_avg.begin.push_back(318);
    bcm_avg.end.push_back(346);
    bcm_avg.begin.push_back(368);
    bcm_avg.end.push_back(462);
    bcm_avg.begin.push_back(480);
    bcm_avg.end.push_back(1412);
    bcm_avg.begin.push_back(1436);
    bcm_avg.end.push_back(1538);
    bcm_avg.begin.push_back(1550);
    bcm_avg.end.push_back(1577);
    bcm_avg.begin.push_back(1578);
    bcm_avg.end.push_back(1653);
    bcm_avg.begin.push_back(1722);
    bcm_avg.end.push_back(2162);
    bcm_avg.begin.push_back(2164);
    bcm_avg.end.push_back(2335);
    bcm_avg.begin.push_back(2336);
    bcm_avg.end.push_back(2510);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2279
    bcm_avg.run = 2279;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(443);
    bcm_avg.begin.push_back(550);
    bcm_avg.end.push_back(3273);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2283
    bcm_avg.run = 2283;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(2315);
    // bcm_avg.begin.push_back(2316);
    // bcm_avg.end.push_back(2440);
    bcm_avg.begin.push_back(2500);
    bcm_avg.end.push_back(2850);
    bcm_avg.begin.push_back(2851);
    bcm_avg.end.push_back(4000);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2406
    bcm_avg.run = 2406;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(354);
    bcm_avg.begin.push_back(485);
    bcm_avg.end.push_back(610);
    bcm_avg.begin.push_back(700);
    bcm_avg.end.push_back(1650);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2410
    bcm_avg.run = 2410;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(3000);
    bcm_avg.begin.push_back(3001);
    bcm_avg.end.push_back(3280);
    bcm_avg.begin.push_back(3290);
    bcm_avg.end.push_back(3330);
    bcm_avg.begin.push_back(3331);
    bcm_avg.end.push_back(3380);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2414
    bcm_avg.run = 2414;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(280);
    bcm_avg.begin.push_back(284);
    bcm_avg.end.push_back(400);
    bcm_avg.begin.push_back(415);
    bcm_avg.end.push_back(750);
    bcm_avg.begin.push_back(800);
    bcm_avg.end.push_back(910);
    bcm_avg.begin.push_back(915);
    bcm_avg.end.push_back(1500);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2421
    bcm_avg.run = 2421;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(1846);
    bcm_avg.begin.push_back(1847);
    bcm_avg.end.push_back(2270);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();


    // 2423
    bcm_avg.run = 2423;
    bcm_avg.weighted = false;
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2424
    bcm_avg.run = 2424;
    bcm_avg.weighted = false;
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();


    // 2425
    bcm_avg.run = 2425;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(445);
    bcm_avg.begin.push_back(470);
    bcm_avg.end.push_back(1140);
    bcm_avg.begin.push_back(1159);
    bcm_avg.end.push_back(1500);
    bcm_avg.begin.push_back(1501);
    bcm_avg.end.push_back(1750);
    bcm_avg.begin.push_back(1751);
    bcm_avg.end.push_back(2000);
    bcm_avg.begin.push_back(2060);
    bcm_avg.end.push_back(2990);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2429
    bcm_avg.run = 2429;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(1250);
    bcm_avg.begin.push_back(1300);
    bcm_avg.end.push_back(1644);
    bcm_avg.begin.push_back(1667);
    bcm_avg.end.push_back(2150);
    bcm_avg.begin.push_back(2260);
    bcm_avg.end.push_back(2336);
    bcm_avg.begin.push_back(2370);
    bcm_avg.end.push_back(3000);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2430
    bcm_avg.run = 2430;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(125);
    bcm_avg.begin.push_back(126);
    bcm_avg.end.push_back(314);
    bcm_avg.begin.push_back(315);
    bcm_avg.end.push_back(395);
    bcm_avg.begin.push_back(420);
    bcm_avg.end.push_back(488);
    bcm_avg.begin.push_back(493);
    bcm_avg.end.push_back(570);
    bcm_avg.begin.push_back(662);
    bcm_avg.end.push_back(715);
    bcm_avg.begin.push_back(740);
    bcm_avg.end.push_back(1020);
    bcm_avg.begin.push_back(1035);
    bcm_avg.end.push_back(1160);
    bcm_avg.begin.push_back(1165);
    bcm_avg.end.push_back(1341);
    bcm_avg.begin.push_back(1450);
    bcm_avg.end.push_back(1623);
    bcm_avg.begin.push_back(1629);
    bcm_avg.end.push_back(2100);
    bcm_avg.begin.push_back(2287);
    bcm_avg.end.push_back(3040);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2431
    bcm_avg.run = 2431;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(120);
    bcm_avg.end.push_back(190);
    bcm_avg.begin.push_back(224);
    bcm_avg.end.push_back(970);
    bcm_avg.begin.push_back(975);
    bcm_avg.end.push_back(1108);
    bcm_avg.begin.push_back(1109);
    bcm_avg.end.push_back(1149);
    bcm_avg.begin.push_back(1150);
    bcm_avg.end.push_back(1180);
    bcm_avg.begin.push_back(1181);
    bcm_avg.end.push_back(1248);
    bcm_avg.begin.push_back(1249);
    bcm_avg.end.push_back(1349);
    bcm_avg.begin.push_back(1350);
    bcm_avg.end.push_back(1390);
    bcm_avg.begin.push_back(1391);
    bcm_avg.end.push_back(1460);
    bcm_avg.begin.push_back(1480);
    bcm_avg.end.push_back(1590);
    bcm_avg.begin.push_back(1600);
    bcm_avg.end.push_back(1630);
    bcm_avg.begin.push_back(1650);
    bcm_avg.end.push_back(1785);
    bcm_avg.begin.push_back(1786);
    bcm_avg.end.push_back(1835);
    bcm_avg.begin.push_back(1840);
    bcm_avg.end.push_back(1948);
    bcm_avg.begin.push_back(1949);
    bcm_avg.end.push_back(1965);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2432
    bcm_avg.run = 2432;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(106);
    bcm_avg.begin.push_back(120);
    bcm_avg.end.push_back(206);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2433
    bcm_avg.run = 2433;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(30);
    bcm_avg.end.push_back(154);
    bcm_avg.begin.push_back(193);
    bcm_avg.end.push_back(331);
    bcm_avg.begin.push_back(336);
    bcm_avg.end.push_back(410);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 2452
    bcm_avg.run = 2452;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(1450);
    bcm_avg.begin.push_back(1600);
    bcm_avg.end.push_back(1730);
    bcm_avg.begin.push_back(1740);
    bcm_avg.end.push_back(1836);
    bcm_avg.begin.push_back(1840);
    bcm_avg.end.push_back(3166);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // 3188
    bcm_avg.run = 3188;
    bcm_avg.weighted = true;
    bcm_avg.begin.push_back(0);
    bcm_avg.end.push_back(1044);
    // bcm_avg.begin.push_back(1046);
    // bcm_avg.end.push_back(1064);
    bcm_avg.begin.push_back(1067);
    bcm_avg.end.push_back(2050);
    bcm_avg.begin.push_back(2060);
    bcm_avg.end.push_back(2148);
    // bcm_avg.begin.push_back(2198);
    // bcm_avg.end.push_back(2210);
    // bcm_avg.begin.push_back(2214);
    // bcm_avg.end.push_back(2225);
    // bcm_avg.begin.push_back(2258);
    // bcm_avg.end.push_back(2263);
    // bcm_avg.begin.push_back(2266);
    // bcm_avg.end.push_back(2280);
    bcm_avg.begin.push_back(2300);
    bcm_avg.end.push_back(2566);
    bcm_avgs[bcm_avg.run] = bcm_avg;
    bcm_avg.begin.clear();
    bcm_avg.end.clear();

    // ------------------------------------------------------------------------
    // Let's do the thing

    TCanvas* c = new TCanvas("c", "canvas", 1024, 640);
    TH2F *diagnosticHisto;
    TString drawMe, canvasName;
    Int_t N;

    // for average reference lines
    TString lineName;
    Double_t slope, yIntercept;

    TString rootFilename;
    TFile *file;
    TTree *TSP, *T;

    Int_t regionBegin, regionEnd;
    Int_t firstEvent;
    Int_t lastEvent;
    Double_t event;
    Double_t bcmCurrent;    // in uA
    Double_t bcmCharge;     // in uC
    Double_t bcmChargeCut;  // in uC
    Double_t scalerTime;
    Double_t thisRawScalerRead;
    Double_t lastRawScalerRead;
    Double_t thisChargeRead;
    Double_t lastChargeRead;
    Double_t lastTimeRead;
    Double_t numerator, denominator; // for calculating averages, uncertainty

    for (auto const &k : data->GetNames()) {

        // LH2 only
        if (lh2Only && (data->GetTarget(k) != "LH2"))
            continue;

        for (auto const &run : data->GetRuns(k)) {

            std::cout << "--------------" << std::endl;

            // Open file
            rootFilename = Form(data->GetRootfileTemplate(k),
                                data->GetRootfileDirectory().Data(),
                                run);
            file = new TFile(rootFilename.Data(), "READ");
            file->GetObject("TSP", TSP);
            file->GetObject("T", T);

            // Draw diagnostic histogram
            canvasName = Form("run%d_%s", run, scalerCurrentBranch.Data());
            drawMe = Form("%s:This->GetReadEntry()", scalerCurrentBranch.Data());
            N = TSP->Draw(drawMe.Data(), "", "goff");

            graphs[run] = new TGraph(N, TSP->GetV2(), TSP->GetV1());
            graphs[run]->SetTitle(Form("run %d; Entry; %s", run, scalerCurrentBranch.Data()));
            graphs[run]->Draw("AL");

            // Set branch addresses
            TSP->SetBranchAddress(scalerRawBranch.Data(),       &thisRawScalerRead);
            TSP->SetBranchAddress(scalerCurrentBranch.Data(),   &bcmCurrent);
            TSP->SetBranchAddress(scalerChargeBranch.Data(),    &bcmCharge);
            TSP->SetBranchAddress(scalerChargeCutBranch.Data(), &bcmChargeCut);
            TSP->SetBranchAddress(scalerTimeBranch.Data(),      &scalerTime);
            TSP->SetBranchAddress("evNumber",                   &event);

            if (data->GetTarget(k) == "LH2") {
                goodCut = cuts->Get("explicitLH2CoinCut");
            } else {
                goodCut = cuts->Get("explicitC12CoinCut");
            }

            // If not weighted, we need to set the beginning and end of the
            // region to be the size of the tree. Should we do this for the
            // weighted runs as well? Might be interesting to see how different
            // the averages are.
            if (!(bcm_avgs[run].weighted)) {
                bcm_avgs[run].begin.push_back(0);
                bcm_avgs[run].end.push_back(TSP->GetEntries());
            }

            // How many regions are there?
            bcm_avgs[run].regions = bcm_avgs[run].begin.size();

            // Loop over regions and calculate averages
            for (int n = 0; n < bcm_avgs[run].regions; n++) {
                regionBegin = bcm_avgs[run].begin[n];
                regionEnd   = bcm_avgs[run].end[n];

                // Check that region ends aren't longer than the tree
                if (regionEnd >= TSP->GetEntries()) {
                    regionEnd = (TSP->GetEntries()-1);
                }

                // ---------------------------------------
                // Regular average
                bcm_avgs[run].countWithNoCut.push_back(0);
                bcm_avgs[run].averagesWithNoCut.push_back(0);
                for (int scalerRead = regionBegin; scalerRead <= regionEnd; scalerRead++) {
                    TSP->GetEntry(scalerRead);

                    bcm_avgs[run].averagesWithNoCut[n] += bcmCurrent;

                    // Scaler read counts
                    bcm_avgs[run].countWithNoCut[n]++;
                }
                bcm_avgs[run].averagesWithNoCut[n] /= bcm_avgs[run].countWithNoCut[n];

                // ---------------------------------------
                // Apply one 95% cut
                bcm_avgs[run].countWithOneCut.push_back(0);
                bcm_avgs[run].averagesWithOneCut.push_back(0);
                for (int scalerRead = regionBegin; scalerRead <= regionEnd; scalerRead++) {
                    TSP->GetEntry(scalerRead);

                    // Only count this read if it's above 95% of average
                    if (bcmCurrent > (0.95*bcm_avgs[run].averagesWithNoCut[n])) {
                        bcm_avgs[run].averagesWithOneCut[n] += bcmCurrent;

                        // Scaler read counts
                        bcm_avgs[run].countWithOneCut[n]++;
                    }
                }
                bcm_avgs[run].averagesWithOneCut[n] /= bcm_avgs[run].countWithOneCut[n];

                // ---------------------------------------
                // Apply a second 95% cut
                bcm_avgs[run].countWithTwoCuts.push_back(0);
                bcm_avgs[run].averagesWithTwoCuts.push_back(0);
                bcm_avgs[run].rawScalerIncrements.push_back(0);
                bcm_avgs[run].charges.push_back(0);
                bcm_avgs[run].goodEvents.push_back(0);
                bcm_avgs[run].times.push_back(0);

                thisRawScalerRead = 0;
                lastRawScalerRead = 0;
                lastChargeRead = 0;
                bcmCharge = 0;
                lastTimeRead = 0;
                scalerTime = 0;

                for (int scalerRead = regionBegin; scalerRead <= regionEnd; scalerRead++) {
                    // Keep track of last read
                    lastRawScalerRead = thisRawScalerRead;
                    lastChargeRead = bcmCharge;
                    lastTimeRead = scalerTime;

                    TSP->GetEntry(scalerRead);

                    // Only count this read if it's above 95% of average
                    if (bcmCurrent > (0.95*bcm_avgs[run].averagesWithOneCut[n])) {
                        std::cout << Form("%d,%d,%d,%f,%f", run, n, scalerRead, lastChargeRead, bcmCharge) << std::endl;
                        bcm_avgs[run].averagesWithTwoCuts[n] += bcmCurrent;

                        // Scaler read counts
                        bcm_avgs[run].countWithTwoCuts[n]++;

                        // Raw scaler incremenet
                        bcm_avgs[run].rawScalerIncrements[n] += (thisRawScalerRead - lastRawScalerRead);

                        // Charge increment
                        bcm_avgs[run].charges[n] += (bcmCharge - lastChargeRead);

                        // Count good events for this scaler read
                        // First, need the first/last event numbers for this region

                        if (scalerRead < TSP->GetEntries()) { // make sure we're NOT at the last read
                            firstEvent = event-1;
                            TSP->GetEntry(scalerRead+1); // get next read
                            lastEvent = event-2;
                            TSP->GetEntry(scalerRead); // get next read
                        } else { // for last read, get size of T tree
                            firstEvent = event-1;
                            lastEvent = T->GetEntries() - 1;
                        }

                        for (int evNum = firstEvent; evNum <= lastEvent; evNum++) {
                            // Count event if it passes our "good" cut
                            if (T->Query("", goodCut.GetTitle(), "", 1, evNum)->GetRowCount() != 0) {
                                bcm_avgs[run].goodEvents[n]++;
                            }
                        }
                        std::cout << Form("event %d to %d: events = %d\n", firstEvent, lastEvent, bcm_avgs[run].goodEvents[n]);

                        // Time increment
                        bcm_avgs[run].times[n] += (scalerTime - lastTimeRead);
                    }
                }
                bcm_avgs[run].averagesWithTwoCuts[n] /= bcm_avgs[run].countWithTwoCuts[n];

                // ---------------------------------------
                // Calculate change in scaler charge (without cut)
                T->GetEntry(regionBegin);
                Double_t startCharge = bcmCharge;
                T->GetEntry(regionEnd-1);
                Double_t endCharge = bcmCharge;
                bcm_avgs[run].scalerCharges.push_back(endCharge - startCharge);

                // ---------------------------------------
                // Calculate this region's uncertainty
                numerator   = bcm_avgs[run].averagesWithTwoCuts[n];
                denominator = sqrt(bcm_avgs[run].rawScalerIncrements[n]);
                bcm_avgs[run].regionUncertainty.push_back(numerator/denominator);

                // ---------------------------------------
                // Assign weight
                if (weightWithSEM) {
                    bcm_avgs[run].weights.push_back( 1 / (bcm_avgs[run].regionUncertainty[n] * bcm_avgs[run].regionUncertainty[n]) );
                }
                if (weightWithScalerReads) {
                    bcm_avgs[run].weights.push_back( bcm_avgs[run].countWithTwoCuts[n] );
                }
                if (weightWithRawScaler) {
                    bcm_avgs[run].weights.push_back( bcm_avgs[run].rawScalerIncrements[n] );
                }

                // ---------------------------------------
                // Print result to cout for progress indication
                std::cout << Form("Run %5d region %3d: start %-4d end %-4d avgs = %-5.2f | %-5.2f | %-5.2f +- %-5.2f (n=%9d) \t charge=%f \t time=%f\n",
                                  run, n, regionBegin, regionEnd,
                                  bcm_avgs[run].averagesWithNoCut[n],
                                  bcm_avgs[run].averagesWithOneCut[n],
                                  bcm_avgs[run].averagesWithTwoCuts[n],
                                  bcm_avgs[run].regionUncertainty[n],
                                  bcm_avgs[run].countWithTwoCuts[n],
                                  bcm_avgs[run].charges[n],
                                  bcm_avgs[run].times[n]
                                  );

                // ---------------------------------------
                // Draw

                // Draw reference line for this region
                lineName = Form("run%d_region%d", run, n);
                lines[run][n] = new TF1(lineName.Data(), "[0]*x+[1]", regionBegin, regionEnd);
                slope = 0;
                yIntercept = bcm_avgs[run].averagesWithTwoCuts[n];
                lines[run][n]->SetParameters(slope, yIntercept);
                lines[run][n]->SetLineColor(kRed);
                lines[run][n]->Draw("SAME");

                // Draw uncertainty for this region
                shades[run][n] = new TGraph(4);
                shades[run][n]->SetPoint(0,regionBegin, bcm_avgs[run].averagesWithTwoCuts[n] + bcm_avgs[run].regionUncertainty[n]);
                shades[run][n]->SetPoint(1,regionEnd,   bcm_avgs[run].averagesWithTwoCuts[n] + bcm_avgs[run].regionUncertainty[n]);
                shades[run][n]->SetPoint(2,regionEnd,   bcm_avgs[run].averagesWithTwoCuts[n] - bcm_avgs[run].regionUncertainty[n]);
                shades[run][n]->SetPoint(3,regionBegin, bcm_avgs[run].averagesWithTwoCuts[n] - bcm_avgs[run].regionUncertainty[n]);
                shades[run][n]->SetFillStyle(3013);
                shades[run][n]->SetFillColor(kRed);
                shades[run][n]->Draw("fSAME");

            } // end loop over regions

            c->Modified();
            c->Update();

            // Get scaler charge calculated by hcana
            TSP->GetEntry(TSP->GetEntries()-1);
            bcm_avgs[run].scalerCharge    = bcmCharge;
            bcm_avgs[run].scalerChargeCut = bcmChargeCut;

            // Calculate my total charge
            bcm_avgs[run].myCharge = 0;
            for (int n = 0; n < bcm_avgs[run].regions; n++) {
                bcm_avgs[run].myCharge += bcm_avgs[run].charges[n];
            }

            // Calculate total time
            bcm_avgs[run].time = 0;
            for (int n = 0; n < bcm_avgs[run].regions; n++) {
                bcm_avgs[run].time += bcm_avgs[run].times[n];
            }

            // Calculate total good events
            bcm_avgs[run].goodEventsTotal = 0;
            for (int n = 0; n < bcm_avgs[run].regions; n++) {
                bcm_avgs[run].goodEventsTotal += bcm_avgs[run].goodEvents[n];
            }

            // Calculate the weighted average
            numerator = 0;
            denominator = 0;
            for (int n = 0; n < bcm_avgs[run].regions; n++) {
                numerator   += bcm_avgs[run].averagesWithTwoCuts[n]  * bcm_avgs[run].weights[n];
                denominator += bcm_avgs[run].weights[n];
            }
            bcm_avgs[run].avg = numerator / denominator;
            bcm_avgs[run].sem = 1 / sqrt(denominator);

            std::cout << Form("\nRun %d\t weighted avg = %f\n\n\n", run, bcm_avgs[run].avg);

            // Write histogram to file
            fWrite->WriteObject(c, canvasName.Data());

            canvasName = Form("run%d_%s_vs_time", run, scalerCurrentBranch.Data());
            drawMe = Form("%s:%s", scalerCurrentBranch.Data(), scalerTimeBranch.Data());
            N = TSP->Draw(drawMe.Data(), "", "goff");

            graphs2[run] = new TGraph(N, T->GetV2(), T->GetV1());
            graphs2[run]->SetTitle(Form("run %d; time; %s", run, scalerCurrentBranch.Data()));
            graphs2[run]->Draw("AL");

            fWrite->WriteObject(c, canvasName.Data());

        // } // loop over runs
    // } // loop over kinematics

            //-------------------------------------------------------------------------
            // Write weighted averages to csv

            // Where should we save the calculated averages?
            averageCsvFilename = Form("/home/jmatter/ct_scripts/analysis/bcm_weighted_average/csv/bcm_average_run%d.csv", run);
            segmentCsvFilename = Form("/home/jmatter/ct_scripts/analysis/bcm_weighted_average/csv/segments_run%d.csv", run);

            std::ofstream ofs;
            ofs.open(averageCsvFilename.Data());
            ofs << "run, bcm4aAverage, bcm4AUncertainty, myCharge, scalerCharge, scalerChargeCut, scalerTime, goodEvents" << std::endl;
    // for (auto const &k : data->GetNames()) {

        // // LH2 only
        // if (lh2Only && (data->GetTarget(k) != "LH2"))
        //     continue;

        // for (auto const &run : data->GetRuns(k)) {
            ofs << run
                << "," << bcm_avgs[run].avg
                << "," << bcm_avgs[run].sem
                << "," << bcm_avgs[run].myCharge
                << "," << bcm_avgs[run].scalerCharge
                << "," << bcm_avgs[run].scalerChargeCut
                << "," << bcm_avgs[run].time
                << "," << bcm_avgs[run].goodEventsTotal
                << std::endl;
        // }
    // }
            ofs.close();

            // Write each segment to csv for further investigation
            ofs.open(segmentCsvFilename.Data());
            ofs << "run, segment, begin, end, averagesWithNoCut, averagesWithOneCut, averagesWithTwoCuts, regionUncertainty, countTwoCuts, rawScalerWeight, scalerCharge, myCharge, myTime, goodEvents" << std::endl;
    // for (auto const &k : data->GetNames()) {

        // // LH2 only
        // if (lh2Only && (data->GetTarget(k) != "LH2"))
        //     continue;

        // for (auto const &run : data->GetRuns(k)) {
            for (int n = 0; n < bcm_avgs[run].regions; n++) {
                regionBegin = bcm_avgs[run].begin[n];
                regionEnd   = bcm_avgs[run].end[n];

                ofs << run
                    << "," << n
                    << "," << regionBegin
                    << "," << regionEnd
                    << "," << bcm_avgs[run].averagesWithNoCut[n]
                    << "," << bcm_avgs[run].averagesWithOneCut[n]
                    << "," << bcm_avgs[run].averagesWithTwoCuts[n]
                    << "," << bcm_avgs[run].regionUncertainty[n]
                    << "," << bcm_avgs[run].countWithTwoCuts[n]
                    << "," << bcm_avgs[run].rawScalerIncrements[n]
                    << "," << bcm_avgs[run].scalerCharges[n]
                    << "," << bcm_avgs[run].charges[n]
                    << "," << bcm_avgs[run].times[n]
                    << "," << bcm_avgs[run].goodEvents[n]
                    << std::endl;
            }
            ofs.close();
        }
    }

    // Write histograms to disk
    fWrite->Close();

}
