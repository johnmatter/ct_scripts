#include <vector>
#include <map>

#include <TCanvas.h>
#include <TH1F.h>
#include <TCut.h>

#include <CTData.h>

class CutSelector {
    private:
        TCanvas *canvas;
        Int_t numKeys;
        std::vector<Double_t> keys;
        std::map<Double_t, TH1*> histograms;
        std::map<Double_t, TChain*> chains;

        TString branch;

        std::map<Double_t, Double_t> cutLow;
        std::map<Double_t, Double_t> cutHigh;
        std::map<Double_t, TCut> cuts;

        Int_t drawBins;
        Double_t drawLow;
        Double_t drawHigh;

    public:
        CutSelector(TString name, std::map<Double_t, TChain*>);
        void Select(TString b, Int_t n, Double_t lo, Double_t hi, std::map<Double_t, TCut> drawCuts);
        TCut Get(Double_t key) { return cuts[key]; }

};

//----------------------
// Constructor
CutSelector::CutSelector(TString name, std::map<Double_t, TChain*> c) {
    // Set chains
    chains = c;

    // Extract keys from input chains
    keys.clear();
    keys.reserve(chains.size());
    for(auto const& mapPair: chains)
            keys.push_back(mapPair.first);

    // Test that chains are non-null
    for (auto const key: keys) {
    }

    numKeys = keys.size();
    if (numKeys != 4) {
        std::cout << "input chains should have length 4!" << std::endl;
    }

    // Create canvas
    canvas = new TCanvas(name.Data(), name.Data(), 800, 600);
    canvas->Divide(2,2);
}

//----------------------
// Draw branch and ask user to make a selection on cut values for branch
// cuts is a map that expects the same keys as chains
void CutSelector::Select(TString b, Int_t n, Double_t lo, Double_t hi, std::map<Double_t, TCut> drawCuts) {
    // Set member values
    branch = b;
    drawBins = n;
    drawLow = lo;
    drawHigh = hi;

    // Draw
    Int_t pad = 1;
    TString drawString, histoName;
    for (auto const key: keys) {
        drawString = Form("%s>>%s(%d,%f,%f)", branch.Data(), histoName.Data(), drawBins, drawLow, drawHigh);
        chains[key]->Draw(drawString.Data(), drawCuts[key], "goff");
        histograms[key] = (TH1F*) gDirectory->Get(histoName.Data());
        canvas->cd(pad);
        histograms[key]->Draw();
        pad++;
    }
    canvas->Modified();
    canvas->Update();

    // Ask user to select cuts for each key
    string str;
    for (auto const key: keys) {

        cout << "Enter low cut for Q^2=" << key << ": ";
        getline(cin, str);
        stringstream(str) >> cutLow[key];

        cout << "Enter high cut for Q^2=" << key << ": ";
        getline(cin, str);
        stringstream(str) >> cutHigh[key];

        cuts[key] = Form("(%s>%f && %s<%f)", branch.Data(), cutLow[key], branch.Data(), cutHigh[key]);

    }
}

//----------------------
void calculate_absorption() {

    // Load data and cuts
    CTData *coinData = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTData *singlesData = new CTData("/home/jmatter/ct_scripts/ct_hms_singles_data.json");

    // keys are Q^2
    std::map<Double_t, TChain*> LH2CoinChains;
    std::map<Double_t, TChain*> LH2SingChains;
    std::map<Double_t, TChain*> AlSingChains;

    LH2CoinChains[8.0]  = coinData->GetChain("LH2_Q2_8");
    LH2CoinChains[9.5]  = coinData->GetChain("LH2_Q2_10_pion_collimator");
    LH2CoinChains[11.5] = coinData->GetChain("LH2_Q2_12");
    LH2CoinChains[14.3] = coinData->GetChain("LH2_Q2_14_large_collimator");

    LH2SingChains[8.0]  = singlesData->GetChain("LH2_Q2_8");
    LH2SingChains[9.5]  = singlesData->GetChain("LH2_Q2_10");
    LH2SingChains[11.5] = singlesData->GetChain("LH2_Q2_12");
    LH2SingChains[14.3] = singlesData->GetChain("LH2_Q2_14");

    AlSingChains[8.0]   = singlesData->GetChain("Al_Q2_8");
    AlSingChains[9.5]   = singlesData->GetChain("Al_Q2_10");
    AlSingChains[11.5]  = singlesData->GetChain("Al_Q2_12");
    AlSingChains[14.3]  = singlesData->GetChain("Al_Q2_14");

    // -----
    std::map<Double_t, TCut> emptyCuts;
    emptyCuts[8.0]  = "";
    emptyCuts[9.5]  = "";
    emptyCuts[11.5] = "";
    emptyCuts[14.3] = "";

    // --------------------------------------
    // First we cut on emiss

    CutSelector* eMissCutSelector = new CutSelector("emiss", LH2CoinChains);
    eMissCutSelector->Select("P.kin.secondary.emiss", 120, -0.02, 0.1, emptyCuts);

    std::map<Double_t, TCut> emissCuts;
    emissCuts[8.0]  = eMissCutSelector->Get(8.0);
    emissCuts[9.5]  = eMissCutSelector->Get(9.5);
    emissCuts[11.5] = eMissCutSelector->Get(11.5);
    emissCuts[14.3] = eMissCutSelector->Get(14.3);

    std::cout << "8.0: " << emissCuts[8.0] << std::endl;
    std::cout << "9.5: " << emissCuts[9.5] << std::endl;
    std::cout << "11.5: " << emissCuts[11.5] << std::endl;
    std::cout << "14.3: " << emissCuts[14.3] << std::endl;

}
