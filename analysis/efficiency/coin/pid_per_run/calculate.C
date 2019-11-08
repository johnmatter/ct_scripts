#include <CTData.h>
#include <CTCuts.h>

#include <TEventList.h>
#include <TTree.h>
#include <TFile.h>

#include <fstream>
#include <vector>
#include <map>

// This calculates PID efficiency per run
int main() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Which detectors?
    std::vector<TString> detectors = {"hCal", "hCer", "pCer"};

    // cuts
    TCut shouldCut, didCut;

    // Store number of pions and electrons
    std::map<std::tuple<TString, TString, Int_t>, TEventList*> shouldList;
    std::map<std::tuple<TString, TString, Int_t>, TEventList*> didList;
    std::map<std::tuple<TString, TString, Int_t>, Int_t> nShould;
    std::map<std::tuple<TString, TString, Int_t>, Int_t> nDid;

    // Where are we saving data?
    TString csvFilename = "pid_per_run.csv";

    // ------------------------------------------------------------------------
    // Loop over kinematics, count should and did
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

            for (auto const &detector: detectors) {

                shouldCut = cuts->Get(Form("%sShould", detector.Data()));
                didCut = cuts->Get(Form("%sDid", detector.Data()));

                // Count should
                elistName.Form("%s_run%d_%s_should", k.Data(), run, detector.Data());
                elistDraw.Form(">>%s", elistName.Data());
                T->Draw(elistDraw, shouldCut);
                shouldList[std::make_tuple(k,detector,run)] = (TEventList*) gDirectory->Get(elistName.Data());
                nShould[std::make_tuple(k,detector,run)] = shouldList[std::make_tuple(k,detector,run)]->GetN();

                // Count did
                elistName.Form("%s_run%d_%s_did", k.Data(), run, detector.Data());
                elistDraw.Form(">>%s", elistName.Data());
                T->Draw(elistDraw, didCut);
                didList[std::make_tuple(k,detector,run)] = (TEventList*) gDirectory->Get(elistName.Data());
                nDid[std::make_tuple(k,detector,run)] = didList[std::make_tuple(k,detector,run)]->GetN();
            }

            delete f;
        }
    }

    // ------------------------------------------------------------------------
    // Output data

    // open csv
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // print header
    ofs << "kinematics,run,detector,should,did" << std::endl;
    for (auto const &k : kinematics) {
        std::cout << "Printing " << k << std::endl;
        for (auto const &run : data->GetRuns(k)) {
            for (auto const &detector: detectors) {
                ofs << k << "," << run << "," << detector << ","
                    << nShould[std::make_tuple(k,detector,run)] << ","
                    << nDid[std::make_tuple(k,detector,run)]
                    << std::endl;
            }
        }
    }

    // close csv
    ofs.close();
}
