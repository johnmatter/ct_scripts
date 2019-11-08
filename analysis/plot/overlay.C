#include <utility>
#include <vector>
#include <map>

#include <TCut.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>

#include <CTData.h>
#include <CTCuts.h>
void overlay(TString branch, Int_t nBins=100, Double_t loBin=0, Double_t hiBin=1, Bool_t doHms=0, Bool_t doShms=0, TCut myCut="") {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Plot
    TString drawThis;
    for (auto const &k: kinematics) {
        std::cout << "Calculating for " << k << std::endl;

        // What cut to use for drawing?
        TCut drawCut;
        if (myCut=="") {
            if (k.Contains("LH2")) {
                TString cutName = Form("coincuts%s",k.Data());
                drawCut = cuts->Get(cutName);
            }
            if (k.Contains("C12")) {
                drawCut = cuts->Get("coinCutsC12");
            }
        } else {
            drawCut = myCut;
        }

        if (doHms) {
            drawThis = Form("H.%s>>%s_H.%s(%d,%f,%f)", branch.Data(), k.Data(), branch.Data(), nBins,loBin,hiBin);
            data->GetChain(k)->Draw(drawThis.Data(), drawCut);
        }

        if (doShms) {
            drawThis = Form("P.%s>>%s_P.%s(%d,%f,%f)", branch.Data(), k.Data(), branch.Data(), nBins,loBin,hiBin);
            data->GetChain(k)->Draw(drawThis.Data(), drawCut);
        }
    }


    // Set up some drawing stuff ----------------------------------------------
    Int_t n=0; // color index
    std::vector<Int_t> colors = {kRed+1, kOrange+8, kOrange, kGreen+2,
                                 kCyan-3, kAzure-4, kBlue-3, kViolet-3,
                                 kOrange+10, kOrange+5};

    TString filename;

    // Draw HMS ---------------------------------------------------------------
    if (doHms) {
        filename = Form("H.%s.pdf",branch.Data());
        gStyle->SetOptStat(0);
        TCanvas* cHMS = new TCanvas("cHMS", "hms_canvas", 640, 640);
        cHMS->Print(Form("%s[",filename.Data())); // open PDF

        // HMS overlay
        TLegend *legHMS = new TLegend(0.1,0.7,0.3,0.9);
        n=0; // color index
        for (auto const &k: kinematics) {
            // Get histogram; make it pretty
            TString hName = Form("%s_H.%s", k.Data(), branch.Data());
            std::cout << "Draw overlay " << hName << std::endl;

            TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
            if (h!=nullptr) {
                // Make it pretty
                h->SetLineColor(colors[n++]);

                // Normalize by area under histo
                Double_t hIntegral = h->Integral();
                std::cout << "Integral=" << hIntegral << std::endl;
                h->Scale(1/hIntegral);

                // Overlay
                h->Draw("same hist");

                // Add to legend
                legHMS->AddEntry(h, k.Data(), "l");
            } else {
                std::cout << "Couldn't find " << hName << std::endl;
            }
        }
        legHMS->Draw();
        cHMS->Print(filename); // write page to PDF

        // Individual pages
        gStyle->SetOptStat(111111);
        n=0; // color index
        for (auto const &k: kinematics) {
            TString hName = Form("%s_H.%s", k.Data(), branch.Data());
            TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
            if (h!=nullptr) {
                h->Draw("");
                cHMS->Print(filename); // write page to PDF
            }
        }

        cHMS->Print(Form("%s]",filename.Data())); // close fPDF
    }

    // Draw SHMS ---------------------------------------------------------------
    if (doShms) {
        filename = Form("P.%s.pdf",branch.Data());
        gStyle->SetOptStat(0);
        TCanvas* cSHMS = new TCanvas("cSHMS", "shms_canvas", 640, 640);
        cSHMS->Print(Form("%s[",filename.Data())); // open PDF

        // SHMS overlay
        TLegend *legSHMS = new TLegend(0.1,0.7,0.3,0.9);
        n=0; // color index
        for (auto const &k: kinematics) {
            // Get histogram; make it pretty
            TString hName = Form("%s_P.%s", k.Data(), branch.Data());
            std::cout << "Draw overlay " << hName << std::endl;

            TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
            if (h!=nullptr) {
                // Make it pretty
                h->SetLineColor(colors[n++]);

                // Normalize by area under histo
                Double_t hIntegral = h->Integral();
                std::cout << "Integral=" << hIntegral << std::endl;
                h->Scale(1/hIntegral);

                // Overlay
                h->Draw("same hist");

                // Add to legend
                legSHMS->AddEntry(h, k.Data(), "l");
            } else {
                std::cout << "Couldn't find " << hName << std::endl;
            }
        }
        legSHMS->Draw();
        cSHMS->Print(filename); // write page to PDF

        // Individual pages
        gStyle->SetOptStat(111111);
        n=0; // color index
        for (auto const &k: kinematics) {
            TString hName = Form("%s_P.%s", k.Data(), branch.Data());
            TH1F *h = (TH1F*) gDirectory->Get(hName.Data());
            if (h!=nullptr) {
                h->Draw("");
                cSHMS->Print(filename); // write page to PDF
            }
        }

        cSHMS->Print(Form("%s]",filename.Data())); // close fPDF
    }
}
