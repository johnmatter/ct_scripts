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
#include <TFile.h>
#include <TPaveText.h>
#include <TLine.h>

#include <CTData.h>

// runthis before print_tdcTime_histos_to_pdf
void generate_tdcTime_histos() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // Which triggers
    std::vector<TString> trigBranches = {"T.coin.pEDTM_tdcTime", "T.coin.pTRIG6_ROC2_tdcTime"};

    // PDF to save histos in
    TString pdfFilename = "trigger_tdcTime_histos.pdf";

    // Open file so we can save histograms
    TFile *f = new TFile("ct_tdcTime.root", "RECREATE");

    // key is <trigger,kinematics,cut.GetTitle()>
    std::map<TString, TH1F*> histos;

    // Cuts
    TCut EDTMOnly = "T.coin.pEDTM_tdcTimeRaw!=0";
    TCut noEDTM = "T.coin.pEDTM_tdcTimeRaw==0";
    TCut noCut = "";
    std::vector<TCut> cuts = {noCut, EDTMOnly, noEDTM};

    // ------------------------------------------------------------------------
    // Draw
    Int_t xBins = 500;
    Double_t xMin = 0;
    Double_t xMax = 500;
    TString histoName, drawStr;

    TCanvas* c = new TCanvas("c", "canvas", 800, 600);
    c->SetLogy();

    for (auto const trig: trigBranches) {
        std::cout << Form("Drawing %s", trig.Data()) << std::endl;
        for (auto const k: kinematics) {
            for (auto const cut: cuts) {
                // Skip because we didn't have the EDTM set up
                if (data->GetQ2(k)==8) {continue;}

                // key
                // std::tuple<TString,TString,TString> key = std::make_tuple(trig,k,cut.GetTitle());

                // Format
                histoName = Form("%s_%s_%s", trig.Data(), k.Data(), cut.GetTitle());
                histoName.ReplaceAll("!=",".NEQ.");
                histoName.ReplaceAll("==",".EQ.");

                std::cout << histoName << std::endl;
                drawStr = Form("%s>>%s(%d,%f,%f)",
                               trig.Data(), histoName.Data(),
                               xBins, xMin, xMax);

                // Draw
                data->GetChain(k)->Draw(drawStr.Data(), cut);

                // Add to map for printing PDF
                histos[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

                // Write to root file
                histos[histoName]->Write();
            }
        }
    }
}
