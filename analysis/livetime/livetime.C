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
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // CSV to save
    TString csvFilename = "livetime.csv";

    // Needed for reading data
    TString rootFilename;
    TFile *f;
    TTree *T, *TS;

    // Store scaler reads and decoded data
    std::map<TString, Double_t> scalerTotalBCM4A;
    std::map<TString, Double_t> scalerTotalBCM4B;
    std::map<TString, Double_t> scalerRateBCM4A;
    std::map<TString, Double_t> scalerRateBCM4B;
    std::map<TString, Double_t> thisRead;
    std::map<TString, Double_t> prevRead;
    std::map<TString, Double_t> value;

    // Flags for whether or not event was above current threshold
    std::vector<Bool_t> eventFlagBCM4A;
    std::vector<Bool_t> eventFlagBCM4B;

    // Accepted triggers
    std::map<TString, Int_t> acceptedTrigsBCM4A;
    std::map<TString, Int_t> acceptedTrigsBCM4B;

    // Scaler read's upper limit
    std::vector<Int_t> scalerEventNumber;

    // Scalers to read
    std::vector<TString> scalers;
    TString bcm4A;
    TString bcm4B;
    TString timer;
    TString hodoTrig;
    TString hodoScaler;
    TString edtmScaler;
    if (spectrometer=="HMS") {
        scalers = {
                   "H.BCM4A.scalerCharge",
                   "H.BCM4A.scalerCurrent",
                   "H.BCM4B.scalerCharge",
                   "H.BCM4B.scalerCurrent",
                   "H.1MHz.scalerTime",
                   "H.S1X.scaler",    // S1X
                   "H.hTRIG1.scaler", // HMS 3/4
                   "H.hTRIG2.scaler", // HMS EL-REAL
                   "H.hTRIG3.scaler", // HMS EL-CLEAN
                   "H.hTRIG4.scaler", // HMS ?
                   "H.hTRIG5.scaler", // HMS ?
                   "H.hTRIG6.scaler", // HMS ?
                   "H.EDTM.scaler",   // HMS EDTM
                   "evNumber"
                  };
        bcm4A = "H.BCM4A.scalerCurrent";
        bcm4B = "H.BCM4B.scalerCurrent";
        timer = "H.1MHz.scalerTime";
        hodoTrig = "pTRIG1";
        hodoScaler = "P.pTRIG1.scaler";
        edtmScaler = "P.EDTM.scaler";
    }
    if (spectrometer=="SHMS") {
        scalers = {
                   "P.BCM4A.scalerCharge",
                   "P.BCM4A.scalerCurrent",
                   "P.BCM4B.scalerCharge",
                   "P.BCM4B.scalerCurrent",
                   "P.1MHz.scalerTime",
                   "P.S1X.scaler",    // S1X
                   "P.pTRIG1.scaler", // SHMS 3/4
                   "P.pTRIG2.scaler", // SHMS EL-REAL
                   "P.pTRIG3.scaler", // SHMS EL-CLEAN
                   "P.pTRIG4.scaler", // SHMS ?
                   "P.pTRIG5.scaler", // SHMS ?
                   "P.pTRIG6.scaler", // SHMS ?
                   "P.EDTM.scaler",   // SHMS EDTM
                   "evNumber"
                  };
        bcm4A = "P.BCM4A.scalerCurrent";
        bcm4B = "P.BCM4B.scalerCurrent";
        timer = "P.1MHz.scalerTime";
        hodoTrig = "hTRIG1";
        hodoScaler = "H.hTRIG1.scaler";
        edtmScaler = "H.EDTM.scaler";
    }

    // Current threshold for trips in uA
    // TODO: implement a "less than 90% of max" threshold
    Double_t currentThreshold = 10.0;

    // Data we need from T
    std::vector<TString> branches;
    branches = {
                // "g.evtyp",
                // "g.evnum",
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
                "T.coin.hEDTM_tdcTimeRaw",
                "T.coin.pEDTM_tdcTimeRaw",
                "H.cal.etottracknorm",
                "H.cer.npeSum",
                "P.ngcer.npeSum",
                "P.gtr.dp",
                "H.gtr.dp",
                "P.dc.ntrack",
                "H.dc.ntrack"
               };

    std::map<Int_t, Double_t> cpuLTBCM4A;
    std::map<Int_t, Double_t> trigLTBCM4A;
    std::map<Int_t, Int_t> nHodoTrigsBCM4A;
    std::map<Int_t, Int_t> nhEDTMTrigsBCM4A;
    std::map<Int_t, Int_t> npEDTMTrigsBCM4A;
    std::map<Int_t, Int_t> scalerCountHodoBCM4A;
    std::map<Int_t, Int_t> scalerCountEDTMBCM4A;
    std::map<Int_t, Double_t> cpuLTBCM4B;
    std::map<Int_t, Double_t> trigLTBCM4B;
    std::map<Int_t, Int_t> nHodoTrigsBCM4B;
    std::map<Int_t, Int_t> nhEDTMTrigsBCM4B;
    std::map<Int_t, Int_t> npEDTMTrigsBCM4B;
    std::map<Int_t, Int_t> scalerCountHodoBCM4B;
    std::map<Int_t, Int_t> scalerCountEDTMBCM4B;

    // ------------------------------------------------------------------------
    // Calculate live time
    for (auto const &k : kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            std::cout << "Run: " << run << "--------------------" << std::endl;

            // Initialize scalers, branches, counts, etc.
            for (auto const &scaler: scalers) {
                thisRead[scaler]  = 0;
                prevRead[scaler] = 0;
                scalerTotalBCM4A[scaler]  = 0;
                scalerTotalBCM4B[scaler]  = 0;
            }
            eventFlagBCM4A.clear();
            eventFlagBCM4B.clear();
            scalerEventNumber.clear();
            acceptedTrigsBCM4A["EDTM"]=0;
            acceptedTrigsBCM4A["hTRIG1"]=0;
            acceptedTrigsBCM4A["pTRIG1"]=0;

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

            // Set branch addresses for our scalers
            for (auto const &scaler: scalers) {
                TS->SetBranchAddress(scaler, &(thisRead[scaler]));
            }
            // Set branch addresses for our data
            for (auto const &branch: branches) {
                T->SetBranchAddress(branch, &(value[branch]));
            }

            // Loop over scalers and count triggers
            for (int i=0; i<TS->GetEntries(); i++) {
                if (i%200==0) {
                    std::cout << "Scaler progress: " << Double_t(i)/TS->GetEntries() << "%" << std::endl;
                }
                TS->GetEntry(i);

                // Push back the event upper limit for this scaler read
                scalerEventNumber.push_back(thisRead["evNumber"]);

                // If current above threshold, increment scalerTotal by an amount
                // equal to the difference between this read and the previous one
                // BCM4A
                eventFlagBCM4A.push_back(thisRead[bcm4A] > currentThreshold);
                if (eventFlagBCM4A[i]) {
                    for (auto const &scaler: scalers) {
                        scalerTotalBCM4A[scaler] += (thisRead[scaler] - prevRead[scaler]);
                    }
                }
                // BCM4B
                eventFlagBCM4B.push_back(thisRead[bcm4A] > currentThreshold);
                if (eventFlagBCM4B[i]) {
                    for (auto const &scaler: scalers) {
                        scalerTotalBCM4B[scaler] += (thisRead[scaler] - prevRead[scaler]);
                    }
                }
            }

            // Calculate rates
            for (auto const &scaler: scalers) {
                scalerRateBCM4A[scaler] = scalerTotalBCM4A[scaler] / scalerTotalBCM4A[timer];
                scalerRateBCM4B[scaler] = scalerTotalBCM4B[scaler] / scalerTotalBCM4B[timer];
            }

            // Loop over data and count triggers
            Bool_t nohEDTM, hEDTM, nopEDTM, pEDTM,
                   hTRIG1, hTRIG2, hTRIG3, hTRIG4, hTRIG5, hTRIG6,
                   pTRIG1, pTRIG2, pTRIG3, pTRIG4, pTRIG5, pTRIG6,
                   hcer, pcer, hcal, ptrack, htrack;
            // Need to keep track of scaler reads for current trips
            Int_t scalerRead = 0;
            for (int i=0; i<T->GetEntries(); i++) {
                if (i%500==0) {
                    std::cout << "Data progress: " << 100*Double_t(i)/T->GetEntries() << "%" << std::endl;
                }
                // Cuts
                nohEDTM  = value["T.coin.hEDTM_tdcTimeRaw"]==0;
                hEDTM    = value["T.coin.hEDTM_tdcTimeRaw"]>0;
                nopEDTM  = value["T.coin.pEDTM_tdcTimeRaw"]==0;
                pEDTM    = value["T.coin.pEDTM_tdcTimeRaw"]>0;
                hTRIG1  = value["T.coin.hTRIG1_tdcTimeRaw"]>0;
                pTRIG1  = value["T.coin.pTRIG1_tdcTimeRaw"]>0;
                hcer    = value["H.cer.npeSum"] > 0.0;
                pcer    = value["P.ngcer.npeSum"] > 0.0;
                hcal    = value["H.cal.etottracknorm"] > 0.0;
                htrack  = value["H.dc.ntrack"] > 0;
                ptrack  = value["P.dc.ntrack"] > 0;

                // Increment counts only if current not tripped
                // BCM4A
                if (eventFlagBCM4A[scalerRead]) {
                    if (hEDTM) {
                        acceptedTrigsBCM4A["hEDTM"]++;
                    }
                    if (pEDTM) {
                        acceptedTrigsBCM4A["pEDTM"]++;
                    }
                    if (hTRIG1 && nohEDTM && nopEDTM) {
                        acceptedTrigsBCM4A["hTRIG1"]++;
                    }
                    if (pTRIG1 && nohEDTM && nopEDTM) {
                        acceptedTrigsBCM4A["pTRIG1"]++;
                    }
                }
                // BCM4B
                if (eventFlagBCM4B[scalerRead]) {
                    if (hEDTM) {
                        acceptedTrigsBCM4B["hEDTM"]++;
                    }
                    if (pEDTM) {
                        acceptedTrigsBCM4B["pEDTM"]++;
                    }
                    if (hTRIG1 && nohEDTM && nopEDTM) {
                        acceptedTrigsBCM4B["hTRIG1"]++;
                    }
                    if (pTRIG1 && nohEDTM && nopEDTM) {
                        acceptedTrigsBCM4B["pTRIG1"]++;
                    }
                }

                // scalerRead should be incremented if we've reached it
                // if (value["g.evnum"] >= scalerEventNumber[scalerRead]) {
                // If I'm not wrong, g.evnum == (i+1), so we'll do this instead
                if ((i+1) >= scalerEventNumber[scalerRead]) {
                    scalerRead++;
                }
            }

            // The Goal! ------------------------
            // Calculate live time
            // BCM4A
            nHodoTrigsBCM4A[run] = acceptedTrigsBCM4A[hodoTrig];
            nhEDTMTrigsBCM4A[run] = acceptedTrigsBCM4A["hEDTM"];
            npEDTMTrigsBCM4A[run] = acceptedTrigsBCM4A["pEDTM"];
            scalerCountHodoBCM4A[run] = scalerTotalBCM4A[hodoScaler];
            scalerCountEDTMBCM4A[run] = scalerTotalBCM4A[edtmScaler];
            cpuLTBCM4A[run]  = acceptedTrigsBCM4A[hodoTrig] / (scalerTotalBCM4A[hodoScaler] - scalerTotalBCM4A[edtmScaler]);
            trigLTBCM4A[run] = acceptedTrigsBCM4A["EDTM"] / scalerTotalBCM4A[edtmScaler];
            // BCM4B
            nHodoTrigsBCM4B[run] = acceptedTrigsBCM4B[hodoTrig];
            nhEDTMTrigsBCM4B[run] = acceptedTrigsBCM4B["hEDTM"];
            npEDTMTrigsBCM4B[run] = acceptedTrigsBCM4B["pEDTM"];
            scalerCountHodoBCM4B[run] = scalerTotalBCM4B[hodoScaler];
            scalerCountEDTMBCM4B[run] = scalerTotalBCM4B[edtmScaler];
            cpuLTBCM4B[run]  = acceptedTrigsBCM4B[hodoTrig] / (scalerTotalBCM4B[hodoScaler] - scalerTotalBCM4B[edtmScaler]);
            trigLTBCM4B[run] = acceptedTrigsBCM4B[hodoTrig] / scalerTotalBCM4B[edtmScaler];

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
    ofs << "kinematics,target,Q2,collimator,run,spectrometer,"
        << "nHodoTrigsBCM4A,nhEDTMTrigsBCM4A,npEDTMTrigsBCM4A,"
        << "scalerCountHodoBCM4A,scalerCountEDTMBCM4A,cpuLTBCM4A,"
        << "nHodoTrigsBCM4B,nhEDTMTrigsBCM4B,npEDTMTrigsBCM4B,"
        << "scalerCountHodoBCM4B,scalerCountEDTMBCM4B,cpuLTBCM4B"
        << std::endl;

    // Loop over kinematics
    for (auto const &k : kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            ofs        << k
                << "," << data->GetTarget(k)
                << "," << data->GetQ2(k)
                << "," << data->GetCollimator(k)
                << "," << spectrometer
                << "," << run
                // BCM4A
                << "," << nHodoTrigsBCM4A[run]
                << "," << nhEDTMTrigsBCM4A[run]
                << "," << npEDTMTrigsBCM4A[run]
                << "," << scalerCountHodoBCM4A[run]
                << "," << scalerCountEDTMBCM4A[run]
                << "," << cpuLTBCM4A[run]
                << "," << trigLTBCM4A[run]
                // BCM4B
                << "," << nHodoTrigsBCM4B[run]
                << "," << nhEDTMTrigsBCM4B[run]
                << "," << npEDTMTrigsBCM4B[run]
                << "," << scalerCountHodoBCM4B[run]
                << "," << scalerCountEDTMBCM4B[run]
                << "," << cpuLTBCM4B[run]
                << "," << trigLTBCM4B[run]
                << std::endl;
        }
    }
    ofs.close();
}
