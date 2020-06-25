#include <utility>
#include <fstream>
#include <map>

#include <TCut.h>
#include <TMath.h>
#include <TString.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TFile.h>
#include <TPaveText.h>
#include <TLine.h>

#include <CTData.h>

// run this after generate_tdcTime_histos.C
void print_tdcTime_histos_to_pdf() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // Which triggers
    std::vector<TString> trigBranches = {"T.coin.pEDTM_tdcTime", "T.coin.pTRIG6_ROC2_tdcTime"};

    // PDF to save histos in
    TString pdfFilename = "trigger_tdcTime_histos.pdf";

    // CSV to save counts in peaks
    TString csvFilename= "trigger_tdcTime_peak_counts.csv";

    // Accepted trigger counts per peak
    std::map<Int_t,Int_t> ttrig;
    std::map<Int_t,Int_t> tedtm;

    // Open file so we can load histograms
    TFile *f = new TFile("ct_tdcTime.root", "READ");

    // Cuts
    TCut EDTMOnly = "T.coin.pEDTM_tdcTimeRaw!=0";
    TCut noEDTM = "T.coin.pEDTM_tdcTimeRaw==0";
    TCut noCut = "";
    std::vector<TCut> cuts = {noCut, EDTMOnly, noEDTM};

    // ------------------------------------------------------------------------
    // Print to PDF and write counts to CSV
    gStyle->SetOptTitle(0);

    TPaveText *text;

    TLine *trigCenter, *trigLow, *trigHi;
    TLine *edtm1Center, *edtm1Low, *edtm1Hi;
    TLine *edtm2Center, *edtm2Low, *edtm2Hi;
    TLine *edtm3Center, *edtm3Low, *edtm3Hi;

    std::map<TString, Double_t> trigCenterX;
    std::map<TString, Double_t> trigLowX;
    std::map<TString, Double_t> trigHiX;
    std::map<TString, Double_t> edtm1CenterX;
    std::map<TString, Double_t> edtm1LowX;
    std::map<TString, Double_t> edtm1HiX;
    std::map<TString, Double_t> edtm2CenterX;
    std::map<TString, Double_t> edtm2LowX;
    std::map<TString, Double_t> edtm2HiX;
    std::map<TString, Double_t> edtm3CenterX;
    std::map<TString, Double_t> edtm3LowX;
    std::map<TString, Double_t> edtm3HiX;

    trigCenterX["T.coin.pTRIG6_ROC2_tdcTime"] = 274;
    trigLowX["T.coin.pTRIG6_ROC2_tdcTime"]    = 273;
    trigHiX["T.coin.pTRIG6_ROC2_tdcTime"]     = 275;

    edtm1CenterX["T.coin.pTRIG6_ROC2_tdcTime"] = 285;
    edtm1LowX["T.coin.pTRIG6_ROC2_tdcTime"]    = 284;
    edtm1HiX["T.coin.pTRIG6_ROC2_tdcTime"]     = 286;

    edtm2CenterX["T.coin.pTRIG6_ROC2_tdcTime"] = 330;
    edtm2LowX["T.coin.pTRIG6_ROC2_tdcTime"]    = 331;
    edtm2HiX["T.coin.pTRIG6_ROC2_tdcTime"]     = 332;

    edtm3CenterX["T.coin.pTRIG6_ROC2_tdcTime"] = 337;
    edtm3LowX["T.coin.pTRIG6_ROC2_tdcTime"]    = 336;
    edtm3HiX["T.coin.pTRIG6_ROC2_tdcTime"]     = 338;

    trigCenterX["T.coin.pEDTM_tdcTime"] = 92.5;
    trigLowX["T.coin.pEDTM_tdcTime"]    = 91.5;
    trigHiX["T.coin.pEDTM_tdcTime"]     = 93.5;

    edtm1CenterX["T.coin.pEDTM_tdcTime"] = 110;
    edtm1LowX["T.coin.pEDTM_tdcTime"]    = 109;
    edtm1HiX["T.coin.pEDTM_tdcTime"]     = 111;

    edtm2CenterX["T.coin.pEDTM_tdcTime"] = 155.5;
    edtm2LowX["T.coin.pEDTM_tdcTime"]    = 154.5;
    edtm2HiX["T.coin.pEDTM_tdcTime"]     = 156.5;

    edtm3CenterX["T.coin.pEDTM_tdcTime"] = 160;
    edtm3LowX["T.coin.pEDTM_tdcTime"]    = 159;
    edtm3HiX["T.coin.pEDTM_tdcTime"]     = 161;

    TString histoOpenName, histoEDTMName, histoNoEDTMName;
    TH1F *histoOpen, *histoEDTM, *histoNoEDTM;

    TCanvas* c = new TCanvas("c", "canvas", 1200, 600);
    c->SetLogy();

    // Open PDF
    c->Print((pdfFilename+"[").Data());

    // Open CSV
    std::ofstream ofs;
    ofs.open(csvFilename.Data());


    ofs << "kinematics,q2,target,collimator,branch,cut,peak,trig" << std::endl;

    for (auto const trig: trigBranches) {
        for (auto const k: kinematics) {
            // Skip because we didn't have the EDTM set up
            if (data->GetQ2(k)==8) {continue;}

            std::cout << "\n" << k << std::endl;

            // histogram names
            histoOpenName = Form("%s_%s_%s", trig.Data(), k.Data(), noCut.GetTitle());
            histoOpenName.ReplaceAll("!=",".NEQ.");
            histoOpenName.ReplaceAll("==",".EQ.");

            histoEDTMName = Form("%s_%s_%s", trig.Data(), k.Data(), EDTMOnly.GetTitle());
            histoEDTMName.ReplaceAll("!=",".NEQ.");
            histoEDTMName.ReplaceAll("==",".EQ.");

            histoNoEDTMName = Form("%s_%s_%s", trig.Data(), k.Data(), noEDTM.GetTitle());
            histoNoEDTMName.ReplaceAll("!=",".NEQ.");
            histoNoEDTMName.ReplaceAll("==",".EQ.");

            // std::cout << Form("\n%s\n%s\n%s", histoOpenName.Data(), histoEDTMName.Data(), histoNoEDTMName.Data()) << std::endl;

            // Get histograms from file
            histoOpen   = (TH1F*) f->Get(histoOpenName.Data());
            histoEDTM   = (TH1F*) f->Get(histoEDTMName.Data());
            histoNoEDTM = (TH1F*) f->Get(histoNoEDTMName.Data());

            // Format histograms
            histoEDTM->SetLineColor(kRed);
            histoNoEDTM->SetLineColor(kGreen);
            histoOpen->SetLineColor(kBlack);

            // Draw histograms
            histoOpen->Draw();
            histoEDTM->Draw("SAME");
            histoNoEDTM->Draw("SAME");

            c->Modified();
            c->Update();

            // Title
            text = new TPaveText(0.15, 0.75, 0.35, 0.85, "brNDC");
            text->AddText(trig.Data());
            text->AddText(k.Data());
            text->Draw();
            c->Modified();
            c->Update();

            // Print
            c->Print(pdfFilename.Data());

            // --------------
            // Indicate limits of peaks we'll later integrate for LT calculation

            Double_t canvasMax = TMath::Power(10,c->GetUymax());
            Double_t canvasMin = TMath::Power(10,c->GetUymin());

            trigCenter = new TLine(trigCenterX[trig], canvasMin, trigCenterX[trig], canvasMax);
            trigCenter->SetLineColor(kBlue);
            trigCenter->SetLineStyle(1);
            trigCenter->Draw();
            trigLow = new TLine(trigLowX[trig], canvasMin, trigLowX[trig], canvasMax);
            trigLow->SetLineColor(kBlue);
            trigLow->SetLineStyle(2);
            trigLow->Draw();
            trigHi = new TLine(trigHiX[trig], canvasMin, trigHiX[trig], canvasMax);
            trigHi->SetLineColor(kBlue);
            trigHi->SetLineStyle(2);
            trigHi->Draw();

            edtm1Center = new TLine(edtm1CenterX[trig], canvasMin, edtm1CenterX[trig], canvasMax);
            edtm1Center->SetLineColor(kBlue);
            edtm1Center->SetLineStyle(1);
            edtm1Center->Draw();
            edtm1Low = new TLine(edtm1LowX[trig], canvasMin, edtm1LowX[trig], canvasMax);
            edtm1Low->SetLineColor(kBlue);
            edtm1Low->SetLineStyle(2);
            edtm1Low->Draw();
            edtm1Hi = new TLine(edtm1HiX[trig], canvasMin, edtm1HiX[trig], canvasMax);
            edtm1Hi->SetLineColor(kBlue);
            edtm1Hi->SetLineStyle(2);
            edtm1Hi->Draw();

            edtm2Center = new TLine(edtm2CenterX[trig], canvasMin, edtm2CenterX[trig], canvasMax);
            edtm2Center->SetLineColor(kBlue);
            edtm2Center->SetLineStyle(1);
            edtm2Center->Draw();
            edtm2Low = new TLine(edtm2LowX[trig], canvasMin, edtm2LowX[trig], canvasMax);
            edtm2Low->SetLineColor(kBlue);
            edtm2Low->SetLineStyle(2);
            edtm2Low->Draw();
            edtm2Hi = new TLine(edtm2HiX[trig], canvasMin, edtm2HiX[trig], canvasMax);
            edtm2Hi->SetLineColor(kBlue);
            edtm2Hi->SetLineStyle(2);
            edtm2Hi->Draw();

            edtm3Center = new TLine(edtm3CenterX[trig], canvasMin, edtm3CenterX[trig], canvasMax);
            edtm3Center->SetLineColor(kBlue);
            edtm3Center->SetLineStyle(1);
            edtm3Center->Draw();
            edtm3Low = new TLine(edtm3LowX[trig], canvasMin, edtm3LowX[trig], canvasMax);
            edtm3Low->SetLineColor(kBlue);
            edtm3Low->SetLineStyle(2);
            edtm3Low->Draw();
            edtm3Hi = new TLine(edtm3HiX[trig], canvasMin, edtm3HiX[trig], canvasMax);
            edtm3Hi->SetLineColor(kBlue);
            edtm3Hi->SetLineStyle(2);
            edtm3Hi->Draw();

            // --------------
            // calculate trigger counts
            ttrig[trigCenterX[trig]]  = histoOpen->Integral(histoOpen->GetXaxis()->FindBin(trigLowX[trig]),
                                                      histoOpen->GetXaxis()->FindBin(trigHiX[trig]));
            ttrig[edtm1CenterX[trig]] = histoOpen->Integral(histoOpen->GetXaxis()->FindBin(edtm1LowX[trig]),
                                                      histoOpen->GetXaxis()->FindBin(edtm1HiX[trig]));
            ttrig[edtm2CenterX[trig]] = histoOpen->Integral(histoOpen->GetXaxis()->FindBin(edtm2LowX[trig]),
                                                      histoOpen->GetXaxis()->FindBin(edtm2HiX[trig]));
            ttrig[edtm3CenterX[trig]] = histoOpen->Integral(histoOpen->GetXaxis()->FindBin(edtm3LowX[trig]),
                                                      histoOpen->GetXaxis()->FindBin(edtm3HiX[trig]));

            tedtm[trigCenterX[trig]]  = histoEDTM->Integral(histoEDTM->GetXaxis()->FindBin(trigLowX[trig]),
                                                      histoEDTM->GetXaxis()->FindBin(trigHiX[trig]));
            tedtm[edtm1CenterX[trig]] = histoEDTM->Integral(histoEDTM->GetXaxis()->FindBin(edtm1LowX[trig]),
                                                      histoEDTM->GetXaxis()->FindBin(edtm1HiX[trig]));
            tedtm[edtm2CenterX[trig]] = histoEDTM->Integral(histoEDTM->GetXaxis()->FindBin(edtm2LowX[trig]),
                                                      histoEDTM->GetXaxis()->FindBin(edtm2HiX[trig]));
            tedtm[edtm3CenterX[trig]] = histoEDTM->Integral(histoEDTM->GetXaxis()->FindBin(edtm3LowX[trig]),
                                                      histoEDTM->GetXaxis()->FindBin(edtm3HiX[trig]));

            // print to csvs
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "open",      trigCenterX[trig],  ttrig[trigCenterX[trig]])  << std::endl;
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "open",      edtm1CenterX[trig], ttrig[edtm1CenterX[trig]]) << std::endl;
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "open",      edtm2CenterX[trig], ttrig[edtm2CenterX[trig]]) << std::endl;
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "open",      edtm3CenterX[trig], ttrig[edtm3CenterX[trig]]) << std::endl;
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "edtmfired", trigCenterX[trig],  tedtm[trigCenterX[trig]])  << std::endl;
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "edtmfired", edtm1CenterX[trig], tedtm[edtm1CenterX[trig]]) << std::endl;
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "edtmfired", edtm2CenterX[trig], tedtm[edtm2CenterX[trig]]) << std::endl;
            ofs << Form("%s,%f,%s,%s,%s,%s,%f,%d", k.Data(), data->GetQ2(k), data->GetTarget(k).Data(), data->GetCollimator(k).Data(), trig.Data(), "edtmfired", edtm3CenterX[trig], tedtm[edtm3CenterX[trig]]) << std::endl;

            // Print
            c->Print(pdfFilename.Data());

        } // loop over kinematics
    } // loop over branches

    // Close PDF and CSV
    c->Print((pdfFilename+"]").Data());
    ofs.close();
}
