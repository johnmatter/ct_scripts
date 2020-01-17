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
int main() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // PDF to print to
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/efficiency/coin/pid_per_run/distributions.pdf";

    TFile *fWrite = new TFile("/home/jmatter/ct_scripts/analysis/efficiency/coin/pid_per_run/distributions.root", "RECREATE");

    // Events-per-delta histograms for weighting
    std::map<TString, TH1F*> histograms;

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // Which detectors
    std::vector<TString> detectors = {"hCal", "hCer", "pCer"};

    // Cuts per detector
    std::map<TString, TCut> detectorShouldCuts;
    std::map<TString, TCut> detectorDidCuts;
    std::map<TString, TCut> detectorDidntCuts;

    detectorShouldCuts["hCer"] = cuts->Get("hHodoTimeCut") &&
                                 cuts->Get("pHodoTimeCut") &&
                                 cuts->Get("hDeltaCut") &&
                                 cuts->Get("pDeltaCut") &&
                                 cuts->Get("hBetaCut") &&
                                 cuts->Get("pBetaCut") &&
                                 cuts->Get("hCalCut");

    detectorShouldCuts["hCal"] = cuts->Get("hHodoTimeCut") &&
                                 cuts->Get("pHodoTimeCut") &&
                                 cuts->Get("hDeltaCut") &&
                                 cuts->Get("pDeltaCut") &&
                                 cuts->Get("hBetaCut") &&
                                 cuts->Get("pBetaCut") &&
                                 cuts->Get("hCerCut");

    detectorShouldCuts["pCer"] = cuts->Get("hHodoTimeCut") &&
                                 cuts->Get("pHodoTimeCut") &&
                                 cuts->Get("hDeltaCut") &&
                                 cuts->Get("pDeltaCut") &&
                                 cuts->Get("hBetaCut") &&
                                 cuts->Get("pBetaCut");
                                 // cuts->Get("hCalCut") &&
                                 // cuts->Get("hCerCut");

    detectorDidCuts["hCer"] = detectorShouldCuts["hCer"] && cuts->Get("hCerCut");
    detectorDidCuts["hCal"] = detectorShouldCuts["hCal"] && cuts->Get("hCalCut");
    detectorDidCuts["pCer"] = detectorShouldCuts["pCer"] && cuts->Get("pCerCut");

    detectorDidntCuts["hCer"] = detectorShouldCuts["hCer"] && !cuts->Get("hCerCut");
    detectorDidntCuts["hCal"] = detectorShouldCuts["hCal"] && !cuts->Get("hCalCut");
    detectorDidntCuts["pCer"] = detectorShouldCuts["pCer"] && !cuts->Get("pCerCut");

    TCut shouldCut, didCut, didntCut, openCut;

    // Branches to draw
    std::vector<TString> branches;
    std::map<TString, TString> branchBinning;

    branches.push_back("H.cal.etottracknorm");
    branchBinning["H.cal.etottracknorm"] = "(150,0,1.5)";

    branches.push_back("H.cer.npeSum");
    branchBinning["H.cer.npeSum"] = "(160,0,40)";

    branches.push_back("P.ngcer.npeSum");
    branchBinning["P.ngcer.npeSum"] = "(160,0,40)";

    // ------------------------------------------------------------------------
    // Generate histograms
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            for (auto const &b : branches) {
                TString drawMe, histoName;

                // Define cuts
                shouldCut = detectorShouldCuts[d];
                didCut    = detectorDidCuts[d];
                didntCut  = detectorDidntCuts[d];
                openCut   = cuts->Get("deltaCut") && cuts->Get("betaCut");

                // Target-specific cuts
                if (k.Contains("C12")) {
                    shouldCut = shouldCut && cuts->Get("pC12EMissPMissCut");
                    didCut    = didCut    && cuts->Get("pC12EMissPMissCut");
                    didntCut  = didntCut  && cuts->Get("pC12EMissPMissCut");
                }

                // Should
                histoName = Form("%s_%s_should_%s", k.Data(), d.Data(), b.Data());
                drawMe = Form("%s>>%s%s", b.Data(), histoName.Data(), branchBinning[b].Data());
                data->GetChain(k)->Draw(drawMe.Data(), shouldCut, "goff");
                histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
                fWrite->WriteObject(histograms[histoName], histoName.Data());

                // Did
                histoName = Form("%s_%s_did_%s", k.Data(), d.Data(), b.Data());
                drawMe = Form("%s>>%s%s", b.Data(), histoName.Data(), branchBinning[b].Data());
                data->GetChain(k)->Draw(drawMe.Data(), didCut, "goff");
                histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
                fWrite->WriteObject(histograms[histoName], histoName.Data());

                // Didnt
                histoName = Form("%s_%s_didnt_%s", k.Data(), d.Data(), b.Data());
                drawMe = Form("%s>>%s%s", b.Data(), histoName.Data(), branchBinning[b].Data());
                data->GetChain(k)->Draw(drawMe.Data(), didntCut, "goff");
                histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
                fWrite->WriteObject(histograms[histoName], histoName.Data());

                // Open
                histoName = Form("%s_%s_open_%s", k.Data(), d.Data(), b.Data());
                drawMe = Form("%s>>%s%s", b.Data(), histoName.Data(), branchBinning[b].Data());
                data->GetChain(k)->Draw(drawMe.Data(), openCut, "goff");
                histograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
                fWrite->WriteObject(histograms[histoName], histoName.Data());
            }
        }
    }

    // ------------------------------------------------------------------------
    // Save to PDF
    TCanvas* c = new TCanvas("c", "c", 1024, 640);

    c->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            for (auto const &b : branches) {
                TH1F* hShould = histograms[Form("%s_%s_should_%s", k.Data(), d.Data(), b.Data())];
                TH1F* hDid    = histograms[Form("%s_%s_did_%s",    k.Data(), d.Data(), b.Data())];
                TH1F* hDidnt  = histograms[Form("%s_%s_didnt_%s",  k.Data(), d.Data(), b.Data())];
                TH1F* hOpen   = histograms[Form("%s_%s_open_%s",   k.Data(), d.Data(), b.Data())];

                // Set title. Only need to set one, since we're overlaying them
                hOpen->SetTitle(Form("%s: %s should&did&...: %s", k.Data(), d.Data(), b.Data()));

                // Line color
                hShould->SetLineColor(kBlue+1);
                hDid->SetLineColor(kGreen+1);
                hDidnt->SetLineColor(kRed+1);
                hOpen->SetLineColor(kBlack);

                // Fill color
                hShould->SetFillColor(kBlue+1);
                hDid->SetFillColor(kGreen+1);
                hDidnt->SetFillColor(kRed+1);
                hOpen->SetFillColor(kBlack);

                // Fill style
                hShould->SetFillStyle(3002);
                hDid->SetFillStyle(3345);
                hDidnt->SetFillStyle(3354);
                hOpen->SetFillStyle(3002);

                // Scale down
                // hShould->Scale(1/hShould->Integral());
                // hDid->Scale(1/hDid->Integral());
                // hDidnt->Scale(1/hDidnt->Integral());
                // hOpen->Scale(1/hOpen->Integral());

                hOpen->Draw("hist");
                hShould->Draw("same hist");
                hDid->Draw("same hist");
                hDidnt->Draw("same hist");

                c->Print(pdfFilename.Data()); // write page to PDF
            }
        }
    }
    c->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"

    // Write histos to disk
    fWrite->Close();

    return 0;
}
