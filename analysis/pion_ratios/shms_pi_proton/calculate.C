#include <CTData.h>
#include <CTCuts.h>

#include <TEventList.h>
#include <TTree.h>
#include <TFile.h>

#include <fstream>
#include <vector>
#include <map>

// This calculates the number of pi+'s and protons in the HMS for our data
void calculate() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Which cuts?
    TCut piCut = cuts->Get("pPi");
    TCut protonCut = cuts->Get("pProton");

    // Store number of pions and protons
    std::map<std::tuple<TString, Int_t>, TEventList*> piLists;
    std::map<std::tuple<TString, Int_t>, TEventList*> protonLists;
    std::map<std::tuple<TString, Int_t>, Int_t> nPi;
    std::map<std::tuple<TString, Int_t>, Int_t> nProton;

    // Where are we saving data?
    TString csvFilename = "pi_proton_count.csv";

    // ------------------------------------------------------------------------
    // Loop over kinematics, count pions and protons
    TString elistName, elistDraw;
    TString rootFilename;
    TFile *f;
    TTree *T;
    for (auto const &k : kinematics) {
        std::cout << "Processing " << k << std::endl;
        for (auto const &run : data->GetRuns(k)) {

            rootFilename = Form(data->GetRootfileTemplate(k),
                                data->GetRootfileDirectory().Data(),
                                run);
            f  = new TFile(rootFilename.Data(), "READ");
            T  = (TTree*) f->Get("T");

            // Count pions
            elistName.Form("%s_run%d_pions", k.Data(), run);
            elistDraw.Form(">>%s", elistName.Data());
            T->Draw(elistDraw, piCut);
            piLists[std::make_tuple(k,run)] = (TEventList*) gDirectory->Get(elistName.Data());
            nPi[std::make_tuple(k,run)] = piLists[std::make_tuple(k,run)]->GetN();

            // Count protons
            elistName.Form("%s_run%d_protons", k.Data(), run);
            elistDraw.Form(">>%s", elistName.Data());
            T->Draw(elistDraw, protonCut);
            protonLists[std::make_tuple(k,run)] = (TEventList*) gDirectory->Get(elistName.Data());
            nProton[std::make_tuple(k,run)] = protonLists[std::make_tuple(k,run)]->GetN();
        }
    }

    // ------------------------------------------------------------------------
    // Output data

    // open csv
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // print header
    ofs << "kinematics,Q2,target,collimator,run,pions,protons" << std::endl;
    for (auto const &k : kinematics) {
        std::cout << "Printing " << k << std::endl;
        for (auto const &run : data->GetRuns(k)) {
            ofs        << k 
                << "," << data->GetQ2(k) 
                << "," << data->GetTarget(k) 
                << "," << data->GetCollimator(k) 
                << "," << run
                << "," << nPi[std::make_tuple(k,run)]
                << "," << nProton[std::make_tuple(k,run)]
                << std::endl;
        }
    }

    // close csv
    ofs.close();
}

int main() {
    calculate();
    return 0;
}
