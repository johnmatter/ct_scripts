#include <CTData.h>
#include <CTCuts.h>

#include <TEventList.h>
#include <TTree.h>
#include <TFile.h>

#include <fstream>
#include <vector>
#include <map>

// This calculates the number of pi-'s and electrons in the HMS for our data
int main() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Which cuts?
    TCut piCut = cuts->Get("hPi");
    TCut electronCut = cuts->Get("hElec");

    // Store number of pions and electrons
    std::map<std::tuple<TString, Int_t>, TEventList*> piLists;
    std::map<std::tuple<TString, Int_t>, TEventList*> electronLists;
    std::map<std::tuple<TString, Int_t>, Int_t> nPi;
    std::map<std::tuple<TString, Int_t>, Int_t> nElectron;

    // Where are we saving data?
    TString csvFilename = "pi_e_count.csv";

    // ------------------------------------------------------------------------
    // Loop over kinematics, count pions and electrons
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

            // Count electrons
            elistName.Form("%s_run%d_electrons", k.Data(), run);
            elistDraw.Form(">>%s", elistName.Data());
            T->Draw(elistDraw, electronCut);
            electronLists[std::make_tuple(k,run)] = (TEventList*) gDirectory->Get(elistName.Data());
            nElectron[std::make_tuple(k,run)] = electronLists[std::make_tuple(k,run)]->GetN();
        }
    }

    // ------------------------------------------------------------------------
    // Output data

    // open csv
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // print header
    ofs << "kinematics,Q2,target,collimator,run,pions,electrons" << std::endl;
    for (auto const &k : kinematics) {
        std::cout << "Printing " << k << std::endl;
        for (auto const &run : data->GetRuns(k)) {
            ofs        << k 
                << "," << data->GetQ2(k) 
                << "," << data->GetTarget(k) 
                << "," << data->GetCollimator(k) 
                << "," << run
                << "," << nPi[std::make_tuple(k,run)]
                << "," << nElectron[std::make_tuple(k,run)]
                << std::endl;
        }
    }

    // close csv
    ofs.close();
}
