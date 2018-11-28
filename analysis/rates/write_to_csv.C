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
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

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
    Double_t bcm1;
    Double_t bcm2;
    Double_t bcm4a;
    Double_t bcm4b;
    Double_t bcm17;
    Double_t unser;
    Double_t hTRIG1;
    Double_t pTRIG1;

    std::ofstream csv;
    csv.open(csvFilename.Data());
    csv << "kinematics"
        << "," << "Q2"
        << "," << "target"
        << "," << "run"
        << "," << "i"
        << "," << "bcm1"
        << "," << "bcm2"
        << "," << "bcm4a"
        << "," << "bcm4b"
        << "," << "bcm17"
        << "," << "unser"
        << "," << "hTRIG1"
        << "," << "pTRIG1"
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
            TTree *scalerTree = (TTree*) f->Get("TSP");

            std::cout<<"set branches for run " << run;
            scalerTree->SetBranchAddress("P.BCM1.scalerCurrent",  &bcm1);
            scalerTree->SetBranchAddress("P.BCM2.scalerCurrent",  &bcm2);
            scalerTree->SetBranchAddress("P.BCM4A.scalerCurrent", &bcm4a);
            scalerTree->SetBranchAddress("P.BCM4B.scalerCurrent", &bcm4b);
            scalerTree->SetBranchAddress("P.BCM17.scalerCurrent", &bcm17);
            scalerTree->SetBranchAddress("P.Unser.scalerCurrent", &unser);
            scalerTree->SetBranchAddress("P.hTRIG1.scalerRate",   &hTRIG1);
            scalerTree->SetBranchAddress("P.pTRIG1.scalerRate",   &pTRIG1);
            std::cout<<"✓"<<std::endl;

            // Print branches to csv for debugging
            Int_t N = scalerTree->GetEntries();
            for (int i=0; i<N; i++) {
                scalerTree->GetEntry(i);

                csv << k
                    << "," << Q2
                    << "," << target
                    << "," << run
                    << "," << i
                    << "," << bcm1
                    << "," << bcm2
                    << "," << bcm4a
                    << "," << bcm4b
                    << "," << bcm17
                    << "," << unser
                    << "," << hTRIG1
                    << "," << pTRIG1
                    << std::endl;
            }

            delete f;
        }
    }
    csv.close();
}
