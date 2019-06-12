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

// Investigating what the distribution of delta look like
// for events that should, do, and don't fire the HMS Cherenkov.
void plot_hcer_delta() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_pass3.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to print to
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/efficiency/coin/distributions/distributions_hcer_delta.pdf";

     // Events-per-delta histograms for weighting
    std::map<TString, TH1F*> histograms;

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // ------------------------------------------------------------------------
    // Generate histograms
    for (auto const &k : kinematics) {
        TString drawMe, histoName;

        // Define cuts
        TCut shouldCut = cuts->Get("betaCut") && cuts->Get("hCalCut") && cuts->Get("pCerCut");
        TCut didCut    = cuts->Get("betaCut") && cuts->Get("hCalCut") && cuts->Get("pCerCut") && cuts->Get("hCerCut");
        TCut didntCut  = cuts->Get("betaCut") && cuts->Get("hCalCut") && cuts->Get("pCerCut") && !(cuts->Get("hCerCut"));
        TCut openCut   = cuts->Get("betaCut");

        // Target-specific cuts
        TString targetCut = Form("p%sEMissPMissCut", k(0,3).Data()); // Assume target is first 3 characters; not true for Al
        shouldCut = shouldCut && cuts->Get(targetCut);
        didCut    = didCut    && cuts->Get(targetCut);
        didntCut  = didntCut  && cuts->Get(targetCut);

        // Should
        histoName = Form("%s_should", k.Data());
        drawMe = Form("H.gtr.dp>>%s(60,-15,15)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), shouldCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

        // Did
        histoName = Form("%s_did", k.Data());
        drawMe = Form("H.gtr.dp>>%s(60,-15,15)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), didCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

        // Didnt
        histoName = Form("%s_didnt", k.Data());
        drawMe = Form("H.gtr.dp>>%s(60,-15,15)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), didntCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

        // Open
        histoName = Form("%s_open", k.Data());
        drawMe = Form("H.gtr.dp>>%s(60,-15,15)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), openCut, "goff");
        histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());

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
        hOpen->SetTitle(Form("%s: H.gtr.dp", k.Data()));

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
}
