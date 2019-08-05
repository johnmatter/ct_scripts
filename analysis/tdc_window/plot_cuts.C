#include <map>
#include <vector>
#include <iostream>

#include <TPad.h>
#include <TVirtualPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TPaveLabel.h>
#include <TCut.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TObjArray.h>

#include "CTData.h"
#include "CTCuts.h"

void plot_cuts() {
    //-------------------------------------------------------------------------------------------------------------------------
    // Load data

    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Which TDCs do you want?
    std::vector<TString> TDCs = {"pTRIG1_ROC2", "pTRIG4_ROC2", "pTRIG6_ROC2",
                                 "pTRIG1_ROC1", "pTRIG4_ROC1", "pTRIG6_ROC1",
                                 "pEDTM", "hEDTM"};

    // Infrastructure for storing histograms
    // Key is tuple<
    std::map<TString, TH1*> histos;

    // PDF to save histograms to
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/tdc_window/histos.pdf";

    // Setup canvas
    gStyle->SetOptTitle(kFALSE);
    // open PDF
    TCanvas* canvas = new TCanvas("canvas", "compare", 640, 480);
    canvas->Print((pdfFilename+"[").Data());

    //-------------------------------------------------------------------------------------------------------------------------
    // TODO: somehow read this info from the param file itself
    // This is my tcoin.param, with cuts set by Deepak.
    // Default window in hcana is [0,10000]
    std::vector<TString> t_coin_adcNames = {
                            "hASUM", "hBSUM", "hCSUM", "hDSUM", "hPSHWR", "hSHWR", "hAER", "hCER", "hFADC_TREF_ROC1", "pAER",
                            "pHGCER", "pNGCER", "pPSHWR", "pFADC_TREF_ROC2", "pHGCER_MOD", "pNGCER_MOD", "pHEL_NEG", "pHEL_POS", "pHEL_MPS"};
    std::vector<TString> t_coin_tdcNames = {
                            "h1X", "h1Y", "h2X", "h2Y", "h1T", "h2T", "hT1", "hASUM", "hBSUM", "hCSUM",
                            "hDSUM", "hPRLO", "hPRHI", "hSHWR", "hEDTM", "hCER", "hT2", "hDCREF1", "hDCREF2", "hDCREF3",
                            "hDCREF4", "hTRIG1_ROC1", "hTRIG2_ROC1", "hTRIG3_ROC1", "hTRIG4_ROC1", "hTRIG5_ROC1", "hTRIG6_ROC1", "pTRIG1_ROC1", "pTRIG2_ROC1", "pTRIG3_ROC1",
                            "pTRIG4_ROC1", "pTRIG5_ROC1", "pTRIG6_ROC1", "pT1", "pT2", "p1X", "p1Y", "p2X", "p2Y", "p1T",
                            "p2T", "pT3", "pAER", "pHGCER", "pNGCER", "pDCREF1", "pDCREF2", "pDCREF3", "pDCREF4", "pDCREF5",
                            "pDCREF6", "pDCREF7", "pDCREF8", "pDCREF9", "pDCREF10", "pEDTM", "pPRLO", "pPRHI", "pTRIG1_ROC2", "pTRIG2_ROC2",
                            "pTRIG3_ROC2", "pTRIG4_ROC2", "pTRIG5_ROC2", "pTRIG6_ROC2", "hTRIG1_ROC2", "hTRIG2_ROC2", "hTRIG3_ROC2", "hTRIG4_ROC2", "hTRIG5_ROC2", "hTRIG6_ROC2",
                            "pSTOF_ROC2", "pEL_LO_LO_ROC2", "pEL_LO_ROC2", "pEL_HI_ROC2", "pEL_REAL_ROC2", "pEL_CLEAN_ROC2", "hSTOF_ROC2", "hEL_LO_LO_ROC2", "hEL_LO_ROC2", "hEL_HI_ROC2",
                            "hEL_REAL_ROC2", "hEL_CLEAN_ROC2", "pSTOF_ROC1", "pEL_LO_LO_ROC1", "pEL_LO_ROC1", "pEL_HI_ROC1", "pEL_REAL_ROC1", "pEL_CLEAN_ROC1", "hSTOF_ROC1", "hEL_LO_LO_ROC1",
                            "hEL_LO_ROC1", "hEL_HI_ROC1", "hEL_REAL_ROC1", "hEL_CLEAN_ROC1", "pPRE40_ROC1", "pPRE100_ROC1", "pPRE150_ROC1", "pPRE200_ROC1", "hPRE40_ROC1", "hPRE100_ROC1",
                            "hPRE150_ROC1", "hPRE200_ROC1", "pPRE40_ROC2", "pPRE100_ROC2", "pPRE150_ROC2", "pPRE200_ROC2", "hPRE40_ROC2", "hPRE100_ROC2", "hPRE150_ROC2", "hPRE200_ROC2",
                            "hDCREF5", "hT3", "pRF", "hHODO_RF", "pHODO_RF"};

    std::vector<Int_t> t_coin_TdcTimeWindowMin = {
                            0, 0, 0, 0, 0, 0, 1500, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 1500, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 2300, 0, 0,
                            2400, 0, 2400, 3000, 2000, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 12500, 12500, 12500, 12500, 12500,
                            12500,  12500,  12500, 12500, 12500, 0, 0, 0, 2700, 0,
                            0, 3100, 0, 3000, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0};

    std::vector<Int_t> t_coin_TdcTimeWindowMax = {
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 4500, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 3600, 100000, 100000,
                            3500, 100000, 3300, 4500, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 4000, 100000,
                            100000, 4000, 100000, 4000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,
                            100000, 100000, 100000, 100000, 100000};

    //-------------------------------------------------------------------------------------------------------------------------
    // Create maps from the above info
    std::map<TString, Int_t> tdcTimeWindowMin;
    std::map<TString, Int_t> tdcTimeWindowMax;
    for (int i=0; i<t_coin_tdcNames.size(); i++) {
        TString tdc = t_coin_tdcNames[i];

        // TDC time window min
        if (i<t_coin_TdcTimeWindowMin.size()) {
            tdcTimeWindowMin[tdc] = t_coin_TdcTimeWindowMin[i];
        } else {
            tdcTimeWindowMin[tdc] = 0;
            std::cout << "Index for " << tdc << " in t_coin_tdcNames beyond bounds of t_coin_TdcTimeWindowMin. Using " << tdcTimeWindowMin[tdc] << std::endl;
        }

        // TDC time window max
        if (i<t_coin_TdcTimeWindowMax.size()) {
            tdcTimeWindowMax[tdc] = t_coin_TdcTimeWindowMax[i];
        } else {
            tdcTimeWindowMax[tdc] = 10000;
            std::cout << "Index for " << tdc << " in t_coin_tdcNames beyond bounds of t_coin_TdcTimeWindowMax. Using " << tdcTimeWindowMax[tdc] << std::endl;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Loop over kinematics and create histograms for data
    TString histoName, drawMe, branch;
    Int_t xMin, xMax;
    Int_t nBins = 200;
    TCut cut;
    for(auto const &tdc: TDCs) {
        for(auto const &k: kinematics) {
            std::cout << "Histogramming " << tdc << " for " << k << std::endl;

            branch = Form("T.coin.%s_tdcTimeRaw", tdc.Data());

            // Get max and min from temp histogram
            cut="";
            histoName = Form("%s_%s_temp", k.Data(), tdc.Data());
            drawMe = Form("%s>>%s", branch.Data(), histoName.Data());
            data->GetChain(k)->Draw(drawMe.Data(), cut, "goff");
            histos[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

            xMin = histos[histoName]->GetXaxis()->GetXmin();
            xMax = histos[histoName]->GetXaxis()->GetXmax();

            // Draw open histogram
            cut="";
            histoName = Form("%s_%s_open", k.Data(), tdc.Data());
            drawMe = Form("%s>>%s(%d,%d,%d)", branch.Data(), histoName.Data(), nBins, xMin, xMax);
            data->GetChain(k)->Draw(drawMe.Data(), cut, "goff");
            histos[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

            // Draw window histogram
            cut = Form("(%s>%d) && (%s<%d)", branch.Data(), tdcTimeWindowMin[tdc],
                                             branch.Data(), tdcTimeWindowMax[tdc]);
            histoName = Form("%s_%s_window", k.Data(), tdc.Data());
            drawMe = Form("%s>>%s(%d,%d,%d)", branch.Data(), histoName.Data(), nBins, xMin, xMax);
            data->GetChain(k)->Draw(drawMe.Data(), cut, "goff");
            histos[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

            std::cout << cut << std::endl;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Loop over kinematics and print histos to PDF

    TString histoOpen, histoWindow;
    TLine *windowMinMarker, *windowMaxMarker;
    for(auto const &tdc: TDCs) {
        for (auto const &k: kinematics) {

            histoOpen = Form("%s_%s_open", k.Data(), tdc.Data());
            histoWindow = Form("%s_%s_window", k.Data(), tdc.Data());

            std::cout << "Printing " << std::endl;

            histos[histoOpen]->SetLineColor(kRed+2);
            histos[histoWindow]->SetLineColor(kBlue+2);

            histos[histoOpen]->SetFillColor(kRed+2);
            histos[histoWindow]->SetFillColor(kBlue+2);

            histos[histoOpen]->SetFillStyle(3345);
            histos[histoWindow]->SetFillStyle(3354);

            histos[histoOpen]->Draw();
            histos[histoWindow]->Draw("SAME");

            // Add reference lines for cuts
            // min
            canvas->Update();
            windowMinMarker = new TLine(tdcTimeWindowMin[tdc], canvas->GetUymin(),
                                        tdcTimeWindowMin[tdc], canvas->GetUymax());
            windowMinMarker->SetLineColor(kGreen);
            windowMinMarker->SetLineStyle(7);
            windowMinMarker->Draw();
            // max
            windowMaxMarker = new TLine(tdcTimeWindowMax[tdc], canvas->GetUymin(),
                                        tdcTimeWindowMax[tdc], canvas->GetUymax());
            windowMaxMarker->SetLineColor(kGreen);
            windowMaxMarker->SetLineStyle(7);
            windowMaxMarker->Draw();

            canvas->Print(pdfFilename.Data());

            delete windowMaxMarker;
            delete windowMinMarker;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // close PDF
    canvas->Print((pdfFilename+"]").Data());

}
