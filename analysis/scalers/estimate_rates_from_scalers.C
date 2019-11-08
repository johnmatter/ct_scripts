#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TEventList.h>

#include <CTData.h>
#include <CTCuts.h>

// Estimate singles and coincidence rates from scaler data
void estimate_rates_from_scalers() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

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
    // Variables we want from the trees

    Double_t bcm4Achargecut;
    Double_t bcm4Bchargecut;
    Double_t bcm4Cchargecut;
    Double_t pTRIG1;
    Double_t pTRIG4;
    Double_t pTRIG6;
    Double_t pEDTM;
    Double_t hEDTM;
    Double_t p1MHzScalerTime;
    Int_t N;

    // ------------------------------------------------------------------------
    // Loop over files and print all entries in the tree

    std::cout << "run,"
              << "target,"
              << "collimator,"
              << "Q2,"
              << "bcm4Achargecut,"
              << "bcm4Bchargecut,"
              << "bcm4Cchargecut,"
              << "pTRIG1,"
              << "pTRIG4,"
              << "pTRIG6,"
              << "pEDTM,"
              << "hEDTM,"
              << "p1MHzScalerTime"
              << std::endl;

    for (auto const &k : kinematics) {

        std::vector<Int_t> runs = data->GetRuns(k);
        std::vector<TFile*> files = rootfiles[k];

        Double_t Q2 = data->GetQ2(k);
        TString target = data->GetTarget(k);
        TString collimator = data->GetCollimator(k);

        for (int n=0; n<runs.size(); n++) {
            Int_t run = runs[n];

            TFile *f = files[n];
            TTree *TSP = (TTree*) f->Get("TSP");
            TTree *TSH = (TTree*) f->Get("TSH");

            TSP->SetBranchAddress("P.BCM4A.scalerChargeCut",  &bcm4Achargecut);
            TSP->SetBranchAddress("P.BCM4B.scalerChargeCut",  &bcm4Bchargecut);
            TSP->SetBranchAddress("P.BCM4C.scalerChargeCut",  &bcm4Cchargecut);
            TSP->SetBranchAddress("P.pTRIG1.scalerCut",       &pTRIG1);
            TSP->SetBranchAddress("P.pTRIG4.scalerCut",       &pTRIG4);
            TSP->SetBranchAddress("P.pTRIG6.scalerCut",       &pTRIG6);
            TSP->SetBranchAddress("P.EDTM.scalerCut",         &pEDTM);
            TSH->SetBranchAddress("H.EDTM.scalerCut",         &hEDTM);
            TSP->SetBranchAddress("P.1MHz.scalerTimeCut",     &p1MHzScalerTime);

            N = TSP->GetEntries();
            TSP->GetEntry(N-1);
            N = TSH->GetEntries();
            TSH->GetEntry(N-1);

            std::cout << Form("%d,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                                run,
                                target.Data(),
                                collimator.Data(),
                                Q2,
                                bcm4Achargecut,
                                bcm4Bchargecut,
                                bcm4Cchargecut,
                                pTRIG1,
                                pTRIG4,
                                pTRIG6,
                                pEDTM,
                                hEDTM,
                                p1MHzScalerTime
                             )
                      << std::endl;

            delete f;
        }
    }
}
