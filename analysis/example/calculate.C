#include <CTData.h>
#include <CTCuts.h>

#include <TRandom.h>
#include <TCanvas.h>
#include <TH1.h>

#include <fstream>
#include <vector>
#include <map>

// This is an example script showing typical things you might want to do
// with the CTData and CTCuts classes in an analysis
int main() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Which cuts?
    TCut myCut = cuts->Get("hBetaCut") && cuts->Get("pBetaCut");

    // Store some kind of information
    std::map<TString, Double_t> randomNumbers;
    std::map<TString, TH1F*> histograms;
    std::map<TString, Int_t> goodevents;

    // Where are we saving data?
    TString csvFilename = "output.csv";
    TString pdfFilename = "output.pdf";

    // ------------------------------------------------------------------------
    // Loop over kinematics and do something
    for (auto const &k : kinematics) {
        std::cout << "Processing " << k << std::endl;
        // A random number
        randomNumbers[k] = gRandom->Gaus(10,2);

        // Draw a histogram
        TString branchName = "H.gtr.dp";
        TString histoName = Form("%s_%d", k.Data(), branchName.Data());
        Int_t bins = 100;
        Double_t lowBin = -10;
        Double_t hiBin  = -10;
        TString drawMe = Form("%s>>%s(%d,%f,%f)",
                               branchName.Data(), histoName.Data(),
                               bins, lowBin, hiBin);

        data->GetChain(k, "T")->Draw(drawMe.Data(), myCut, "goff");

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

    // open PDF; "filename.pdf["
    TCanvas* c1 = new TCanvas("c1", "canvas", 640, 480);
    c1->Print((pdfFilename+"[").Data());

    // print header
    ofs << "kinematics,randnum,goodevents" << std::endl;

    for (auto const &k : kinematics) {
        std::cout << "Printing" << k << std::endl;
        //write to csv
        ofs << k << "," << randomNumbers[k] << "," << goodevents[k] << std::endl;

        // write page to PDF
        c1->Print(pdfFilename.Data());
    }

    // close csv
    ofs.close();

    // close PDF; "filename.pdf]"
    c1->Print((pdfFilename+"]").Data());
}
