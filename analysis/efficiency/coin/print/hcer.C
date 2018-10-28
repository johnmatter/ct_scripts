#include <algorithm>
#include <utility>
#include <vector>
#include <tuple>
#include <map>

#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

// This macro calculates the efficiency of the HMS Cherenkov detector
// It then prints the values as a CSV
void hcer() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::map<TString, Efficiency0D*> efficiencies;

    TCut cutShould = cuts->Get("hCerShould");
    TCut cutDid = cuts->Get("hCerDid");

    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Calculate efficiencies
    for (auto const &k : kinematics) {
        efficiencies[k] = new Efficiency0D(k.Data());

        TChain* chain = data->GetChain(k.Data());
        efficiencies[k]->SetChain(chain);

        efficiencies[k]->SetShouldCut(cutShould);
        efficiencies[k]->SetDidCut(cutDid);

        efficiencies[k]->SetEvents(-1);
        efficiencies[k]->Init();
        efficiencies[k]->Calculate();

        efficiencies[k]->SetTitle(k.Data());
    }

    // Print to screen
    std::cout << "kinematics,target,Q2,hmsAngle,shmsAngle,hmsMomentum,shmsMomentum,efficiency,efficiencyErrorUp,efficiencyErrorLow" << std::endl;
    for (auto &k: kinematics) {
        // Spectrometer info
        TString target        = data->GetTarget(k);
        Double_t Q2           = data->GetQ2(k);
        Double_t hmsAngle     = data->GetHMSAngle(k);
        Double_t shmsAngle    = data->GetSHMSAngle(k);
        Double_t hmsMomentum  = data->GetHMSMomentum(k);
        Double_t shmsMomentum = data->GetSHMSMomentum(k);

        // Efficiency info
        Double_t efficiency         = efficiencies[k]->GetEfficiency();
        Double_t efficiencyErrorUp  = efficiencies[k]->GetEfficiencyErrorUp();
        Double_t efficiencyErrorLow = efficiencies[k]->GetEfficiencyErrorLow();

        // Form print string
        TString printMe = Form("%s,%s,%f,%f,%f,%f,%f,%f,%f,%f",
                                k.Data(), target.Data(), Q2,
                                hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
                                efficiency, efficiencyErrorUp, efficiencyErrorLow);
        std::cout << printMe << std::endl;
    }
}
