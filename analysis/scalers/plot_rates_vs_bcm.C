#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TEventList.h>
#include <TCanvas.h>
#include <TH2.h>

#include <CTData.h>
#include <CTCuts.h>

void plot_rates_vs_bcm() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to save
    TString pdfFilename = "rates_vs_bcm.pdf";

    // Which kinematics
    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
    //                                    "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};
    std::vector<TString> kinematics = data->GetNames();

    // Which triggers
    std::vector<TString> trigNames = {"pTRIG1", "pTRIG3", "pTRIG4", "pTRIG6"};

    // Which bcms
    std::vector<TString> bcms = {"BCM4A"};

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
    // Plot and save PDF
    TString drawMe; // used to format tree->Draw() type lines
    TString histoName;
    TString histoTitle;
    TH2F* histoTemp;

    TCanvas* cRates = new TCanvas("cRates", "Rates", 1024, 640);

    // Open PDF
    cRates->Print((pdfFilename+"[").Data());

    for (auto const &k : kinematics) {
        std::cout << k << std::endl;

        // Get scaler tree
        TTree *scalerTree = data->GetChain(k, "TSP");

        // Loop over bcms
        for (auto const &b : bcms) {

            // Loop over triggers
            for (auto const &t : trigNames) {
                // Histogram name
                histoName = Form("h_%s_%s_vs_%s", k.Data(), t.Data(), b.Data());
                drawMe = Form("P.%s.scalerRate:P.%s.scalerCurrent>>%s", t.Data(), b.Data(), histoName.Data());

                // Draw
                scalerTree->Draw(drawMe.Data());

                // Get object
                histoTemp = (TH2F*) gPad->GetPrimitive(histoName.Data());

                // Formatting
                histoTitle = Form("%s: %s vs %s", k.Data(), t.Data(), b.Data());
                histoTemp->SetTitle(histoTitle.Data());
                histoTemp->GetXaxis()->SetTitle("uA");
                histoTemp->GetYaxis()->SetTitle("Hz");
                cRates->Update();

                // Write page to PDF
                cRates->Print(pdfFilename.Data());
            }
        }
    }

    // Close PDF
    cRates->Print((pdfFilename+"]").Data());
}
