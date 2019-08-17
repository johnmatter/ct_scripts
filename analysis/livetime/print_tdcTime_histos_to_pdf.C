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
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // Which triggers
    std::vector<TString> trigBranches = {"T.coin.pEDTM_tdcTime", "T.coin.pTRIG6_ROC2_tdcTime"};

    // PDF to save histos in
    TString pdfFilename = "trigger_tdcTime_histos.pdf";

    // CSV to save counts in peaks
    TString csvFilename= "trigger_tdcTime_peak_counts.csv";

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

    Double_t trigCenterX = 274;
    Double_t trigLowX = 272;
    Double_t trigHiX = 276;

    Double_t edtm1CenterX = 285;
    Double_t edtm1LowX = 283;
    Double_t edtm1HiX = 287;

    Double_t edtm2CenterX = 331;
    Double_t edtm2LowX = 329;
    Double_t edtm2HiX = 333;

    Int_t ttrig, tedtm;

    TString histoOpenName, histoEDTMName, histoNoEDTMName;
    TH1F *histoOpen, *histoEDTM, *histoNoEDTM;

    TCanvas* c = new TCanvas("c", "canvas", 1200, 600);
    c->SetLogy();

    // Open PDF
    c->Print((pdfFilename+"[").Data());

    // Open CSV
    std::ofstream ofs;
    ofs.open(csvFilename.Data());


    ofs << "kinematics,q2,target,collimator,ttrig,tedtm" << std::endl;

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

            // Get histograms from file
            histoOpen   = (TH1F*) f->Get(histoOpenName.Data());
            histoEDTM   = (TH1F*) f->Get(histoEDTMName.Data());
            histoNoEDTM = (TH1F*) f->Get(histoNoEDTMName.Data());

            // Format histograms
            histoEDTM->SetLineColor(kRed);
            histoNoEDTM->SetLineColor(kGreen);
            histoOpen->SetLineColor(kBlack);

            // Draw histograms
            histoEDTM->Draw();
            histoNoEDTM->Draw("SAME");
            histoOpen->Draw("SAME");

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

            if (trig.Contains("pTRIG6_ROC2")) {
                Double_t canvasMax = TMath::Power(10,c->GetUymax());
                Double_t canvasMin = TMath::Power(10,c->GetUymin());

                trigCenter = new TLine(trigCenterX, canvasMin, trigCenterX, canvasMax);
                trigCenter->SetLineColor(kBlue);
                trigCenter->SetLineStyle(1);
                trigCenter->Draw();
                trigLow = new TLine(trigLowX, canvasMin, trigLowX, canvasMax);
                trigLow->SetLineColor(kBlue);
                trigLow->SetLineStyle(2);
                trigLow->Draw();
                trigHi = new TLine(trigHiX, canvasMin, trigHiX, canvasMax);
                trigHi->SetLineColor(kBlue);
                trigHi->SetLineStyle(2);
                trigHi->Draw();

                edtm1Center = new TLine(edtm1CenterX, canvasMin, edtm1CenterX, canvasMax);
                edtm1Center->SetLineColor(kBlue);
                edtm1Center->SetLineStyle(1);
                edtm1Center->Draw();
                edtm1Low = new TLine(edtm1LowX, canvasMin, edtm1LowX, canvasMax);
                edtm1Low->SetLineColor(kBlue);
                edtm1Low->SetLineStyle(2);
                edtm1Low->Draw();
                edtm1Hi = new TLine(edtm1HiX, canvasMin, edtm1HiX, canvasMax);
                edtm1Hi->SetLineColor(kBlue);
                edtm1Hi->SetLineStyle(2);
                edtm1Hi->Draw();

                edtm2Center = new TLine(edtm2CenterX, canvasMin, edtm2CenterX, canvasMax);
                edtm2Center->SetLineColor(kBlue);
                edtm2Center->SetLineStyle(1);
                edtm2Center->Draw();
                edtm2Low = new TLine(edtm2LowX, canvasMin, edtm2LowX, canvasMax);
                edtm2Low->SetLineColor(kBlue);
                edtm2Low->SetLineStyle(2);
                edtm2Low->Draw();
                edtm2Hi = new TLine(edtm2HiX, canvasMin, edtm2HiX, canvasMax);
                edtm2Hi->SetLineColor(kBlue);
                edtm2Hi->SetLineStyle(2);
                edtm2Hi->Draw();

                // calculate trigger counts
                ttrig=0;
                tedtm=0;

                ttrig += histoOpen->Integral(histoOpen->GetXaxis()->FindBin(trigLowX),
                                              histoOpen->GetXaxis()->FindBin(trigHiX));
                ttrig += histoOpen->Integral(histoOpen->GetXaxis()->FindBin(edtm1LowX),
                                              histoOpen->GetXaxis()->FindBin(edtm1HiX));
                ttrig += histoOpen->Integral(histoOpen->GetXaxis()->FindBin(edtm2LowX),
                                              histoOpen->GetXaxis()->FindBin(edtm2HiX));

                tedtm += histoEDTM->Integral(histoEDTM->GetXaxis()->FindBin(trigLowX),
                                              histoEDTM->GetXaxis()->FindBin(trigHiX));
                tedtm += histoEDTM->Integral(histoEDTM->GetXaxis()->FindBin(edtm1LowX),
                                              histoEDTM->GetXaxis()->FindBin(edtm1HiX));
                tedtm += histoEDTM->Integral(histoEDTM->GetXaxis()->FindBin(edtm2LowX),
                                              histoEDTM->GetXaxis()->FindBin(edtm2HiX));

                // print to csv
                ofs << k << ","
					<< data->GetQ2(k) << ","
					<< data->GetTarget(k) << ","
					<< data->GetCollimator(k) << ","
					<< ttrig << ","
					<< tedtm << std::endl;

                // Print
                c->Print(pdfFilename.Data());
            }
        }
    }

    // Close PDF and CSV
    c->Print((pdfFilename+"]").Data());
    ofs.close();
}
