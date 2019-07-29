#include <utility>
#include <fstream>
#include <vector>
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
void tracking(TString spectrometer) {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::vector<TString>  spectrometers = {"SHMS","HMS"};

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();
    // std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
    //                                    "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // This map takes std::pair<spectrometer,kinematics> as a key
    std::map<TString, Efficiency0D*> efficiencyCalculators;

    // Set up our cuts
    TCut cutShould;
    TCut hCutShould = cuts->Get("hScinShoulde");
    TCut pCutShould = cuts->Get("pScinShouldh");
    TCut cutDid;
    TCut hCutDid    = cuts->Get("hScinDide");
    TCut pCutDid    = cuts->Get("pScinDidh");

    TString dummyBranch;

    // Spectrometer-specific info
    if (spectrometer=="SHMS") {
        cutShould = pCutShould;
        cutDid = pCutDid;
        dummyBranch = "P.hod.goodscinhit";
    }
    if (spectrometer=="HMS") {
        cutShould = hCutShould;
        cutDid = hCutDid;
        dummyBranch = "H.hod.goodscinhit";
    }

    // CSV to save
    TString csvFilename = Form("%s_tracking_per_run.csv", spectrometer.Data());

    TString rootFilename;
    TString key;

    // ------------------------------------------------------------------------
    // Calculate and print efficiencies

    // Open file
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // Print header
    TString printme = Form("kinematics,target,Q2,collimator,run,spectrometer,efficiency,efficiencyErrorUp,efficiencyErrorLow,did,should");
    ofs << printme << std::endl;

    // Loop over kinematics
    for (auto const &k : kinematics) {
        for (auto const &run : data->GetRuns(k)) {
std::cout << "Run: " << run << std::endl;
            // Create efficiency object
            key = Form("teff_%s_%s_run%d", spectrometer.Data(), k.Data(), run);
            efficiencyCalculators[key] = new Efficiency0D(key.Data());

            // Get tree for this run and add to calculator
            // Currently my efficiency class only accepts TChain*, not TTree*,
            // so we'll coerce it.
            rootFilename = Form(data->GetRootfileTemplate(k),
                                data->GetRootfileDirectory().Data(),
                                run);
            TChain *chain = new TChain("T");
            chain->Add(rootFilename.Data());
            efficiencyCalculators[key]->SetChain(chain);

            // Set cuts
            efficiencyCalculators[key]->SetShouldCut(cutShould);
            efficiencyCalculators[key]->SetDidCut(cutDid);

            // Dummy branch for histogram
            efficiencyCalculators[key]->SetScanBranch(dummyBranch);
            efficiencyCalculators[key]->SetScanRange(0,2);

            // Calculate
            efficiencyCalculators[key]->SetEvents(-1);
            efficiencyCalculators[key]->Init();
            efficiencyCalculators[key]->Calculate();

            //----------
            // Print
            Double_t Q2    = data->GetQ2(k);
            TString target = data->GetTarget(k);
            TString collimator = data->GetCollimator(k);

            Double_t thisE    = efficiencyCalculators[key]->GetEfficiency();
            Double_t thisEUp  = efficiencyCalculators[key]->GetEfficiencyErrorUp();
            Double_t thisELow = efficiencyCalculators[key]->GetEfficiencyErrorLow();

            Int_t did         = efficiencyCalculators[key]->GetTEfficiency()->GetCopyPassedHisto()->GetBinContent(1);
            Int_t should      = efficiencyCalculators[key]->GetTEfficiency()->GetCopyTotalHisto()->GetBinContent(1);

            TString printme = Form("%s,%s,%f,%s,%d,%s,%f,%f,%f,%d,%d",
                                    k.Data(), target.Data(), Q2,
                                    collimator.Data(), run, spectrometer.Data(),
                                    thisE, thisEUp, thisELow, did, should);
            ofs << printme << std::endl;

            // Cleanup
            delete efficiencyCalculators[key];
        }
    }
    ofs.close();
}
