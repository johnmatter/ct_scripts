#include <CTData.h>
#include <CTCuts.h>

#include <TRandom.h>
#include <TCanvas.h>
#include <TH1.h>

#include <fstream>
#include <vector>
#include <map>

// Calculate number of good events for all kinematics and print to CSV
int main() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Store some kind of information
    std::map<TString, TH1F*> histograms;
    std::map<TString, Int_t> goodevents;

    // Where are we saving data?
    TString csvFilename = "output.csv";

    // ------------------------------------------------------------------------
    // Loop over kinematics and calculate good events
    for (auto const &k : kinematics) {
        std::cout << "Processing " << k << std::endl;

        // Which cut?
        TCut cut = "initialize";
        if (k.Contains("C12")) {
            cut = cuts->Get("coinCutsC12");
        }
        if (k.Contains("LH2")) {
            cut = cuts->Get("coinCutsLH2");
        }
        if (cut == "initialize") {
            std::cerr << "Did not find cut for this kinematics: " << k << std::endl;
            continue;
        }

        // Draw a histogram
        // We'll use HMS delta as a dummy variable
        TString branchName = "H.gtr.dp";
        TString histoName = k;
        Int_t bins = 20;
        Double_t lowBin = -10;
        Double_t hiBin  = -10;
        TString drawMe = Form("%s>>%s(%d,%f,%f)",
                               branchName.Data(), histoName.Data(),
                               bins, lowBin, hiBin);

        data->GetChain(k, "T")->Draw(drawMe.Data(), cut, "goff");

        histograms[k] = (TH1F*) gDirectory->Get(histoName.Data());

        // Number of events in the histogram
        // Bin 0 = underflow
        // Bin 1 through n = inside histogram
        // Bin n+1 = overflow
        // So we want integral from 1 to n
        goodevents[k] = histograms[k]->Integral(1,bins);
    }

    // ------------------------------------------------------------------------
    // Output data

    // open csv
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // print header
    ofs << "kinematics,target,Q2,collimator,goodevents" << std::endl;

    for (auto const &k : kinematics) {
        std::cout << "Printing " << k << std::endl;
        //write to csv
        ofs << k
            << "," << data->GetTarget(k)
            << "," << data->GetQ2(k)
            << "," << data->GetCollimator(k)
            << "," << goodevents[k]
            << std::endl;
    }

    // close csv
    ofs.close();
}
