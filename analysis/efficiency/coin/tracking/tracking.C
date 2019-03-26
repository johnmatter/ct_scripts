#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCut.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TGraphAsymmErrors.h>

#include <CTData.h>
#include <CTCuts.h>
#include <Efficiency0D.h>

// This calculates tracking efficiency for the SHMS and HMS
// as a function of Q^2 for C12 and LH2 targets
void tracking() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::vector<TString>  spectrometers = {"HMS","SHMS","SHMS_REPORT"};

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14",
                                       "LH2_Q2_10_pion_collimator",
                                       "LH2_Q2_10_large_collimator",
                                       "LH2_Q2_14_large_collimator",
                                       "LH2_Q2_14_pion_collimator",
                                       "C12_Q2_14_pion_collimator",
                                       "C12_Q2_14_large_collimator"};

    // This map takes std::pair<spectrometer,kinematics> as a key
    std::map<std::pair<TString, TString>, Efficiency0D*> efficiencyCalculators;

    // Set up our cuts
    TCut cutShould;
    TCut hCutShould = cuts->Get("hScinShoulde");
    TCut pCutShould = cuts->Get("pScinShouldh");
    TCut cutDid;
    TCut hCutDid    = cuts->Get("hScinDide");
    TCut pCutDid    = cuts->Get("pScinDidh");

    // Cuts so we can compare my cuts to the report template cuts
    TCut pReportCutShould = cuts->Get("pScinShould") && cuts->Get("pReportPIDCut");
    TCut pReportCutDid    = pReportCutShould && "P.dc.ntrack>0";

    // Save
    TString csvFilename = "tracking.csv";

    // ------------------------------------------------------------------------
    // Calculate efficiencies
    for (auto const &s : spectrometers) {
        // Get this spectrometer's cuts
        if (s=="SHMS") {
            cutShould = pCutShould;
            cutDid = pCutDid;
        }
        if (s=="SHMS_REPORT") {
            cutShould = pReportCutShould;
            cutDid = pReportCutDid;
        }
        if (s=="HMS") {
            cutShould = hCutShould;
            cutDid = hCutDid;
        }

        // Loop over kinematics
        for (auto const &k : kinematics)   {
            auto key = std::make_pair(s,k);

            // Create efficiency object
            TString efficiencyName = Form("teff_%s_%s", s.Data(), k.Data());
            efficiencyCalculators[key] = new Efficiency0D(efficiencyName.Data());

            // Set chain
            TChain* chain = data->GetChain(k);
            efficiencyCalculators[key]->SetChain(chain);

            // Set cuts
            efficiencyCalculators[key]->SetShouldCut(cutShould);
            efficiencyCalculators[key]->SetDidCut(cutDid);

            TString status = Form("-------\nSTATUS: %s, %s", k.Data(), s.Data());
            std::cout << status << std::endl;

            // Calculate
            efficiencyCalculators[key]->SetEvents(-1);
            efficiencyCalculators[key]->Init();
            efficiencyCalculators[key]->Calculate();
        }
    }

    // ------------------------------------------------------------------------
    // Print efficiencies for other analyses
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    TString printme = Form("kinematics,target,Q2,spectrometer,efficiency,efficiencyErrorUp,efficiencyErrorLow,did,should");
    ofs << printme << std::endl;

    for (auto const &s : spectrometers) {
        for (auto const &k : kinematics) {
            auto key = std::make_pair(s,k);

            Double_t Q2    = data->GetQ2(k);
            TString target = data->GetTarget(k);

            Double_t thisE    = efficiencyCalculators[key]->GetEfficiency();
            Double_t thisEUp  = efficiencyCalculators[key]->GetEfficiencyErrorUp();
            Double_t thisELow = efficiencyCalculators[key]->GetEfficiencyErrorLow();

            Int_t did         = efficiencyCalculators[key]->GetTEfficiency()->GetCopyPassedHisto()->GetBinContent(1);
            Int_t should      = efficiencyCalculators[key]->GetTEfficiency()->GetCopyTotalHisto()->GetBinContent(1);

            TString printme = Form("%s,%s,%f,%s,%f,%f,%f,%d,%d", k.Data(), target.Data(), Q2, s.Data(),
                    thisE, thisEUp, thisELow, did, should);
            ofs << printme << std::endl;
        }
    }
    ofs.close();
}
