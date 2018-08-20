#include <utility>
#include <vector>
#include <map>

#include <TCut.h>
#include <TCanvas.h>

#include <CTData.h>
#include <CTCuts.h>
#include <Efficiency1D.h>

void hcal_delta() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::map<std::pair<TString, int>, Efficiency1D*> efficiencies;

    TString scanBranch = "H.gtr.dp";
    TCut cutShould = cuts->Get("hCalShould");
    TCut cutDid = cuts->Get("hCalDid");

    std::vector<TString> targets = {"LH2","C12"};
    std::vector<Int_t> Q2s = {8,10,12,14};

    // Calculate efficiencies
    for (auto const &t : targets) {
        for (auto const &q : Q2s) {
            std::pair<TString, int> key = std::make_pair(t,q);

            TString name = Form("%s_Q2_%d", t.Data(), q);
            efficiencies[key] = new Efficiency1D(name.Data());

            TChain* chain = data->GetChain(name);
            efficiencies[key]->SetChain(chain);

            efficiencies[key]->SetScanBranch(scanBranch);
            efficiencies[key]->SetScanRange(100, -20, 20);

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
    TCanvas* cHcal = new TCanvas("cHcal", "HCal Efficiency", 1024, 640);
    cHcal->Print("hcal_delta.pdf["); // open PDF
    cHcal->Divide(2,2);
    int pad=1;
    TString t = "LH2";
    for (auto const &q : Q2s) {
        cHcal->cd(pad);

        std::pair<TString, int> key = std::make_pair(t,q);
        efficiencies[key]->GetTEfficiency()->Draw();

        pad++;
    }
    cHcal->Print("hcal_delta.pdf"); // write page to PDF

    pad=1;
    t = "C12";
    for (auto const &q : Q2s) {
        cHcal->cd(pad);

        std::pair<TString, int> key = std::make_pair(t,q);
        efficiencies[key]->GetTEfficiency()->Draw();

        pad++;
    }
    cHcal->Print("hcal_delta.pdf"); // write page to PDF

    cHcal->Print("hcal_delta.pdf]"); // close fPDF
}
