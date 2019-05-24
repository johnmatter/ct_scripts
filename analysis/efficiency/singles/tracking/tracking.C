#include <utility>
#include <vector>
#include <tuple>
#include <map>

#include <TCut.h>

#include <CTData.h>
#include <CTCuts.h>
#include <Efficiency0D.h>

// Calculate tracking efficiency for all singles kinematics
void tracking() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *hms_data  = new CTData("/home/jmatter/ct_scripts/ct_hms_singles_data.json");
    CTData *shms_data = new CTData("/home/jmatter/ct_scripts/ct_shms_singles_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which Spectrometers
    std::vector<TString>  spectrometers = {"HMS","SHMS"};

    // Which kinematics
    // Assume SHMS and HMS singles have same kinematics runlists
    std::vector<TString> kinematics = hms_data->GetNames();

    // This map takes std::pair<spectrometer,kinematics> as a key
    std::map<std::pair<TString, TString>, Efficiency0D*> efficiencyCalculators;

    // Set up our cuts
    TCut cutShould;
    TCut hCutShould = cuts->Get("hScinShoulde");
    TCut pCutShould = cuts->Get("pScinShouldh");
    TCut cutDid;
    TCut hCutDid    = cuts->Get("hScinDide");
    TCut pCutDid    = cuts->Get("pScinDidh");

    // File to save to
    TString csvDir = "/home/jmatter/ct_scripts/analysis/efficiency/singles/tracking";
    TString csvName = "tracking.csv";
    TString csvFilename = Form("%s/%s", csvDir.Data(), csvName.Data());

    // Used in loop over kinematics for data access or text output
    TChain* chain;
    Double_t Q2;
    TString target;

    // ------------------------------------------------------------------------
    // Calculate and print efficiencies

    // Open file
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // Print header
    TString printme = Form("kinematics,target,Q2,spectrometer,efficiency,efficiencyErrorUp,efficiencyErrorLow,did,should");
    ofs << printme << std::endl;

    // Loop over kinematics
    for (auto const &k : kinematics)   {
        for (auto const &s : spectrometers) {
            // Get this spectrometer's cuts and chain
            if (s=="SHMS") {
                cutShould = pCutShould;
                cutDid    = pCutDid;

                chain  = shms_data->GetChain(k);
                Q2     = shms_data->GetQ2(k);
                target = shms_data->GetTarget(k);
            }
            if (s=="HMS") {
                cutShould = hCutShould;
                cutDid    = hCutDid;

                chain  = hms_data->GetChain(k);
                Q2     = hms_data->GetQ2(k);
                target = hms_data->GetTarget(k);
            }

            auto key = std::make_pair(s,k);

            //----------
            // Calculate

            // Create efficiency object
            TString efficiencyName = Form("teff_%s_%s", s.Data(), k.Data());
            efficiencyCalculators[key] = new Efficiency0D(efficiencyName.Data());

            // Set chain
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

            //----------
            // Print

            Double_t thisE    = efficiencyCalculators[key]->GetEfficiency();
            Double_t thisEUp  = efficiencyCalculators[key]->GetEfficiencyErrorUp();
            Double_t thisELow = efficiencyCalculators[key]->GetEfficiencyErrorLow();

            Int_t did         = efficiencyCalculators[key]->GetTEfficiency()->GetCopyPassedHisto()->GetBinContent(1);
            Int_t should      = efficiencyCalculators[key]->GetTEfficiency()->GetCopyTotalHisto()->GetBinContent(1);

            TString printme = Form("%s,%s,%f,%s,%f,%f,%f,%d,%d", k.Data(), target.Data(), Q2, s.Data(),
                    thisE, thisEUp, thisELow, did, should);
            ofs << printme << std::endl;

            // Delete TEfficiency
            delete efficiencyCalculators[key];
        }
    }
    ofs.close();
}

