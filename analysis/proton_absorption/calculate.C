#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TEventList.h>

#include <CTData.h>
#include <CTCuts.h>

void calculate() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14"};
    // std::vector<TString> kinematics = {"C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12"};
    // std::vector<TString> kinematics = {"LH2_Q2_14"};

    // Cuts for determining good events
    std::map<TString, TCut> goodEventCut;
    goodEventCut["LH2"] = cuts->Get("coinCutsLH2");
    goodEventCut["C12"] = cuts->Get("coinCutsC12");

    // bcm names
    std::vector<TString> bcms = {"bcm1", "bcm2", "bcm4a", "bcm4b", "bcm17", "unser"};

    // Where do we save the debug data?
    TString csvFilename = "branches.csv";

    // ----------------------------
    // Declare bookkeeping stuff

    // key is kinematics
    // value is a vector of that kinematics' rootfiles
    std::map<TString, std::vector<TString>> rootfilenames;
    std::map<TString, std::vector<TFile*>> rootfiles;

    // Where are the rootfiles?
    TString rootDirectory = data->GetRootfileDirectory();

    // Keep track of charge
    // key is <kinematics,runNumber,bcmName>
    std::map<std::tuple<TString,Int_t,TString>, Double_t> charge;

    // Keep track of good events
    // key is kinematics
    std::map<TString, Double_t> goodEvents;

    // Keep track of charge-normalized yield
    // key is <kinematics,runNumber,bcmName>
    std::map<std::tuple<TString,Int_t,TString>, Double_t> yield;

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
    // Loop over files and calculate charge-normalized yields
    Double_t bcm1;
    Double_t bcm2;
    Double_t bcm4a;
    Double_t bcm4b;
    Double_t bcm17;
    Double_t unser;

    // std::ofstream csv;
    // csv.open(csvFilename.Data());
    // csv << "kinematics,file,index,bcm1" << std::endl;

    std::cout << "Get charge---------------------" << std::endl;
    for (auto const &k : kinematics) {
        std::cout << k << std::endl;

        std::vector<Int_t> runs = data->GetRuns(k);
        std::vector<TFile*> files = rootfiles[k];

        for (int n=0; n<runs.size(); n++) {
            Int_t run = runs[n];

            TFile *f = files[n];
            TTree *scalerTree = (TTree*) f->Get("TSP");

            std::cout<<"set branches for run " << run;
            scalerTree->SetBranchAddress("P.BCM1.scalerCharge",  &bcm1);
            scalerTree->SetBranchAddress("P.BCM2.scalerCharge",  &bcm2);
            scalerTree->SetBranchAddress("P.BCM4A.scalerCharge", &bcm4a);
            scalerTree->SetBranchAddress("P.BCM4B.scalerCharge", &bcm4b);
            scalerTree->SetBranchAddress("P.BCM17.scalerCharge", &bcm17);
            scalerTree->SetBranchAddress("P.Unser.scalerCharge", &unser);
            std::cout<<"✓"<<std::endl;

            // Total charge is the last entry in the branch
            std::cout<<"get entries" << run;
            Int_t N = scalerTree->GetEntries();
            std::cout<<" " << N << " ✓"<<std::endl;

            std::cout<<"set entry" << run;
            scalerTree->GetEntry(N-1);
            std::cout<<"✓"<<std::endl;

            // Store values
            charge[std::make_tuple(k,run,"bcm1")]  = bcm1;
            charge[std::make_tuple(k,run,"bcm2")]  = bcm2;
            charge[std::make_tuple(k,run,"bcm4a")] = bcm4a;
            charge[std::make_tuple(k,run,"bcm4b")] = bcm4b;
            charge[std::make_tuple(k,run,"bcm17")] = bcm17;
            charge[std::make_tuple(k,run,"unser")] = unser;

            // Print branches to csv for debugging
            // for (int i=0; i<N; i++) {
            //     scalerTree->GetEntry(i);

            //     csv << k << "," << f->GetName() << "," << i
            //              << "," << bcm1
            //              << "," << bcm2
            //              << "," << bcm4a
            //              << "," << bcm4b
            //              << "," << bcm17
            //              << "," << unser
            //              << std::endl;
            // }

            delete f;
        }
    }
    // csv.close();

    std::cout << "Get good events ---------------" << std::endl;
    for (auto const &k : kinematics) {
        std::cout << k << std::endl;

        // Get cut
        TCut thisCut = goodEventCut[data->GetTarget(k)];

        // Create list
        data->GetChain(k)->Draw(">>goodEventsList", thisCut);
        TEventList *list = (TEventList*)gDirectory->Get("goodEventsList");

        // Store N
        goodEvents[k] = list->GetN();
    }

    std::cout << "Calculate yield ---------------" << std::endl;
    std::cout << "kinematics,target,Q2,run,bcm,charge,events,yield" << std::endl;
    for (auto const &k : kinematics) {
        std::vector<Int_t> runs = data->GetRuns(k.Data());
        for (auto const &r : runs) {
            for (auto const &b : bcms) {
                auto key = std::make_tuple(k,r,b);
                Double_t thisYield = goodEvents[k] / charge[key];
                yield[key] = thisYield;
                std::cout << k << "," <<data->GetTarget(k) << "," << data->GetQ2(k)
                          << "," << r << "," << b << "," << charge[key] << ","
                          << goodEvents[k] << "," << thisYield << std::endl;
            }
        }
    }
}
