#include <utility>
#include <fstream>
#include <map>

#include <TCut.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>

#include <CTData.h>

// This calculates livetime for SHMS and HMS.
// Big thanks to Carlos Yero for the inspiration for this script.
void livetime(TString spectrometer) {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // CSV to save
    TString csvFilename = Form("livetime_%s.csv", spectrometer.Data());

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

    // Scaler read's upper limit
    std::vector<Int_t> scalerEventNumber;

    // Scalers to read
    std::vector<TString> scalers;
    TString bcm, bcmQ;
    TString timer;
    TString hodoTrig;
    TString hodoScaler;
    TString edtmScaler;
    if (spectrometer=="HMS") {
        bcm = "H.BCM4A.scalerCurrent";
        bcmQ = "H.BCM4A.scalerCharge",
        timer = "H.1MHz.scalerTime";
        hodoTrig = "hTRIG1";
        hodoScaler = "H.hTRIG1.scaler";
        edtmScaler = "H.EDTM.scaler";
        scalers = {
                   "H.S1X.scaler",    // S1X
                   "H.hTRIG1.scaler", // HMS 3/4
                   "H.hTRIG2.scaler", // HMS EL-REAL
                   "H.hTRIG3.scaler", // HMS EL-CLEAN
                   "H.hTRIG4.scaler", // HMS ?
                   "H.hTRIG5.scaler", // HMS ?
                   "H.hTRIG6.scaler", // HMS ?
                   "H.EDTM.scaler",   // HMS EDTM
                   bcm,
                   bcmQ,
                   timer,
                   "evNumber"
                  };
    }
    if (spectrometer=="SHMS") {
        bcm = "P.BCM4A.scalerCurrent";
        bcmQ = "P.BCM4A.scalerCharge",
        timer = "P.1MHz.scalerTime";
        hodoTrig = "pTRIG1";
        hodoScaler = "P.pTRIG1.scaler";
        edtmScaler = "P.EDTM.scaler";
        scalers = {
                   "P.S1X.scaler",    // S1X
                   "P.pTRIG1.scaler", // SHMS 3/4
                   "P.pTRIG2.scaler", // SHMS EL-REAL
                   "P.pTRIG3.scaler", // SHMS EL-CLEAN
                   "P.pTRIG4.scaler", // SHMS ?
                   "P.pTRIG5.scaler", // SHMS ?
                   "P.pTRIG6.scaler", // SHMS ?
                   "P.EDTM.scaler",   // SHMS EDTM
                   bcm,
                   bcmQ,
                   timer,
                   "evNumber"
                  };
    }

    // Current threshold for trips in uA
    // TODO: implement a "less than 90% of max" threshold
    Double_t currentThreshold = 10.0;

    // Data we need from T
    std::vector<TString> trigBranches;
    trigBranches = {
                "T.coin.hTRIG1_ROC1_tdcTimeRaw",
                "T.coin.hTRIG2_ROC1_tdcTimeRaw",
                "T.coin.hTRIG3_ROC1_tdcTimeRaw",
                "T.coin.hTRIG4_ROC1_tdcTimeRaw",
                "T.coin.hTRIG5_ROC1_tdcTimeRaw",
                "T.coin.hTRIG6_ROC1_tdcTimeRaw",
                "T.coin.pTRIG1_ROC1_tdcTimeRaw",
                "T.coin.pTRIG2_ROC1_tdcTimeRaw",
                "T.coin.pTRIG3_ROC1_tdcTimeRaw",
                "T.coin.pTRIG4_ROC1_tdcTimeRaw",
                "T.coin.pTRIG5_ROC1_tdcTimeRaw",
                "T.coin.pTRIG6_ROC1_tdcTimeRaw",
                "T.coin.hTRIG1_ROC2_tdcTimeRaw",
                "T.coin.hTRIG2_ROC2_tdcTimeRaw",
                "T.coin.hTRIG3_ROC2_tdcTimeRaw",
                "T.coin.hTRIG4_ROC2_tdcTimeRaw",
                "T.coin.hTRIG5_ROC2_tdcTimeRaw",
                "T.coin.hTRIG6_ROC2_tdcTimeRaw",
                "T.coin.pTRIG1_ROC2_tdcTimeRaw",
                "T.coin.pTRIG2_ROC2_tdcTimeRaw",
                "T.coin.pTRIG3_ROC2_tdcTimeRaw",
                "T.coin.pTRIG4_ROC2_tdcTimeRaw",
                "T.coin.pTRIG5_ROC2_tdcTimeRaw",
                "T.coin.pTRIG6_ROC2_tdcTimeRaw",
                };

    std::vector<TString> edtmBranches;
    edtmBranches = {
                "T.coin.hEDTM_tdcTimeRaw",
                "T.coin.pEDTM_tdcTimeRaw"
                };

    std::vector<TString> dataBranches;
    dataBranches = {
                // "g.evtyp",
                // "g.evnum",
                "H.cal.etottracknorm",
                "H.cer.npeSum",
                "P.ngcer.npeSum",
                "P.gtr.dp",
                "H.gtr.dp",
                "P.dc.ntrack",
                "H.dc.ntrack"
               };

    // ------------------------------------------------------------------------
    // Count triggers, calculate livetime
    for (auto const &k : kinematics) {
        // We didn't have EDTM for Q^2=8 GeV^2 data, so skip for now
        // There are ways to do this without the EDTM, but I don't have it
        // implemented in this script yet.
        if (data->GetQ2(k)==8) {
            continue;
        }

        for (auto const &run : data->GetRuns(k)) {
            std::cout << "Run: " << run << "--------------------" << std::endl;

            // ----------------------------------------------------------------
            // Bookkeeping

            // Initialize scalers, branches, counts, etc.
            for (auto const &scaler: scalers) {
                thisRead[scaler] = 0;
                prevRead[scaler] = 0;
                scalerTotal[std::make_tuple(run,scaler)] = 0;
                for (std::map<tuple<Int_t, TString, TString>, Int_t>::iterator it=acceptedTrigs.begin(); it!=acceptedTrigs.end(); it++) {
                    acceptedTrigs[it->first] = 0;
                }
            }
            eventFlag.clear();
            scalerEventNumber.clear();

            // Get trees for this run from the rootfile
            rootFilename = Form(data->GetRootfileTemplate(k),
                                data->GetRootfileDirectory().Data(),
                                run);
            f  = new TFile(rootFilename.Data(), "READ");
            T  = (TTree*) f->Get("T");
            if (spectrometer=="HMS") {
                TS = (TTree*) f->Get("TSH");
            }
            if (spectrometer=="SHMS") {
                TS = (TTree*) f->Get("TSP");
            }

            // Set branch addresses
            for (auto const &scaler: scalers) {
                TS->SetBranchAddress(scaler, &(thisRead[scaler]));
            }
            for (auto const &branch: trigBranches) {
                T->SetBranchAddress(branch, &(value[branch]));
            }
            for (auto const &branch: edtmBranches) {
                T->SetBranchAddress(branch, &(value[branch]));
            }
            for (auto const &branch: dataBranches) {
                T->SetBranchAddress(branch, &(value[branch]));
            }

            // ----------------------------------------------------------------
            // Loop over scalers and count triggers
            for (int i=0; i<TS->GetEntries(); i++) {
                if (i%1000==0) {
                    std::cout << "Scaler progress: " << 100*Double_t(i)/TS->GetEntries() << "%" << std::endl;
                }
                TS->GetEntry(i);

                // Push back the event upper limit for this scaler read
                scalerEventNumber.push_back(thisRead["evNumber"]);

                // If current above threshold, increment scalerTotal by an amount
                // equal to the difference between this read and the previous one
                eventFlag.push_back(thisRead[bcm] > currentThreshold);
                if (eventFlag[i]) {
                    for (auto const &scaler: scalers) {
                        scalerTotal[std::make_tuple(run,scaler)] += (thisRead[scaler] - prevRead[scaler]);
                    }
                }
            }
            std::cout << "Scaler progress: 100%" << std::endl;

            // ----------------------------------------------------------------
            // Loop over data and count triggers.
            // Need to keep track of scaler reads for current trips
            Int_t scalerRead = 0;
            TString description;
            for (int i=0; i<T->GetEntries(); i++) {
                if (i%2000==0) {
                    std::cout << "Data progress: " << 100*Double_t(i)/T->GetEntries() << "%" << std::endl;
                }
                T->GetEntry(i);

                // Increment counts only if current not tripped
                if (eventFlag[scalerRead]) {
                    for (auto const &edtm: edtmBranches) {
                        // EDTM
                        if (value[edtm]>0) {
                            description = "accepted EDTM";
                            acceptedTrigs[std::make_tuple(run, edtm, description)]++;
                        }

                        // trig and !EDTM
                        for (auto const &trig: trigBranches) {
                            if (value[trig]>0 && value[edtm]==0) {
                                description = Form("!%s", edtm.Data());
                                acceptedTrigs[std::make_tuple(run, trig, description)]++;
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
            // cpuLT[run]  = acceptedTrigs[hodoTrig] / (scalerTotal[hodoScaler] - scalerTotal[edtmScaler]);
            // trigLT[run] = acceptedTrigs[hodoTrig] / scalerTotal[edtmScaler];

            // ----------------------------------------------------------------
            // Cleanup
            // Deleting the TFile frees the memory taken up by any objects Get()-ed form it
            f->Close();
            delete f;
        }
    }

    // ------------------------------------------------------------------------
    // Print live time

    // Open file
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // Print header

    // Loop over kinematics
    for (auto const &k : kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            // Print scaler counts
            for (auto const &scaler: scalers) {
                // Examples:
                // LH2_Q2_8,LH2,8,pion,H.hTRIG1.scaler,scaler,3287568287652
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
        }
        // Print accepted trigger counts
        // This is outside the loop over runs because I used "description" and run as part of the tuple key
        // It was easier in this moment to just loop using an iterator (which contains run).
        // TODO: fix weird acceptedTrigs map
        for (std::map<tuple<Int_t, TString, TString>, Int_t>::iterator it=acceptedTrigs.begin(); it!=acceptedTrigs.end(); it++) {
            // Examples:
            // LH2_Q2_8,LH2,8,pion,T.coin.hTRIG1_ROC2_tdcTimeRaw,!hEDTM,3568287652
            // LH2_Q2_8,LH2,8,pion,T.coin.hTRIG1_ROC2_tdcTimeRaw,!hEDTM,3568287652
            acceptedTrigs[it->first] = 0;
            ofs << k                      << ","
                << data->GetTarget(k)     << ","
                << data->GetQ2(k)         << ","
                << data->GetCollimator(k) << ","
                << std::get<0>(it->first) << ","
                << std::get<1>(it->first) << ","
                << std::get<2>(it->first) << ","
                << acceptedTrigs[it->first]
                << std::endl;
        }
    }
    ofs.close();
}
