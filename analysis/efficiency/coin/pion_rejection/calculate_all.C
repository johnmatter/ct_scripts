#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TEventList.h>

#include <CTData.h>
#include <CTCuts.h>

// How many pions "accidentally" fire our PID detectors in the HMS?
// I estimate pi:e ratios to be below 1 for LH2 data, but for
// C12 data we have a ratio of ~40 for Q^2=14.3. So this pion
// misfiring is a relevant question.
int calculate_all() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which detectors
    std::vector<TString> detectors = {"hCal", "hCer"};

    // Cleanup cuts
    TCut cleanupCut = cuts->Get("hDeltaCut") && cuts->Get("hBetaCut");

    // Pion selection for each detector
    // This is a selection of pions based on other detectors' information.
    // We want to know how many of these pions accidentally fire the detector.
    // These cuts are basically the negation of the electron PID cuts,
    // and maybe I should have just done the negation.
    // One risk there would be allowing in kBig values from hcana.
    std::map<TString, TCut> pionCuts;
    pionCuts["hCal"] = cleanupCut && "H.cer.npeSum==0";
    pionCuts["hCer"] = cleanupCut && "H.cal.etottracknorm<0.8";

    // Which kinematics
    std::vector<TString> kinematics = {"C12_Q2_8","C12_Q2_14"};
    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
    //                                    "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14",
    //                                    "LH2_Q2_10_pion_collimator",
    //                                    "LH2_Q2_10_large_collimator",
    //                                    "LH2_Q2_14_large_collimator",
    //                                    "LH2_Q2_14_pion_collimator",
    //                                    "C12_Q2_14_pion_collimator",
    //                                    "C12_Q2_14_large_collimator"};

    TEventList* elist;
    TString elistName, elistDraw;
    Int_t pionN, pionFiredN;
    Double_t pionRejection, error;

    // ------------------------------------------------------------------------
    // Calculate and print
    std::cout << "kinematics,Q2,target,detector,pionN,pionFiredN,pionRejection,error" << std::endl;
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            TCut thisPionCut   = pionCuts[d];
            TCut thisFiredCut  = thisPionCut && cuts->Get(Form("%sCut", d.Data()));

            TChain* chain = data->GetChain(k.Data());

            elistName.Form("%s_%s_pionElist", d.Data(), k.Data());
            elistDraw.Form(">>%s", elistName.Data());
            chain->Draw(elistDraw, thisPionCut);
            elist = (TEventList*) gDirectory->Get(elistName.Data());
            pionN = elist->GetN();

            elistName.Form("%s_%s_pionFiredElist", d.Data(), k.Data());
            elistDraw.Form(">>%s", elistName.Data());
            chain->Draw(elistDraw, thisFiredCut);
            elist = (TEventList*) gDirectory->Get(elistName.Data());
            pionFiredN = elist->GetN();

            pionRejection = 1.0 - Double_t(pionFiredN)/pionN;
            // error = pionRejection * sqrt(1/Double_t(pionFiredN) + 1/Double_t(pionN));
            error = pionRejection * sqrt(1/Double_t(pionN));

            std::cout << k                  << ","
                      << data->GetQ2(k)     << ","
                      << data->GetTarget(k) << ","
                      << d                  << ","
                      << pionN              << ","
                      << pionFiredN         << ","
                      << pionRejection      << ","
                      << error              << std::endl;
        }
    }

    return 0;
}
