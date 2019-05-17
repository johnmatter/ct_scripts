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

void plot_rates() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to save
    TString pdfFilename = "rates.pdf";

    // Which kinematics
    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
    //                                    "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};
    std::vector<TString> kinematics = data->GetNames();

    // Which triggers
    std::vector<TString> trigNames = {"pTRIG1", "pTRIG2", "pTRIG3", "pTRIG4", "pTRIG5", "pTRIG6",
                                      "hTRIG1", "hTRIG2", "hTRIG3", "hTRIG4", "hTRIG5", "hTRIG6"};

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
    TString histoTitle, xAxisTitle, yAxisTitle;
    TH2F* histoTemp;

    TCanvas* cRates = new TCanvas("cRates", "Rates", 1024, 640);

    // Open PDF
    cRates->Print((pdfFilename+"[").Data());

    // Loop over kinematics
    for (auto const &k : kinematics) {
        std::cout << k << std::endl;

        // Get scaler tree
        TTree *scalerTree = data->GetChain(k, "TSP");

        // Loop over triggers
        for (auto const &t : trigNames) {
            // Histogram name
            histoName = Form("h_%s_%s", k.Data(), t.Data());
            drawMe = Form("P.%s.scalerRate:This->GetReadEntry()>>%s", t.Data(), histoName.Data());

            // Draw
            scalerTree->Draw(drawMe.Data());

            // Get object
            histoTemp = (TH2F*) gPad->GetPrimitive(histoName.Data());

            // Formatting
            histoTitle = Form("%s", k.Data());
            xAxisTitle = Form("Entry");
            yAxisTitle = Form("P.%s.scalerRate", t.Data());
            histoTemp->SetTitle(histoTitle.Data());
            histoTemp->GetXaxis()->SetTitle(xAxisTitle.Data());
            histoTemp->GetYaxis()->SetTitle(yAxisTitle.Data());
            cRates->Update();

            // Write page to PDF
            cRates->Print(pdfFilename.Data());
        }
    }
    // Close PDF
    cRates->Print((pdfFilename+"]").Data());
}
