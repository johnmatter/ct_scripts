#include <utility>
#include <vector>
#include <map>

#include <TCut.h>
#include <TH2F.h>
#include <TCanvas.h>

#include <CTData.h>
#include <CTCuts.h>
// Do we have more events in the HMS Cherenkovs broken mirror region for
// carbon runs than for hydrogen runs? This might explain the lower efficiency
// for carbon runs with Q^2=8 and Q^2=14.
//
// This macro investigates the question by plotting the ratio of LH2 to C12
// events for each value of Q^2. The ratio is calculated over the face of the detector.
void hcer_position2() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::vector<TH2F*> ratios;

    TCut cutShould = cuts->Get("hCalCut") && cuts->Get("hBetaCut");

    std::vector<Int_t> Q2s = {8,10,12,14};

    TString name;
    TString drawThis;
    TChain *chain;

    // Draw TH2Fs from which to calculate ratios
    for (auto const &q : Q2s) {

        std::cout << "Calculating Q^2 = " << q << std::endl;

        // Draw TH2 for C12
        name = Form("C12_Q2_%d", q);
        chain = data->GetChain(name);
        drawThis = Form("H.cer.xAtCer:H.cer.yAtCer>>%s(25,-50,50,50,-100,100)", name.Data());
        chain->Draw(drawThis.Data(), cutShould);
    }


    // Calculate ratio TH2Fs ------------------------
    TH2F* hNumerator;
    TH2F* hDenominator;
    // 8 to 10
    name = Form("C12 ratio, Q^{2}=8 to Q^{2}=10");
    ratios.push_back(new TH2F(name.Data(), name.Data(), 25,-50,50,50,-100,100));
    hNumerator   = (TH2F*) gDirectory->Get("C12_Q2_8");
    hDenominator = (TH2F*) gDirectory->Get("C12_Q2_10");
    ratios.back()->Divide(hNumerator, hDenominator,
                          hDenominator->GetEntries(), hNumerator->GetEntries());

    // 8 to 12
    name = Form("C12 ratio, Q^{2}=8 to Q^{2}=12");
    ratios.push_back(new TH2F(name.Data(), name.Data(), 25,-50,50,50,-100,100));
    hNumerator   = (TH2F*) gDirectory->Get("C12_Q2_8");
    hDenominator = (TH2F*) gDirectory->Get("C12_Q2_12");
    ratios.back()->Divide(hNumerator, hDenominator,
                          hDenominator->GetEntries(), hNumerator->GetEntries());

    // 14 to 10
    name = Form("C12 ratio, Q^{2}=14 to Q^{2}=10");
    ratios.push_back(new TH2F(name.Data(), name.Data(), 25,-50,50,50,-100,100));
    hNumerator   = (TH2F*) gDirectory->Get("C12_Q2_14");
    hDenominator = (TH2F*) gDirectory->Get("C12_Q2_10");
    ratios.back()->Divide(hNumerator, hDenominator,
                          hDenominator->GetEntries(), hNumerator->GetEntries());

    // 14 to 12
    name = Form("C12 ratio, Q^{2}=14 to Q^{2}=12");
    ratios.push_back(new TH2F(name.Data(), name.Data(), 25,-50,50,50,-100,100));
    hNumerator   = (TH2F*) gDirectory->Get("C12_Q2_14");
    hDenominator = (TH2F*) gDirectory->Get("C12_Q2_12");
    ratios.back()->Divide(hNumerator, hDenominator,
                          hDenominator->GetEntries(), hNumerator->GetEntries());

    // Get max/min so we can adjust Z color bar to be the same for all TH2Fs
    Double_t minZ=0;
    Double_t maxZ=1;
    for (auto const &r: ratios) {
        if (r->GetMinimum() < minZ) { minZ = r->GetMinimum(); }
        if (r->GetMaximum() > maxZ) { maxZ = r->GetMaximum(); }
    }

    // Plot ratios
    TCanvas* cHcer = new TCanvas("cHcer", "HCer Efficiency", 1024, 640);
    cHcer->Print("hcer2.pdf["); // open PDF
    cHcer->Divide(2,2);

    int pad=1;
    for (auto const &r: ratios) {
        cHcer->cd(pad);

        r->SetMinimum(minZ);
        r->SetMaximum(maxZ);
        r->Draw("colz");

        pad++;
    }
    cHcer->Print("hcer2.pdf"); // write page to PDF

    cHcer->Print("hcer2.pdf]"); // close fPDF
}
