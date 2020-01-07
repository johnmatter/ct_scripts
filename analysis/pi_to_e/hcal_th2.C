#include <CTData.h>
#include <CTCuts.h>

#include <TH2F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TList.h>

#include <vector>
#include <map>

// Plot HMS calorimeter shower vs preshower energy
int main() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Any cut to apply?
    TCut cut;

    // Store histograms
    std::map<TString, TH2F*> histograms;

    // Where are we saving data?
    TString rootFilename = "hcal_th2.root";
    TFile *fWrite = new TFile(rootFilename, "RECREATE");

    TString drawMe, histoName;
    TH2F *histo;
    TList *l = new TList();

    // ------------------------------------------------------------------------
    // Loop over kinematics, draw, and write to disk
    for (auto const &k : kinematics) {
        std::cout << "Histogramming " << k << std::endl;
        if (data->GetTarget(k).Contains("C12")) {
            cut = cuts->Get("hCalShould") && cuts->Get("pC12EMissPMissCut");
        } else {
            cut = cuts->Get("hCalShould");
        }

        histoName = Form("%s_pshwr_vs_shwr_norm", k.Data());

        drawMe = Form("H.cal.eprtracknorm:(H.cal.etottracknorm-H.cal.eprtracknorm)>>%s(45,0,1.5,45,0,1.5)", histoName.Data());

        data->GetChain(k)->Draw(drawMe.Data(), cut, "goff");

        histograms[histoName] = (TH2F*) gDirectory->Get(histoName.Data());
        fWrite->WriteObject(histograms[histoName], histoName.Data());
    }
    fWrite->Close();

    return 0;
}
