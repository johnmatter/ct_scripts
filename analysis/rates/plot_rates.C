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
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Which bcms
    std::vector<TString> bcms = {"BCM1", "BCM2", "BCM4A", "BCM4B", "BCM17", "Unser"};

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
    cRates->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["

    for (auto const &k : kinematics) {
        std::cout << k << std::endl;
        for (auto const &b : bcms) {

            TTree *scalerTree = data->GetChain(k, "TSP");

            // pTRIG1
            histoName = Form("h_%s_ptrig_vs_%s", k.Data(), b.Data());
            drawMe = Form("P.pTRIG1.scalerRate:P.%s.scalerCurrent>>%s", b.Data(), histoName.Data());
            scalerTree->Draw(drawMe.Data());
            histoTemp = (TH2F*) gPad->GetPrimitive(histoName.Data());
            histoTitle = Form("%s pTRIG1 vs %s", k.Data(), b.Data());
            histoTemp->SetTitle(histoTitle.Data());
            cRates->Update();
            cRates->Print(pdfFilename.Data()); // write page to PDF

            // hTRIG1
            histoName = Form("h_%s_htrig_vs_%s", k.Data(), b.Data());
            drawMe = Form("P.hTRIG1.scalerRate:P.%s.scalerCurrent>>%s", b.Data(), histoName.Data());
            scalerTree->Draw(drawMe.Data());
            histoTemp = (TH2F*) gPad->GetPrimitive(histoName.Data());
            histoTitle = Form("%s hTRIG1 vs %s", k.Data(), b.Data());
            histoTemp->SetTitle(histoTitle.Data());
            cRates->Update();
            cRates->Print(pdfFilename.Data()); // write page to PDF
        }
    }
    cRates->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"
}
