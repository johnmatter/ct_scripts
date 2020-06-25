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

// run this before print_tdcTimeRaw_histos_to_pdf
void generate_tdcTimeRaw_histos() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // Which triggers
    std::vector<TString> trigBranches = {"T.coin.pEDTM_tdcTimeRaw", "T.coin.pTRIG6_ROC2_tdcTimeRaw"};

    // Open file so we can save histograms
    TFile *f = new TFile("ct_tdcTimeRaw.root", "RECREATE");

    // key is histoName
    std::map<TString, TH1F*> histos;

    // Cuts
    TCut EDTMOnly = "T.coin.pEDTM_tdcTimeRaw!=0";
    TCut noEDTM = "T.coin.pEDTM_tdcTimeRaw==0";
    TCut noCut = "";
    std::vector<TCut> cuts = {noCut, EDTMOnly, noEDTM};

    // ------------------------------------------------------------------------
    // Draw
    Int_t xBins = 5000;
    Double_t xMin = 0;
    Double_t xMax = 5000;
    TString histoName, drawStr;

    TCanvas* c = new TCanvas("c", "canvas", 800, 600);
    c->SetLogy();

    for (auto const trig: trigBranches) {
        std::cout << Form("Drawing %s", trig.Data()) << std::endl;
        for (auto const k: kinematics) {
            for (auto const cut: cuts) {
                // Skip because we didn't have the EDTM set up
                if (data->GetQ2(k)==8) {continue;}

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
