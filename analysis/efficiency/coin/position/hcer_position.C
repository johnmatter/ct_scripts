#include <utility>
#include <vector>
#include <map>

#include <TCut.h>
#include <TCanvas.h>

#include <CTData.h>
#include <CTCuts.h>
#include <Efficiency2D.h>

void hcer_position() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::map<std::pair<TString, int>, Efficiency2D*> efficiencies;

    TString scanBranch = "H.gtr.dp";
    TCut cutShould = cuts->Get("hCerShould");
    TCut cutDid = cuts->Get("hCerDid");

    std::vector<TString> targets = {"LH2","C12"};
    std::vector<Int_t> Q2s = {8,10,12,14};

    // Calculate efficiencies
    for (auto const &t : targets) {
        for (auto const &q : Q2s) {
            std::pair<TString, int> key = std::make_pair(t,q);

            TString name = Form("%s_Q2_%d", t.Data(), q);
            efficiencies[key] = new Efficiency2D(name.Data());

            TChain* chain = data->GetChain(name);
            efficiencies[key]->SetChain(chain);

            efficiencies[key]->SetScanBranches("H.cer.yAtCer", "H.cer.xAtCer");
            efficiencies[key]->SetScanRange(25, -50, 50, 50, -100, 100);

            efficiencies[key]->SetShouldCut(cutShould);
            efficiencies[key]->SetDidCut(cutDid);

            efficiencies[key]->SetEvents(-1);
            efficiencies[key]->Init();
            efficiencies[key]->Calculate();

            TString title = Form("%s Q^{2} = %.1f GeV^{2}", t.Data(), data->GetQ2(name));
            efficiencies[key]->SetTitle(title.Data());
        }
    }

    // Plot efficiencies
    TCanvas* cHcer = new TCanvas("cHcer", "HCer Efficiency", 1024, 640);
    cHcer->Print("hcer_position.pdf["); // open PDF
    cHcer->Divide(2,2);
    int pad=1;
    TString t = "LH2";
    for (auto const &q : Q2s) {
        cHcer->cd(pad);

        std::pair<TString, int> key = std::make_pair(t,q);
        efficiencies[key]->GetTEfficiency()->Draw("colz");

        pad++;
    }
    cHcer->Print("hcer_position.pdf"); // write page to PDF

    pad=1;
    t = "C12";
    for (auto const &q : Q2s) {
        cHcer->cd(pad);

        std::pair<TString, int> key = std::make_pair(t,q);
        efficiencies[key]->GetTEfficiency()->Draw("colz");

        pad++;
    }
    cHcer->Print("hcer_position.pdf"); // write page to PDF

    cHcer->Print("hcer_position.pdf]"); // close fPDF
}
