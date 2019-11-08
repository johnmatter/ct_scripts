#include <CTData.h>
#include <CTCuts.h>

#include <TCanvas.h>
#include <TH1.h>

#include <vector>

// Plot pmiss for all/pions/electrons
int main() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Which cuts?
    TCut pmissAllcut = cuts->Get("betaCut") && cuts->Get("deltaCut") && cuts->Get("hodoTimeCut");
    TCut pmissPionscut = cuts->Get("betaCut") && cuts->Get("deltaCut") && cuts->Get("hodoTimeCut") && cuts->Get("hPiPID");
    TCut pmissElectronscut = cuts->Get("betaCut") && cuts->Get("deltaCut") && cuts->Get("hodoTimeCut") && cuts->Get("hPIDCut");

    TH1F *pmissAll, *pmissPions, *pmissElectrons;

    // Where are we saving data?
    TString pdfFilename = "pmiss.pdf";

    // ------------------------------------------------------------------------
    // Loop over kinematics and plot
    TCanvas* c1 = new TCanvas("c1", "canvas", 640, 480);

    // open pdf
    c1->Print((pdfFilename+"[").Data());

    for (auto const &k : kinematics) {
        std::cout << "Processing " << k << std::endl;
        std::cout << "draw" << std::endl;
        data->GetChain(k)->Draw("TMath::Sign(1,P.kin.secondary.pmiss_z)*P.kin.secondary.pmiss>>pmissAll(200,-2,2)",pmissAllcut,"goff");
        data->GetChain(k)->Draw("TMath::Sign(1,P.kin.secondary.pmiss_z)*P.kin.secondary.pmiss>>pmissPions(200,-2,2)",pmissPionscut,"goff");
        data->GetChain(k)->Draw("TMath::Sign(1,P.kin.secondary.pmiss_z)*P.kin.secondary.pmiss>>pmissElectrons(200,-2,2)",pmissElectronscut,"goff");

        std::cout << "get" << std::endl;
        pmissAll = (TH1F*) gDirectory->Get("pmissAll");
        pmissPions = (TH1F*) gDirectory->Get("pmissPions");
        pmissElectrons = (TH1F*) gDirectory->Get("pmissElectrons");

        std::cout << "format" << std::endl;
        pmissPions->SetLineColor(2);pmissPions->SetFillColor(2);pmissPions->SetFillStyle(3345);
        pmissElectrons->SetLineColor(8);pmissElectrons->SetFillColor(8);pmissElectrons->SetFillStyle(3354);

        std::cout << "print" << std::endl;
        pmissAll->Draw();
        pmissPions->Draw("SAME");
        pmissElectrons->Draw("SAME");

        c1->Print(pdfFilename.Data());
    }

    // close pdf
    c1->Print((pdfFilename+"]").Data());
}
