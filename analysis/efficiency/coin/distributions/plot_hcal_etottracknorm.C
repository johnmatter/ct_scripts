#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>
#include <TH1F.h>

#include <Efficiency1D.h>
#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

// Investigating what the distribution of coincidence times look like
// for events that should, do, and don't fire the HMS Calorimeter.
void plot_hcal_etottracknorm() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to print to
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/efficiency/coin/distributions/distributions_hcal_etottracknorm.pdf";

    TFile *fWrite = new TFile("/home/jmatter/ct_scripts/analysis/efficiency/coin/distributions/distributions_hcal_etottracknorm.root", "RECREATE");

    // Events-per-delta histograms for weighting
    std::map<TString, TH1F*> histograms;

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // ------------------------------------------------------------------------
    // Generate histograms
    for (auto const &k : kinematics) {
        TString drawMe, histoName;

        // Define cuts
        TCut shouldCut = cuts->Get("deltaCut") && cuts->Get("betaCut") && cuts->Get("hCalCut") && cuts->Get("pCerCut");
        TCut didCut    = cuts->Get("deltaCut") && cuts->Get("betaCut") && cuts->Get("hCalCut") && cuts->Get("pCerCut") && cuts->Get("hCerCut");
        TCut didntCut  = cuts->Get("deltaCut") && cuts->Get("betaCut") && cuts->Get("hCalCut") && cuts->Get("pCerCut") && !(cuts->Get("hCerCut"));
        TCut openCut   = cuts->Get("deltaCut") && cuts->Get("betaCut");

        // Target-specific cuts
        TString targetCut = Form("p%sEMissPMissCut", k(0,3).Data()); // Assume target is first 3 characters; not true for A;
        shouldCut = shouldCut && cuts->Get(targetCut);
        didCut    = didCut    && cuts->Get(targetCut);
        didntCut  = didntCut  && cuts->Get(targetCut);

        // Should
        histoName = Form("%s_should", k.Data());
        drawMe = Form("H.cal.etottracknorm>>%s(150,0,1.5)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), shouldCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
        fWrite->WriteObject(histograms[histoName], histoName.Data());

        // Did
        histoName = Form("%s_did", k.Data());
        drawMe = Form("H.cal.etottracknorm>>%s(150,0,1.5)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), didCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
        fWrite->WriteObject(histograms[histoName], histoName.Data());

        // Didnt
        histoName = Form("%s_didnt", k.Data());
        drawMe = Form("H.cal.etottracknorm>>%s(150,0,1.5)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), didntCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
        fWrite->WriteObject(histograms[histoName], histoName.Data());

        // Open
        histoName = Form("%s_open", k.Data());
        drawMe = Form("H.cal.etottracknorm>>%s(150,0,1.5)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), openCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
        fWrite->WriteObject(histograms[histoName], histoName.Data());

    }

    // ------------------------------------------------------------------------
    // Save to PDF
    TCanvas* c = new TCanvas("c", "c", 1024, 640);

    c->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["
    for (auto const &k : kinematics) {
        TH1F* hShould = histograms[Form("%s_should", k.Data())];
        TH1F* hDid    = histograms[Form("%s_did",    k.Data())];
        TH1F* hDidnt  = histograms[Form("%s_didnt",  k.Data())];
        TH1F* hOpen   = histograms[Form("%s_open",  k.Data())];

        // Set title. Only need to set one, since we're overlaying them
        hOpen->SetTitle(Form("%s: H.cal.etottracknorm", k.Data()));

        // Line color
        hShould->SetLineColor(kBlue+1);
        hDid->SetLineColor(kGreen+1);
        hDidnt->SetLineColor(kRed+1);
        hOpen->SetLineColor(kBlack);

        // Fill color
        hShould->SetFillColor(kBlue+1);
        hDid->SetFillColor(kGreen+1);
        hDidnt->SetFillColor(kRed+1);
        // hOpen->SetFillColor(kBlack);

        // Fill style
        hShould->SetFillStyle(3002);
        hDid->SetFillStyle(3345);
        hDidnt->SetFillStyle(3354);
        // hOpen->SetFillStyle(3002);

        // Scale down
        // hShould->Scale(1/hShould->Integral());
        // hDid->Scale(1/hDid->Integral());
        // hDidnt->Scale(1/hDidnt->Integral());
        // hOpen->Scale(1/hOpen->Integral());

        hOpen->Draw("hist");
        // hShould->Draw("same hist");
        hDid->Draw("same hist");
        hDidnt->Draw("same hist");

        c->Print(pdfFilename.Data()); // write page to PDF
    }
    c->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"

    // Write histos to disk
    fWrite->Close();
}
