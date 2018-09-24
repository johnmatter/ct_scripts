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
void hcer_position() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::map<int, TH2F*> ratios;

    TCut cutShould = cuts->Get("hCalCut") && cuts->Get("hBetaCut");

    std::vector<Int_t> Q2s = {8,10,12,14};

    TString name;
    TString drawThis;
    TChain *chain;

    // Calculate ratios
    for (auto const &q : Q2s) {

        std::cout << "Calculating Q^2 = " << q << std::endl;

        // Draw TH2 for LH2
        name = Form("LH2_Q2_%d", q);
        chain = data->GetChain(name);
        drawThis = Form("H.cer.xAtCer:H.cer.yAtCer>>%s(20,-40,40,25,-70,-20)", name.Data());
        chain->Draw(drawThis.Data(), cutShould);
        TH2F* hLH2 = (TH2F*) gDirectory->Get(name);

        // Draw TH2 for C12
        name = Form("C12_Q2_%d", q);
        chain = data->GetChain(name);
        drawThis = Form("H.cer.xAtCer:H.cer.yAtCer>>%s(20,-40,40,25,-70,-20)", name.Data());
        chain->Draw(drawThis.Data(), cutShould);
        TH2F* hC12 = (TH2F*) gDirectory->Get(name);

        // Calculate ratio TH2F
        name = Form("LH2/C12 ratio, Q^{2}=%d", q);
        ratios[q] = new TH2F(name.Data(), name.Data(), 20,-40,40,25,-70,-20);
        ratios[q]->Divide(hLH2, hC12);
    }

    // Get max/min so we can adjust Z color bar to be the same for all TH2Fs
    Double_t minZ=0;
    Double_t maxZ=1;
    for (auto const &q : Q2s) {
        if (ratios[q]->GetMinimum() < minZ) { minZ = ratios[q]->GetMinimum(); }
        if (ratios[q]->GetMaximum() > maxZ) { maxZ = ratios[q]->GetMaximum(); }
    }

    // Plot ratios
    TCanvas* cHcer = new TCanvas("cHcer", "HCer Efficiency", 1024, 640);
    cHcer->Print("hcer.pdf["); // open PDF
    cHcer->Divide(2,2);

    int pad=1;
    TString t = "LH2";
    for (auto const &q : Q2s) {
        cHcer->cd(pad);

        // Draw "axes", "contents", "statistics box"
        ratios[q]->SetMinimum(minZ);
        ratios[q]->SetMaximum(maxZ);
        ratios[q]->Draw("colz");
        cHcer->Update();

        pad++;
    }
    cHcer->Print("hcer.pdf"); // write page to PDF

    cHcer->Print("hcer.pdf]"); // close fPDF
}
