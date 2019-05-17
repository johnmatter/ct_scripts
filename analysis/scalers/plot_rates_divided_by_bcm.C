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

void plot_rates_divided_by_bcm() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to save
    TString pdfFilename = "rates_divided_by_bcm.pdf";

    // Which kinematics
    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
    //                                    "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};
    std::vector<TString> kinematics = data->GetNames();

    // Which triggers
    std::vector<TString> trigNames = {"pTRIG1", "pTRIG3", "pTRIG4", "pTRIG6"};

    // Which bcms
    std::vector<TString> bcms = {"BCM4A"};

    // ------------------------------------------------------------------------
    // Plot and save PDF
    TString drawMe; // used to format tree->Draw() type lines
    TString histoName;
    TString histoTitle, xAxisTitle, yAxisTitle;
    TH2F* histoTemp;

    TCanvas* cRates = new TCanvas("cRates", "Rates", 1024, 640);

    //Open PDF
    cRates->Print((pdfFilename+"[").Data());

    // Loop over kinematics
    for (auto const &k : kinematics) {
        std::cout << k << std::endl;

        // Get scaler tree
        TTree *scalerTree = data->GetChain(k, "TSP");

        // Loop over bcms
        for (auto const &b : bcms) {

            // Loop over triggers
            for (auto const &t : trigNames) {

                // Draw histo
                histoName = Form("h_%s_%s_by_%s", k.Data(), t.Data(), b.Data());
                drawMe = Form("(P.%s.scalerRate/P.%s.scalerCurrent):This->GetReadEntry()>>%s", t.Data(), b.Data(), histoName.Data());
                scalerTree->Draw(drawMe.Data());

                // Get histo
                histoTemp = (TH2F*) gPad->GetPrimitive(histoName.Data());

                // Format
                histoTitle = Form("%s", k.Data());
                xAxisTitle = Form("Entry");
                yAxisTitle = Form("P.%s.scalerRate/P.%s.scalerCurrent", t.Data(), b.Data());
                histoTemp->SetTitle(histoTitle.Data());
                histoTemp->GetXaxis()->SetTitle(xAxisTitle.Data());
                histoTemp->GetYaxis()->SetTitle(yAxisTitle.Data());
                cRates->Update();

                // Write page to pdf
                cRates->Print(pdfFilename.Data());
            }
        }
    }

    // Close PDF
    cRates->Print((pdfFilename+"]").Data());
}
