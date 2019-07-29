#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TEventList.h>

#include <CTData.h>
#include <CTCuts.h>

// Print bcm currents, charges, and pTRIG1 rate
void write_to_csv() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();
    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
    //                                    "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Where do we save the debug data?
    TString csvFilename = "/home/jmatter/work_jmatter/csv/ct_rates.csv";

    // key is kinematics
    // value is a vector of that kinematics' rootfiles
    std::map<TString, std::vector<TString>> rootfilenames;
    std::map<TString, std::vector<TFile*>>  rootfiles;

    // Where are the rootfiles?
    TString rootDirectory = data->GetRootfileDirectory();

    // ------------------------------------------------------------------------
    // Get lists of files
    for (auto const &k : kinematics) {
        // Get runs
        std::vector<Int_t> runs = data->GetRuns(k);

        // Get rootfile template
        TString rootTemplate = data->GetRootfileTemplate(k.Data());

        TString filename;
        for (auto const &r : runs) {
            filename = Form(rootTemplate, rootDirectory.Data(), r);
            rootfilenames[k].push_back(filename);

            TFile *f= new TFile(filename.Data(),"READ");
            rootfiles[k].push_back(f);
        }
    }

    // ------------------------------------------------------------------------
    // Loop over files and print all entries in the tree
    Double_t bcm1current,  bcm1charge,  bcm1chargecut;
    Double_t bcm2current,  bcm2charge,  bcm2chargecut;
    Double_t bcm4Acurrent, bcm4Acharge, bcm4Achargecut;
    Double_t bcm4Bcurrent, bcm4Bcharge, bcm4Bchargecut;
    Double_t bcm4Ccurrent, bcm4Ccharge, bcm4Cchargecut;
    Double_t Unsercurrent, Unsercharge, Unserchargecut;
    Double_t pTRIG1rate, pTRIG2rate, pTRIG3rate, pTRIG4rate, pTRIG5rate, pTRIG6rate;
    Double_t hTRIG1rate, hTRIG2rate, hTRIG3rate, hTRIG4rate, hTRIG5rate, hTRIG6rate;
    Double_t pS1Xrate, pS1Yrate, pS1XS1Yrate;
    Double_t hS1Xrate, hS1Yrate, hS1XS1Yrate;
    Double_t pTRIG1, pTRIG2, pTRIG3, pTRIG4, pTRIG5, pTRIG6;
    Double_t hTRIG1, hTRIG2, hTRIG3, hTRIG4, hTRIG5, hTRIG6;
    Double_t pS1X, pS1Y, pS1XS1Y;
    Double_t hS1X, hS1Y, hS1XS1Y;
    Double_t p1MHzScalerTime, h1MHzScalerTime;

    std::ofstream csv;
    csv.open(csvFilename.Data());
    csv << "kinematics"
        << "," << "Q2"
        << "," << "target"
        << "," << "run"
        << "," << "i"
        << "," << "bcm1current"
        << "," << "bcm1charge"
        << "," << "bcm1chargecut"
        << "," << "bcm2current"
        << "," << "bcm2charge"
        << "," << "bcm2chargecut"
        << "," << "bcm4Acurrent"
        << "," << "bcm4Acharge"
        << "," << "bcm4Achargecut"
        << "," << "bcm4Bcurrent"
        << "," << "bcm4Bcharge"
        << "," << "bcm4Bchargecut"
        << "," << "bcm4Ccurrent"
        << "," << "bcm4Ccharge"
        << "," << "bcm4Cchargecut"
        << "," << "Unsercurrent"
        << "," << "Unsercharge"
        << "," << "Unserchargecut"
        << "," << "pTRIG1rate"
        << "," << "pTRIG2rate"
        << "," << "pTRIG3rate"
        << "," << "pTRIG4rate"
        << "," << "pTRIG5rate"
        << "," << "pTRIG6rate"
        << "," << "hTRIG1rate"
        << "," << "hTRIG2rate"
        << "," << "hTRIG3rate"
        << "," << "hTRIG4rate"
        << "," << "hTRIG5rate"
        << "," << "hTRIG6rate"
        << "," << "PS1Xrate"
        << "," << "PS1Yrate"
        << "," << "PS1XS1Yrate"
        << "," << "HS1Xrate"
        << "," << "HS1Yrate"
        << "," << "HS1XS1Yrate"
        << "," << "pTRIG1"
        << "," << "pTRIG2"
        << "," << "pTRIG3"
        << "," << "pTRIG4"
        << "," << "pTRIG5"
        << "," << "pTRIG6"
        << "," << "hTRIG1"
        << "," << "hTRIG2"
        << "," << "hTRIG3"
        << "," << "hTRIG4"
        << "," << "hTRIG5"
        << "," << "hTRIG6"
        << "," << "PS1X"
        << "," << "PS1Y"
        << "," << "PS1XS1Y"
        << "," << "HS1X"
        << "," << "HS1Y"
        << "," << "HS1XS1Y"
        << "," << "P1MhzScalerTime"
        << "," << "H1MhzScalerTime"
        << std::endl;

    std::cout << "Get charge---------------------" << std::endl;
    for (auto const &k : kinematics) {
        std::cout << k << std::endl;

        std::vector<Int_t> runs = data->GetRuns(k);
        std::vector<TFile*> files = rootfiles[k];

        Double_t Q2 = data->GetQ2(k);
        TString target = data->GetTarget(k);

        for (int n=0; n<runs.size(); n++) {
            Int_t run = runs[n];

            TFile *f = files[n];
            TTree *TSP = (TTree*) f->Get("TSP");
            TTree *TSH = (TTree*) f->Get("TSH");

            std::cout<<"set branches for run " << run;
            TSP->SetBranchAddress("P.BCM1.scalerCurrent",    &bcm1current);
            TSP->SetBranchAddress("P.BCM1.scalerCharge",     &bcm1charge);
            TSP->SetBranchAddress("P.BCM1.scalerChargeCut",  &bcm1chargecut);

            TSP->SetBranchAddress("P.BCM2.scalerCurrent",    &bcm2current);
            TSP->SetBranchAddress("P.BCM2.scalerCharge",     &bcm2charge);
            TSP->SetBranchAddress("P.BCM2.scalerChargeCut",  &bcm2chargecut);

            TSP->SetBranchAddress("P.BCM4A.scalerCurrent",    &bcm4Acurrent);
            TSP->SetBranchAddress("P.BCM4A.scalerCharge",     &bcm4Acharge);
            TSP->SetBranchAddress("P.BCM4A.scalerChargeCut",  &bcm4Achargecut);

            TSP->SetBranchAddress("P.BCM4B.scalerCurrent",    &bcm4Bcurrent);
            TSP->SetBranchAddress("P.BCM4B.scalerCharge",     &bcm4Bcharge);
            TSP->SetBranchAddress("P.BCM4B.scalerChargeCut",  &bcm4Bchargecut);

            TSP->SetBranchAddress("P.BCM4C.scalerCurrent",    &bcm4Ccurrent);
            TSP->SetBranchAddress("P.BCM4C.scalerCharge",     &bcm4Ccharge);
            TSP->SetBranchAddress("P.BCM4C.scalerChargeCut",  &bcm4Cchargecut);

            TSP->SetBranchAddress("P.Unser.scalerCurrent",    &Unsercurrent);
            TSP->SetBranchAddress("P.Unser.scalerCharge",     &Unsercharge);
            TSP->SetBranchAddress("P.Unser.scalerChargeCut",  &Unserchargecut);

            TSP->SetBranchAddress("P.pTRIG1.scalerRate",   &pTRIG1rate);
            TSP->SetBranchAddress("P.pTRIG2.scalerRate",   &pTRIG2rate);
            TSP->SetBranchAddress("P.pTRIG3.scalerRate",   &pTRIG3rate);
            TSP->SetBranchAddress("P.pTRIG4.scalerRate",   &pTRIG4rate);
            TSP->SetBranchAddress("P.pTRIG5.scalerRate",   &pTRIG5rate);
            TSP->SetBranchAddress("P.pTRIG6.scalerRate",   &pTRIG6rate);

            TSP->SetBranchAddress("P.hTRIG1.scalerRate",   &hTRIG1rate);
            TSP->SetBranchAddress("P.hTRIG2.scalerRate",   &hTRIG2rate);
            TSP->SetBranchAddress("P.hTRIG3.scalerRate",   &hTRIG3rate);
            TSP->SetBranchAddress("P.hTRIG4.scalerRate",   &hTRIG4rate);
            TSP->SetBranchAddress("P.hTRIG5.scalerRate",   &hTRIG5rate);
            TSP->SetBranchAddress("P.hTRIG6.scalerRate",   &hTRIG6rate);

            TSP->SetBranchAddress("P.S1X.scalerRate",    &pS1Xrate);
            TSP->SetBranchAddress("P.S1Y.scalerRate",    &pS1Yrate);
            TSP->SetBranchAddress("P.S1XS1Y.scalerRate", &pS1XS1Yrate);

            TSH->SetBranchAddress("H.S1X.scalerRate",    &hS1Xrate);
            TSH->SetBranchAddress("H.S1Y.scalerRate",    &hS1Yrate);
            TSH->SetBranchAddress("H.S1XS1Y.scalerRate", &hS1XS1Yrate);

            TSP->SetBranchAddress("P.pTRIG1.scaler",   &pTRIG1);
            TSP->SetBranchAddress("P.pTRIG2.scaler",   &pTRIG2);
            TSP->SetBranchAddress("P.pTRIG3.scaler",   &pTRIG3);
            TSP->SetBranchAddress("P.pTRIG4.scaler",   &pTRIG4);
            TSP->SetBranchAddress("P.pTRIG5.scaler",   &pTRIG5);
            TSP->SetBranchAddress("P.pTRIG6.scaler",   &pTRIG6);

            TSP->SetBranchAddress("P.hTRIG1.scaler",   &hTRIG1);
            TSP->SetBranchAddress("P.hTRIG2.scaler",   &hTRIG2);
            TSP->SetBranchAddress("P.hTRIG3.scaler",   &hTRIG3);
            TSP->SetBranchAddress("P.hTRIG4.scaler",   &hTRIG4);
            TSP->SetBranchAddress("P.hTRIG5.scaler",   &hTRIG5);
            TSP->SetBranchAddress("P.hTRIG6.scaler",   &hTRIG6);

            TSP->SetBranchAddress("P.S1X.scaler",    &pS1X);
            TSP->SetBranchAddress("P.S1Y.scaler",    &pS1Y);
            TSP->SetBranchAddress("P.S1XS1Y.scaler", &pS1XS1Y);

            TSH->SetBranchAddress("H.S1X.scaler",    &hS1X);
            TSH->SetBranchAddress("H.S1Y.scaler",    &hS1Y);
            TSH->SetBranchAddress("H.S1XS1Y.scaler", &hS1XS1Y);

            TSP->SetBranchAddress("P.1MHz.scalerTime", &p1MHzScalerTime);
            TSH->SetBranchAddress("H.1MHz.scalerTime", &h1MHzScalerTime);

            std::cout<<"✓"<<std::endl;

            // Print branches to csv for debugging
            Int_t N = TSP->GetEntries();
            for (int i=0; i<N; i++) {
                TSP->GetEntry(i);

                csv << k
                    << "," << Q2
                    << "," << target
                    << "," << run
                    << "," << i
                    << "," << bcm1current
                    << "," << bcm1charge
                    << "," << bcm1chargecut
                    << "," << bcm2current
                    << "," << bcm2charge
                    << "," << bcm2chargecut
                    << "," << bcm4Acurrent
                    << "," << bcm4Acharge
                    << "," << bcm4Achargecut
                    << "," << bcm4Bcurrent
                    << "," << bcm4Bcharge
                    << "," << bcm4Bchargecut
                    << "," << bcm4Ccurrent
                    << "," << bcm4Ccharge
                    << "," << bcm4Cchargecut
                    << "," << Unsercurrent
                    << "," << Unsercharge
                    << "," << Unserchargecut
                    << "," << pTRIG1rate
                    << "," << pTRIG2rate
                    << "," << pTRIG3rate
                    << "," << pTRIG4rate
                    << "," << pTRIG5rate
                    << "," << pTRIG6rate
                    << "," << hTRIG1rate
                    << "," << hTRIG2rate
                    << "," << hTRIG3rate
                    << "," << hTRIG4rate
                    << "," << hTRIG5rate
                    << "," << hTRIG6rate
                    << "," << pS1Xrate
                    << "," << pS1Yrate
                    << "," << pS1XS1Yrate
                    << "," << hS1Xrate
                    << "," << hS1Yrate
                    << "," << hS1XS1Yrate
                    << "," << pTRIG1
                    << "," << pTRIG2
                    << "," << pTRIG3
                    << "," << pTRIG4
                    << "," << pTRIG5
                    << "," << pTRIG6
                    << "," << hTRIG1
                    << "," << hTRIG2
                    << "," << hTRIG3
                    << "," << hTRIG4
                    << "," << hTRIG5
                    << "," << hTRIG6
                    << "," << pS1X
                    << "," << pS1Y
                    << "," << pS1XS1Y
                    << "," << hS1X
                    << "," << hS1Y
                    << "," << hS1XS1Y
                    << "," << p1MHzScalerTime
                    << "," << h1MHzScalerTime
                    << std::endl;
            }

            delete f;
        }
    }
    csv.close();
}
