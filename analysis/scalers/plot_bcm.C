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

void plot_bcm() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to save
    TString pdfFilename = "bcm.pdf";

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Which bcms
    std::vector<TString> bcms = {"BCM1", "BCM2", "BCM4A", "BCM4B", "BCM4C"};

    // ------------------------------------------------------------------------
    // Plot and save PDF
    TString drawMe; // used to format tree->Draw() type lines
    TString histoName;
    TString histoTitle, xAxisTitle, yAxisTitle;
    TH2F* histoTemp;

    TCanvas* cRates = new TCanvas("cRates", "Rates", 1024, 640);
    cRates->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["

    for (auto const &k : kinematics) {
        std::cout << k << std::endl;
        for (auto const &b : bcms) {

            TTree *scalerTree = data->GetChain(k, "TSP");

            // Draw
            histoName = Form("h_%s_ptrig_vs_%s", k.Data(), b.Data());
            drawMe = Form("P.%s.scalerCurrent:This->GetReadEntry()>>%s", b.Data(), histoName.Data());
            scalerTree->Draw(drawMe.Data());

            // Format
            histoTemp = (TH2F*) gPad->GetPrimitive(histoName.Data());
            histoTitle = Form("%s", k.Data());
            xAxisTitle = Form("Entry");
            yAxisTitle = Form("P.%s.scalerCurrent",b.Data());
            histoTemp->SetTitle(histoTitle.Data());
            histoTemp->GetXaxis()->SetTitle(xAxisTitle.Data());
            histoTemp->GetYaxis()->SetTitle(yAxisTitle.Data());

            // Write page
            cRates->Update();
            cRates->Print(pdfFilename.Data()); // write page to PDF
        }
    }
    cRates->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"
}
