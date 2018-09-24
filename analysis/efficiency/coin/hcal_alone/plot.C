#include <utility>
#include <vector>
#include <map>

#include <TCut.h>
#include <TH2F.h>
#include <TCanvas.h>

#include <CTData.h>
#include <CTCuts.h>
void plot() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    TCut cutShould = cuts->Get("hDeltaCut") && cuts->Get("hBetaCut");

    std::vector<TString> kinematics = {"LH2_Q2_8", "LH2_Q2_10", "LH2_Q2_12", "LH2_Q2_14",
                                       "C12_Q2_8", "C12_Q2_10", "C12_Q2_12", "C12_Q2_14"};

    // Plot
    for (auto const &k: kinematics) {
        std::cout << "Calculating for " << k << std::endl;

        TString drawThis = Form("(H.cal.etot-H.cal.eprtrack):H.cal.eprtrack>>%s(40,0,4,70,0,7)", k.Data());

        data->GetChain(k)->Draw(drawThis.Data(), cutShould);
    }

    // Draw
    TCanvas* c = new TCanvas("c", "hcal shower vs preshower energy", 1024, 640);
    c->Print("hcal.pdf["); // open PDF
    c->Divide(2,2);
    int pad;

    // Hydrogen
    pad=1;
    for (auto const &k: {"LH2_Q2_8", "LH2_Q2_10", "LH2_Q2_12", "LH2_Q2_14"}) {
        c->cd(pad++);
        TH2F *h = (TH2F*) gDirectory->Get(k);
        h->Draw("colz");
        h->SetTitle(k);
    }
    c->SetLogz();
    c->Print("hcal.pdf"); // write page to PDF

    // Carbon
    pad=1;
    for (auto const &k: {"C12_Q2_8", "C12_Q2_10", "C12_Q2_12", "C12_Q2_14"}) {
        c->cd(pad++);
        TH2F *h = (TH2F*) gDirectory->Get(k);
        h->Draw("colz");
        h->SetTitle(k);
    }
    c->SetLogz();
    c->Print("hcal.pdf"); // write page to PDF

    c->Print("hcal.pdf]"); // close fPDF
}
