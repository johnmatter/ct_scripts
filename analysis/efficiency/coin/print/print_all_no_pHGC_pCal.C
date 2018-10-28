#include <algorithm>
#include <utility>
#include <vector>
#include <tuple>
#include <map>

#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

// This macro calculates the efficiency of PID detectors relevant to
// out experiment. It then prints the values as a CSV.
void print_all_no_pHGC_pCal() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts_no_pHGC_pCal.json");

    std::map<TString, Efficiency0D*> efficiencies;


    std::vector<TString> detectors = {"pHGCer","hCal","hCer"};

    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14",
                                       "LH2_Q2_10_pion_collimator",
                                       "LH2_Q2_10_large_collimator"};

    // Calculate efficiencies
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            TString key = Form("%s_%s", k.Data(), d.Data());

            TCut cutShould = cuts->Get(Form("%sShould", d.Data()));
            TCut cutDid    = cuts->Get(Form("%sDid",    d.Data()));

            // Target-specific cuts
            if (k.Contains("LH2")) {
                cutShould = cutShould && cuts->Get("pLH2PMissCut")
                                      && cuts->Get("pLH2EMissCut");
            }

            efficiencies[key] = new Efficiency0D(key.Data());

            TChain* chain = data->GetChain(k.Data());
            efficiencies[key]->SetChain(chain);

            efficiencies[key]->SetShouldCut(cutShould);
            efficiencies[key]->SetDidCut(cutDid);

            efficiencies[key]->SetEvents(-1);
            efficiencies[key]->Init();
            efficiencies[key]->Calculate();

            efficiencies[key]->SetTitle(key.Data());
        }
    }

    // Print to screen
    std::cout << "kinematics,detector,target,Q2,"
              << "hmsAngle,shmsAngle,hmsMomentum,shmsMomentum,"
              << "efficiency,efficiencyErrorUp,efficiencyErrorLow" << std::endl;
    for (auto &k: kinematics) {
        for (auto const &d : detectors) {
            TString key = Form("%s_%s", k.Data(), d.Data());

            // Spectrometer info
            TString target        = data->GetTarget(k);
            Double_t Q2           = data->GetQ2(k);
            Double_t hmsAngle     = data->GetHMSAngle(k);
            Double_t shmsAngle    = data->GetSHMSAngle(k);
            Double_t hmsMomentum  = data->GetHMSMomentum(k);
            Double_t shmsMomentum = data->GetSHMSMomentum(k);

            // Efficiency info
            Double_t efficiency         = efficiencies[key]->GetEfficiency();
            Double_t efficiencyErrorUp  = efficiencies[key]->GetEfficiencyErrorUp();
            Double_t efficiencyErrorLow = efficiencies[key]->GetEfficiencyErrorLow();

            // Form print string
            TString printMe = Form("%s,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f",
                                    k.Data(), d.Data(), target.Data(), Q2,
                                    hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
                                    efficiency, efficiencyErrorUp, efficiencyErrorLow);
            std::cout << printMe << std::endl;
        }
    }
}
