#include <utility>
#include <fstream>
#include <map>

#include <TCut.h>
#include <TString.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TH2F.h>

#include <CTData.h>

void plot_EDTM_vs_others() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // PDF to save histos in
    TString pdfFilename = "trigger_tdcTimeRaw_histos.pdf";


    std::vector<TString> yBranches = {
        "T.coin.hEDTM_tdcTimeRaw",
        "T.coin.pEDTM_tdcTimeRaw"
    };

    std::vector<TString> xBranches = {
        "T.coin.hTRIG1_ROC1_tdcTimeRaw",
        "T.coin.hTRIG2_ROC1_tdcTimeRaw",
        "T.coin.hTRIG3_ROC1_tdcTimeRaw",
        "T.coin.hTRIG4_ROC1_tdcTimeRaw",
        "T.coin.hTRIG5_ROC1_tdcTimeRaw",
        "T.coin.hTRIG6_ROC1_tdcTimeRaw",
        "T.coin.pTRIG1_ROC1_tdcTimeRaw",
        "T.coin.pTRIG2_ROC1_tdcTimeRaw",
        "T.coin.pTRIG3_ROC1_tdcTimeRaw",
        "T.coin.pTRIG4_ROC1_tdcTimeRaw",
        "T.coin.pTRIG5_ROC1_tdcTimeRaw",
        "T.coin.pTRIG6_ROC1_tdcTimeRaw",
        "T.coin.hTRIG1_ROC2_tdcTimeRaw",
        "T.coin.hTRIG2_ROC2_tdcTimeRaw",
        "T.coin.hTRIG3_ROC2_tdcTimeRaw",
        "T.coin.hTRIG4_ROC2_tdcTimeRaw",
        "T.coin.hTRIG5_ROC2_tdcTimeRaw",
        "T.coin.hTRIG6_ROC2_tdcTimeRaw",
        "T.coin.pTRIG1_ROC2_tdcTimeRaw",
        "T.coin.pTRIG2_ROC2_tdcTimeRaw",
        "T.coin.pTRIG3_ROC2_tdcTimeRaw",
        "T.coin.pTRIG4_ROC2_tdcTimeRaw",
        "T.coin.pTRIG5_ROC2_tdcTimeRaw",
        "T.coin.pTRIG6_ROC2_tdcTimeRaw"
    };

    // ------------------------------------------------------------------------
    // Draw and print to PDF
    Int_t xBins=200;
    Int_t yBins=200;
    Double_t yMin, yMax, xMin, xMax;
    TH1F *hXlim, *hYlim;
    TH2F *h2;
    TString histoName, drawStr;

    gStyle->SetOptStat(0);

    TCanvas* c = new TCanvas("c", "canvas", 800, 600);
    c->SetLogz();

    // Open PDF
    c->Print((pdfFilename+"[").Data());

    for (auto const &yBranch: yBranches) {
        for (auto const &xBranch: xBranches) {
            std::cout << Form("%s vs %s", yBranch.Data(), xBranch.Data()) << std::endl;

            for (auto const &k: kinematics) {
                // Skip because we didn't have the EDTM set up
                if (data->GetQ2(k)==8) {continue;}

                // Get max and min for yBranch
                histoName = Form("%s_%s", yBranch.Data(), k.Data());
                data->GetChain(k)->Draw(Form("%s>>%s", yBranch.Data(), histoName.Data()),"");
                hYlim = (TH1F*) gDirectory->Get(histoName.Data());
                yMin = hYlim->GetXaxis()->GetXmin();
                yMax = hYlim->GetXaxis()->GetXmax();

                // Get max and min for xBranch
                histoName = Form("%s_%s", xBranch.Data(), k.Data());
                data->GetChain(k)->Draw(Form("%s>>%s", xBranch.Data(), histoName.Data()),"");
                hXlim = (TH1F*) gDirectory->Get(histoName.Data());
                xMin = hXlim->GetXaxis()->GetXmin();
                xMax = hXlim->GetXaxis()->GetXmax();

                // Format
                histoName = Form("%s_vs_%s_%s", yBranch.Data(), xBranch.Data(), k.Data());
                drawStr = Form("%s:%s>>%s(%d,%f,%f,%d,%f,%f)",
                                  yBranch.Data(), xBranch.Data(), histoName.Data(),
                                  xBins, xMin, xMax,
                                  yBins, yMin, yMax);

                std::cout << drawStr << std::endl;
                // Draw
                data->GetChain(k)->Draw(drawStr.Data(),"","colz");

                // Format
                h2 = (TH2F*) gDirectory->Get(histoName.Data());
                h2->SetTitle(histoName.Data());
                h2->Draw("colz");

                // Print
                c->Print(pdfFilename.Data());
           }
       }
    }

    // Close PDF
    c->Print((pdfFilename+"]").Data());
}
