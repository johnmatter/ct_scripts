#include <utility>
#include <fstream>
#include <map>

#include <TH1F.h>
#include <TCut.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TPaveLabel.h>
#include <TPaveStats.h>

#include <CTData.h>

// This calculates livetime.
// It also creates histograms of triggers' raw TDC times, both with and without window cuts.
// This allows us to check if our windows are set correctly
// Big thanks to Carlos Yero for the inspiration for this script.
void livetime() {
    //-------------------------------------------------------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // CSV to save
    TString csvFilename = "livetime.csv";

    // Needed for reading data
    TString rootFilename;
    TFile *f;
    TTree *T, *TS;

    // Store scaler reads and decoded data
    std::map<TString, Double_t> thisRead;
    std::map<TString, Double_t> prevRead;
    std::map<TString, Double_t> value;

    // Key is <run, scaler>
    std::map<std::tuple<Int_t, TString>, Double_t> scalerTotal;

    // Flags for whether or not event was above current threshold
    std::vector<Bool_t> eventFlag;

    // Accepted triggers
    // Key is <run, trigger, description>
    std::map<std::tuple<Int_t, TString, TString>, Int_t> acceptedTrigs;

    // Livetimes
    // Key is <run, lt> where lt is one of  {"edtm", "cpu", "phys"}
    std::map<std::tuple<Int_t, TString>, Double_t> livetime;

    // Scaler read's upper limit
    std::vector<Int_t> scalerEventNumber;

    // Scalers to read
    TString bcm = "P.BCM4A.scalerCurrent";
    TString bcmQ = "P.BCM4A.scalerCharge";
    TString timer = "P.1MHz.scalerTime";
    TString physBranch = "T.coin.pTRIG6_ROC2_tdcTimeRaw";
    TString physScaler = "P.pTRIG6.scaler";
    TString edtmScaler = "P.EDTM.scaler";
    std::vector<TString> scalers = {
               "P.pTRIG1.scaler", // SHMS 3/4
               "P.pTRIG4.scaler", // HMS  3/4
               "P.pTRIG6.scaler", // SHMS 3/4 & HMS 3/4 coincidence
               "P.EDTM.scaler",   // EDTM
               bcm,               // Merely for bookkeeping. The csv value is meaningless.
               bcmQ,              // This one is actually useful.
               timer,             // You can use this to get average current though.
               "evNumber"         // Currently, this total will be wrong in the csv.
              };

    // Current threshold for trips in uA
    // TODO: implement a "less than 90% of max" threshold
    Double_t currentThreshold = 10.0;

    // Data we need from T
    std::vector<TString> trigBranches;
    trigBranches = {
                "T.coin.pTRIG1_ROC2_tdcTimeRaw",
                "T.coin.pTRIG4_ROC2_tdcTimeRaw",
                "T.coin.pTRIG6_ROC2_tdcTimeRaw",
                };

    TString edtmBranch = "T.coin.pEDTM_tdcTimeRaw";

    std::vector<TString> dataBranches;
    dataBranches = {
                // "g.evnum",
                "H.cal.etottracknorm",
                "H.cer.npeSum",
                "P.ngcer.npeSum",
                "P.gtr.dp",
                "H.gtr.dp",
                "P.dc.ntrack",
                "H.dc.ntrack"
               };

    // TODO: somehow read this info from the param file itself
    // This is my tcoin.param, with cuts set by Deepak.
    // These are parallel arrays. They should have the same number of elements, but for some reason they do not.
    // TODO: Is it safe to assume that hcana uses open windows if you don't specify limits?
    std::vector<TString> t_coin_adcNames = {
                            "hASUM", "hBSUM", "hCSUM", "hDSUM", "hPSHWR", "hSHWR", "hAER", "hCER", "hFADC_TREF_ROC1", "pAER",
                            "pHGCER", "pNGCER", "pPSHWR", "pFADC_TREF_ROC2", "pHGCER_MOD", "pNGCER_MOD", "pHEL_NEG", "pHEL_POS", "pHEL_MPS"};
    std::vector<TString> t_coin_tdcNames = {
                            "h1X", "h1Y", "h2X", "h2Y", "h1T", "h2T", "hT1", "hASUM", "hBSUM", "hCSUM",
                            "hDSUM", "hPRLO", "hPRHI", "hSHWR", "hEDTM", "hCER", "hT2", "hDCREF1", "hDCREF2", "hDCREF3",
                            "hDCREF4", "hTRIG1_ROC1", "hTRIG2_ROC1", "hTRIG3_ROC1", "hTRIG4_ROC1", "hTRIG5_ROC1", "hTRIG6_ROC1", "pTRIG1_ROC1", "pTRIG2_ROC1", "pTRIG3_ROC1",
                            "pTRIG4_ROC1", "pTRIG5_ROC1", "pTRIG6_ROC1", "pT1", "pT2", "p1X", "p1Y", "p2X", "p2Y", "p1T",
                            "p2T", "pT3", "pAER", "pHGCER", "pNGCER", "pDCREF1", "pDCREF2", "pDCREF3", "pDCREF4", "pDCREF5",
                            "pDCREF6", "pDCREF7", "pDCREF8", "pDCREF9", "pDCREF10", "pEDTM", "pPRLO", "pPRHI", "pTRIG1_ROC2", "pTRIG2_ROC2",
                            "pTRIG3_ROC2", "pTRIG4_ROC2", "pTRIG5_ROC2", "pTRIG6_ROC2", "hTRIG1_ROC2", "hTRIG2_ROC2", "hTRIG3_ROC2", "hTRIG4_ROC2", "hTRIG5_ROC2", "hTRIG6_ROC2",
                            "pSTOF_ROC2", "pEL_LO_LO_ROC2", "pEL_LO_ROC2", "pEL_HI_ROC2", "pEL_REAL_ROC2", "pEL_CLEAN_ROC2", "hSTOF_ROC2", "hEL_LO_LO_ROC2", "hEL_LO_ROC2", "hEL_HI_ROC2",
                            "hEL_REAL_ROC2", "hEL_CLEAN_ROC2", "pSTOF_ROC1", "pEL_LO_LO_ROC1", "pEL_LO_ROC1", "pEL_HI_ROC1", "pEL_REAL_ROC1", "pEL_CLEAN_ROC1", "hSTOF_ROC1", "hEL_LO_LO_ROC1",
                            "hEL_LO_ROC1", "hEL_HI_ROC1", "hEL_REAL_ROC1", "hEL_CLEAN_ROC1", "pPRE40_ROC1", "pPRE100_ROC1", "pPRE150_ROC1", "pPRE200_ROC1", "hPRE40_ROC1", "hPRE100_ROC1",
                            "hPRE150_ROC1", "hPRE200_ROC1", "pPRE40_ROC2", "pPRE100_ROC2", "pPRE150_ROC2", "pPRE200_ROC2", "hPRE40_ROC2", "hPRE100_ROC2", "hPRE150_ROC2", "hPRE200_ROC2",
                            "hDCREF5", "hT3", "pRF", "hHODO_RF", "pHODO_RF"};

    std::vector<Int_t> t_coin_TdcTimeWindowMin = {
                            0, 0, 0, 0, 0, 0, 1500, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 1500, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 2300, 0, 0,
                            2400, 0, 2400, 3000, 2000, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 12500, 12500, 12500, 12500, 12500,
                            12500,  12500,  12500, 12500, 12500, 0, 0, 0, 2700, 0,
                            0, 3100, 0, 3000, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0};

    std::vector<Int_t> t_coin_TdcTimeWindowMax = {
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 4500, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 3600, 100000, 100000,
                            3500, 100000, 3300, 4500, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 4000, 100000,
                            100000, 4000, 100000, 4000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000};


    // Create maps from the above info
    std::map<TString, Int_t> tdcTimeWindowMin;
    std::map<TString, Int_t> tdcTimeWindowMax;
    for (int i=0; i<t_coin_tdcNames.size(); i++) {
        TString tdc = Form("T.coin.%s_tdcTimeRaw",t_coin_tdcNames[i].Data());

        // TDC time window min
        if (i<t_coin_TdcTimeWindowMin.size()) {
            tdcTimeWindowMin[tdc] = t_coin_TdcTimeWindowMin[i];
        } else {
            tdcTimeWindowMin[tdc] = 0;
            std::cout << "Index for " << tdc << " in t_coin_tdcNames beyond bounds of t_coin_TdcTimeWindowMin. Using " << tdcTimeWindowMin[tdc] << std::endl;
            std::cout << "CHECK YOUR VECTORS" << std::endl;
        }

        // TDC time window max
        if (i<t_coin_TdcTimeWindowMax.size()) {
            tdcTimeWindowMax[tdc] = t_coin_TdcTimeWindowMax[i];
        } else {
            tdcTimeWindowMax[tdc] = 10000;
            std::cout << "Index for " << tdc << " in t_coin_tdcNames beyond bounds of t_coin_TdcTimeWindowMax. Using " << tdcTimeWindowMax[tdc] << std::endl;
            std::cout << "CHECK YOUR VECTORS" << std::endl;
        }
    }

    // Initialize histograms
    // Key is <kinematics, trigger, {"open","cut","noedtm"}>
    std::map<std::tuple<TString, TString, TString>, TH1F*> histosPerKinematics;
    // Key is <kinematics, run, trigger, {"open","cut","noedtm"}>
    std::map<std::tuple<TString, Int_t, TString, TString>, TH1F*> histosPerRun;
    std::map<TString, TH1F*> histosTemp;
    TString histoName, drawMe, histoTitle;
    Double_t xMin, xMax;
    Int_t nBins = 200;
    for (auto const &k : kinematics) {
        for (auto const &branch: trigBranches) {
            histoName = Form("%s_%s_temp", k.Data(), branch.Data());
            drawMe = Form("%s>>%s", branch.Data(), histoName.Data());
            data->GetChain(k)->Draw(drawMe.Data(), "", "goff");
            histosTemp[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

            xMin = histosTemp[histoName]->GetXaxis()->GetXmin();
            xMax = histosTemp[histoName]->GetXaxis()->GetXmax();

            // Create histo summed over runs for this kinematics
            histoName = Form("%s_%s_open", k.Data(), branch.Data());
            histoTitle = Form("%s;%s;Counts", k.Data(), branch.Data());
            histosPerKinematics[std::make_tuple(k, branch, "open")] = new TH1F(histoName.Data(),
                                                                     histoTitle.Data(),
                                                                     nBins, xMin, xMax);

            histoName = Form("%s_%s_cut", k.Data(), branch.Data());
            histoTitle = Form("%s;%s;Counts", k.Data(), branch.Data());
            histosPerKinematics[std::make_tuple(k, branch, "cut")] = new TH1F(histoName.Data(),
                                                                     histoTitle.Data(),
                                                                     nBins, xMin, xMax);

            histoName = Form("%s_%s_noedtm", k.Data(), branch.Data());
            histoTitle = Form("%s;%s;Counts", k.Data(), branch.Data());
            histosPerKinematics[std::make_tuple(k, branch, "noedtm")] = new TH1F(histoName.Data(),
                                                                     histoTitle.Data(),
                                                                     nBins, xMin, xMax);

            // Create histo for runs
            for (auto const &run : data->GetRuns(k)) {
                histoName = Form("%s_run%d_%s_open", k.Data(), run, branch.Data());
                histoTitle = Form("%s: run %d;%s;Counts", k.Data(), run, branch.Data());
                histosPerRun[std::make_tuple(k,run,branch,"open")] = new TH1F(histoName.Data(),
                                                                      histoTitle.Data(),
                                                                      nBins, xMin, xMax);

                histoName = Form("%s_run%d_%s_cut", k.Data(), run, branch.Data());
                histoTitle = Form("%s: run %d;%s;Counts", k.Data(), run, branch.Data());
                histosPerRun[std::make_tuple(k,run,branch,"cut")] = new TH1F(histoName.Data(),
                                                                      histoTitle.Data(),
                                                                      nBins, xMin, xMax);

                histoName = Form("%s_run%d_%s_noedtm", k.Data(), run, branch.Data());
                histoTitle = Form("%s: run %d;%s;Counts", k.Data(), run, branch.Data());
                histosPerRun[std::make_tuple(k,run,branch,"noedtm")] = new TH1F(histoName.Data(),
                                                                      histoTitle.Data(),
                                                                      nBins, xMin, xMax);
            }
        }
    }

    // Initialize trig counters for all keys to be used in this analysis
    TString description;
    for (auto const &k: kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            // EDTM
            description = "EDTM";
            acceptedTrigs[std::make_tuple(run, edtmBranch, description)] = 0;

            // trig and !EDTM
            for (auto const &trig: trigBranches) {
                description = "!EDTM";
                acceptedTrigs[std::make_tuple(run, trig, description)] = 0;
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Count triggers, calculate livetime
    for (auto const &k : kinematics) {
        // We didn't have EDTM for Q^2=8 GeV^2 data, so skip for now
        // There are ways to do this without the EDTM, but I don't have it
        // implemented in this script yet.
        if (data->GetQ2(k)==8) {
            std::cout << "Skip " << k << std::endl;
            continue;
        }
        std::cout << "Begin " << k << std::endl;

        for (auto const &run : data->GetRuns(k)) {
            std::cout << "Run: " << run << "--------------------" << std::endl;

            // ----------------------------------------------------------------
            // Bookkeeping

            // Initialize scalers, branches, counts, etc.
            for (auto const &scaler: scalers) {
                thisRead[scaler] = 0;
                prevRead[scaler] = 0;
                scalerTotal[std::make_tuple(run,scaler)] = 0;
                // for (std::map<tuple<Int_t, TString, TString>, Int_t>::iterator it=acceptedTrigs.begin(); it!=acceptedTrigs.end(); it++) {
                //     acceptedTrigs[it->first] = 0;
                // }
            }
            eventFlag.clear();
            scalerEventNumber.clear();

            // Get trees for this run from the rootfile
            rootFilename = Form(data->GetRootfileTemplate(k),
                                data->GetRootfileDirectory().Data(),
                                run);
            f  = new TFile(rootFilename.Data(), "READ");
            T  = (TTree*) f->Get("T");
            TS = (TTree*) f->Get("TSP");

            // Set branch addresses
            for (auto const &scaler: scalers) {
                TS->SetBranchAddress(scaler, &(thisRead[scaler]));
            }

            for (auto const &branch: trigBranches) {
                T->SetBranchAddress(branch, &(value[branch]));
            }

            T->SetBranchAddress(edtmBranch, &(value[edtmBranch]));

            for (auto const &branch: dataBranches) {
                T->SetBranchAddress(branch, &(value[branch]));
            }

            // ----------------------------------------------------------------
            // Loop over scalers and count triggers
            for (int i=0; i<TS->GetEntries(); i++) {
                if (i%1000==0) {
                    std::cout << "Scaler progress: " << setprecision(3) << 100*Double_t(i)/TS->GetEntries() << "%" << std::endl;
                }
                TS->GetEntry(i);

                // Push back the event upper limit for this scaler read
                scalerEventNumber.push_back(thisRead["evNumber"]);

                // If current above threshold, increment scalerTotal by an amount
                // equal to the difference between this read and the previous one
                eventFlag.push_back(thisRead[bcm] > currentThreshold);
                if (eventFlag[i]) {
                    for (auto const &scaler: scalers) {
                        Double_t increment = (thisRead[scaler] - prevRead[scaler]);

                        // // Debug cout
                        // std::cout << Form("\n%-24s %d: %10.1f += ((%-10.0f-%-10.0f)=(%-6.0f))\n",
                        //                    scaler.Data(),
                        //                    i,
                        //                    scalerTotal[std::make_tuple(run,scaler)],
                        //                    thisRead[scaler],
                        //                    prevRead[scaler],
                        //                    increment);

                        scalerTotal[std::make_tuple(run,scaler)] += (increment);
                    }
                }

                // Store this scaler read for the next read
                for (auto const &scaler: scalers) {
                    prevRead[scaler] = thisRead[scaler];
                }
            }
            std::cout << "Scaler progress: 100%" << std::endl;

            // // Debug cout
            // for (auto const &scaler: scalers) {
            //     std::cout << Form("%-24s total = %10.0f\n", scaler.Data(), scalerTotal[std::make_tuple(run,scaler)]);
            // }

            // ----------------------------------------------------------------
            // Loop over data and count triggers.
            // Need to keep track of scaler reads for current trips
            Int_t scalerRead = 0;
            for (int i=0; i<T->GetEntries(); i++) {
                if (i%2000==0) {
                    std::cout << "Data progress: " << setprecision(3) << 100*Double_t(i)/T->GetEntries() << "%" << std::endl;
                }
                T->GetEntry(i);

                // If current not tripped (according to scaler read above), increment counts and fill histograms
                if (eventFlag[scalerRead]) {
                    // EDTM
                    // std::cout << Form("%-32s %f in (%d, %d)?\n", edtmBranch.Data(), value[edtmBranch], tdcTimeWindowMin[edtmBranch], tdcTimeWindowMax[edtmBranch]);
                    if (value[edtmBranch]>0) {
                        description = "EDTM";
                        acceptedTrigs[std::make_tuple(run, edtmBranch, description)]++;
                    }

                    // physics trigger
                    for (auto const &trig: trigBranches) {
                        // Fill open histo summed over runs for this kinematics
                        histosPerKinematics[std::make_tuple(k, trig, "open")]->Fill(value[trig]);
                        // Fill open histo for this run
                        histosPerRun[std::make_tuple(k, run, trig, "open")]->Fill(value[trig]);

                        // Check for physics trigger
                        // std::cout << Form("event %d: %-32s %f in (%d, %d)?\n", i, trig.Data(), value[trig], tdcTimeWindowMin[trig], tdcTimeWindowMax[trig]);
                        if (value[trig]>0) {
                            // Increment count of accepted triggers
                            description = "!EDTM";
                            acceptedTrigs[std::make_tuple(run, trig, description)]++;
                        }

                        // Check if inside the time window and not an edtm trigger
                        // std::cout << Form("event %d: %-32s %f in (%d, %d)?\n", i, trig.Data(), value[trig], tdcTimeWindowMin[trig], tdcTimeWindowMax[trig]);
                        if ((value[trig]>tdcTimeWindowMin[trig]) && (value[trig]<tdcTimeWindowMax[trig])) {

                            // Fill cut histo summed over runs for this kinematics
                            histosPerKinematics[std::make_tuple(k, trig, "cut")]->Fill(value[trig]);
                            // Fill cut histo for this run
                            histosPerRun[std::make_tuple(k, run, trig, "cut")]->Fill(value[trig]);

                            // Check if inside the time window AND not an edtm trigger
                            if (value[edtmBranch]==0) {
                                // Fill cut histo summed over runs for this kinematics
                                histosPerKinematics[std::make_tuple(k, trig, "noedtm")]->Fill(value[trig]);
                                // Fill cut histo for this run
                                histosPerRun[std::make_tuple(k, run, trig, "noedtm")]->Fill(value[trig]);
                            }
                        }
                    }
                }

                // scalerRead should be incremented if we've reached it
                // if (value["g.evnum"] >= scalerEventNumber[scalerRead]) {
                // If I'm not wrong, g.evnum == (i+1), so we'll do this instead
                if ((i+1) >= scalerEventNumber[scalerRead]) {
                    scalerRead++;
                }
            }
            std::cout << "Data progress: 100%" << std::endl;

            // ----------------------------------------------------------------
            // Calculate live time
            // scalerTotal is a Double_t so we should be fine without recasting anything
            Int_t physTrigN, physScalerN;
            Int_t edtmTrigN, edtmScalerN;

            edtmTrigN = acceptedTrigs[std::make_tuple(run,edtmBranch,"EDTM")];
            edtmScalerN = scalerTotal[std::make_tuple(run,edtmScaler)];
            physTrigN = acceptedTrigs[std::make_tuple(run,physBranch,"!EDTM")];
            physScalerN = scalerTotal[std::make_tuple(run,physScaler)];

            std::cout << Form("edtm lt = %d/%d\n", edtmTrigN, edtmScalerN);
            std::cout << Form("cpu  lt = %d/%d\n", physTrigN, physScalerN);
            std::cout << Form("phys lt = (%d-%d)/(%d-%d)\n", physTrigN, edtmTrigN, physScalerN, edtmScalerN);

            livetime[std::make_tuple(run,"edtm")] = Double_t(edtmTrigN) / Double_t(edtmScalerN);
            livetime[std::make_tuple(run,"cpu")]  = Double_t(physTrigN) / Double_t(physScalerN);
            livetime[std::make_tuple(run,"phys")] = Double_t(physTrigN-edtmTrigN) / Double_t(physScalerN-edtmScalerN);

            // ----------------------------------------------------------------
            // Cleanup
            // Deleting the TFile frees the memory taken up by any objects Get()-ed form it
            f->Close();
            delete f;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Print live time

    // Open file
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // Print header
    ofs << "kinematics"           << ","
        << "target"               << ","
        << "Q2"                   << ","
        << "collimator"           << ","
        << "run"                  << ","
        << "branch"               << ","
        << "description"          << ","
        << "count"
        << std::endl;

    // Loop over kinematics
    for (auto const &k : kinematics) {
        // Loop over runs
        for (auto const &run : data->GetRuns(k)) {

            // Print scaler counts
            for (auto const &scaler: scalers) {
                ofs << k                      << ","
                    << data->GetTarget(k)     << ","
                    << data->GetQ2(k)         << ","
                    << data->GetCollimator(k) << ","
                    << run                    << ","
                    << scaler                 << ","
                    << "scaler"               << ","
                    << scalerTotal[std::make_tuple(run,scaler)]
                    << std::endl;
            }

            // Print physics trigger counts
            for (auto const &trig: trigBranches) {
                ofs << k                      << ","
                    << data->GetTarget(k)     << ","
                    << data->GetQ2(k)         << ","
                    << data->GetCollimator(k) << ","
                    << run                    << ","
                    << trig                   << ","
                    << "!EDTM"                << ","
                    << acceptedTrigs[std::make_tuple(run, trig, "!EDTM")]
                    << std::endl;
            }

            // Print EDTM trigger counts
            ofs << k                      << ","
                << data->GetTarget(k)     << ","
                << data->GetQ2(k)         << ","
                << data->GetCollimator(k) << ","
                << run                    << ","
                << edtmBranch             << ","
                << "EDTM"                 << ","
                << acceptedTrigs[std::make_tuple(run, edtmBranch, "EDTM")]
                << std::endl;

            // Print livetime
            for (TString lt: {"phys","edtm","cpu"}) {
                ofs << k                      << ","
                    << data->GetTarget(k)     << ","
                    << data->GetQ2(k)         << ","
                    << data->GetCollimator(k) << ","
                    << run                    << ","
                    << lt                     << ","
                    << "livetime"             << ","
                    << scalerTotal[std::make_tuple(run,lt)]
                    << std::endl;
            }
        } // end loop over runs
    } // end loop over kinematics
    ofs.close();

    //-------------------------------------------------------------------------------------------------------------------------
    // Write histos to disk
    f = new TFile("histos.root","RECREATE");
    f->Write();

    //-------------------------------------------------------------------------------------------------------------------------
    // Print histos
    TH1F *histoOpen, *histoCut, *histoNoEDTM;
    TLine *windowMinMarker, *windowMaxMarker;
    TPaveLabel *text;
    TString pdfFilename;
    TString thisWindowLabel;
    TCanvas* canvas = new TCanvas("canvas", "compare", 700, 500);

    // Create one file for histos summed over all runs in a kinematic setting
    pdfFilename = "/home/jmatter/ct_scripts/analysis/livetime/histos_per_kinematics.pdf";
    canvas->Print((pdfFilename+"[").Data());
    for (auto const &branch: trigBranches) {
        for (auto const &k : kinematics) {
            histoOpen    = histosPerKinematics[std::make_tuple(k, branch, "open")];
            histoCut     = histosPerKinematics[std::make_tuple(k, branch, "cut")];
            histoNoEDTM  = histosPerKinematics[std::make_tuple(k, branch, "noedtm")];

            histoOpen->SetLineColor(kRed+2);
            histoCut->SetLineColor(kGreen+2);
            histoNoEDTM->SetLineColor(kBlue+2);

            histoOpen->SetFillColor(kRed+2);
            histoCut->SetFillColor(kGreen+2);
            histoNoEDTM->SetFillColor(kBlue+2);

            histoOpen->SetFillStyle(3345);
            histoCut->SetFillStyle(3354);
            histoNoEDTM->SetFillStyle(3305);

            histoOpen->Draw();
            histoCut->Draw("SAME");
            histoNoEDTM->Draw("SAME");

            // Add reference lines for cuts
            // min
            canvas->Update();
            windowMinMarker = new TLine(tdcTimeWindowMin[branch], canvas->GetUymin(),
                                        tdcTimeWindowMin[branch], canvas->GetUymax());
            windowMinMarker->SetLineColor(kGreen);
            windowMinMarker->SetLineStyle(7);
            windowMinMarker->Draw();
            // max
            windowMaxMarker = new TLine(tdcTimeWindowMax[branch], canvas->GetUymin(),
                                        tdcTimeWindowMax[branch], canvas->GetUymax());
            windowMaxMarker->SetLineColor(kGreen);
            windowMaxMarker->SetLineStyle(7);
            windowMaxMarker->Draw();

            // Label indicating window max/min
            thisWindowLabel = Form("tdc window: (%d,%d)", tdcTimeWindowMin[branch], tdcTimeWindowMax[branch]);
            text = new TPaveLabel(0.125, 0.64, 0.3, 0.68, thisWindowLabel.Data(), "brNDC");
            text->Draw();
            canvas->Modified();
            canvas->Update();

            gPad->Update();
            TPaveStats *st = (TPaveStats*)histoOpen->FindObject("stats");
            st->SetX1NDC(0.125);
            st->SetX2NDC(0.250);
            st->SetY1NDC(0.700);
            st->SetY2NDC(0.890);
            st->Draw();

            canvas->Print(pdfFilename.Data());

            delete windowMaxMarker;
            delete windowMinMarker;
        }
    }
    canvas->Print((pdfFilename+"]").Data());

    // Create one file with one page per run
    pdfFilename = "/home/jmatter/ct_scripts/analysis/livetime/histos_per_run.pdf";
    canvas->Print((pdfFilename+"[").Data());
    for (auto const &branch: trigBranches) {
        for (auto const &k : kinematics) {
            for (auto const &run : data->GetRuns(k)) {
                histoOpen   = histosPerRun[std::make_tuple(k, run, branch, "open")];
                histoCut    = histosPerRun[std::make_tuple(k, run, branch, "cut")];
                histoNoEDTM = histosPerRun[std::make_tuple(k, run, branch, "noedtm")];

                histoOpen->SetLineColor(kRed+2);
                histoCut->SetLineColor(kGreen+2);
                histoNoEDTM->SetLineColor(kBlue+2);

                histoOpen->SetFillColor(kRed+2);
                histoCut->SetFillColor(kGreen+2);
                histoNoEDTM->SetFillColor(kBlue+2);

                histoOpen->SetFillStyle(3345);
                histoCut->SetFillStyle(3354);
                histoNoEDTM->SetFillStyle(3305);

                histoOpen->Draw();
                histoCut->Draw("SAME");
                histoNoEDTM->Draw("SAME");

                // Add reference lines for cuts
                // min
                canvas->Update();
                windowMinMarker = new TLine(tdcTimeWindowMin[branch], canvas->GetUymin(),
                                            tdcTimeWindowMin[branch], canvas->GetUymax());
                windowMinMarker->SetLineColor(kGreen);
                windowMinMarker->SetLineStyle(7);
                windowMinMarker->Draw();
                // max
                windowMaxMarker = new TLine(tdcTimeWindowMax[branch], canvas->GetUymin(),
                                            tdcTimeWindowMax[branch], canvas->GetUymax());
                windowMaxMarker->SetLineColor(kGreen);
                windowMaxMarker->SetLineStyle(7);
                windowMaxMarker->Draw();

                // Label indicating window max/min
                thisWindowLabel = Form("tdc window: (%d,%d)", tdcTimeWindowMin[branch], tdcTimeWindowMax[branch]);
                text = new TPaveLabel(0.125, 0.64, 0.3, 0.68, thisWindowLabel.Data(), "brNDC");
                text->Draw();
                canvas->Modified();
                canvas->Update();

                gPad->Update();
                TPaveStats *st = (TPaveStats*) histoOpen->FindObject("stats");
                st->SetX1NDC(0.125);
                st->SetX2NDC(0.250);
                st->SetY1NDC(0.700);
                st->SetY2NDC(0.890);
                st->Draw();

                canvas->Print(pdfFilename.Data());

                delete windowMaxMarker;
                delete windowMinMarker;
            }
        }
    }
    canvas->Print((pdfFilename+"]").Data());
}
