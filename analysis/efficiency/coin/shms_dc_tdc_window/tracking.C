#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TH1.h>
#include <TCut.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TEfficiency.h>

#include <CTCuts.h>

// This script studies the dependence of SHMS tracking efficiency on
// TDC window for track reconstruction, along the lines of the following
// note:
//      https://hallcweb.jlab.org/DocDB/0000/000023/001/hms_tracking.pdf
//
// I replayed the same run with 5 different windows; this will calculate
// efficiency for each and print the results for further analysis.
void tracking() {
    // ------------------------------------------------------------------------
    // Load cuts
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Window sizes
    std::vector<Int_t> windows = {1600, 3200, 4800, 6400, 8000};

    // Runs
    std::vector<Int_t> runs = {2054, 2279, 3188, 2453, 2283, 2427, 2191,
                               2065, 3181, 3198, 2441, 2317, 2358};

    // Where's our data?
    TString rootfileLocation = "/home/jmatter/ROOTfiles/pass1/pdc_time_window_study";
    TString rootfileFormat   = "%s/coin_replay_production_%d_-1_window_%d.root";

    // These maps take the <run,window> as a key
    std::map<std::pair<Int_t,Int_t>, TTree*> trees;
    std::map<std::pair<Int_t,Int_t>, TEfficiency*> efficiencyCalculators;
    std::map<std::pair<Int_t,Int_t>, Int_t> didN;
    std::map<std::pair<Int_t,Int_t>, Int_t> shouldN;
    std::map<std::pair<Int_t,Int_t>, Double_t> eff;

    // Set up our cuts
    TCut cutShould = cuts->Get("pScinShouldh");
    TCut cutDid    = cuts->Get("pScinDidh");

    // File to save to
    TString csvFilename = "tracking_shms_window_study.csv";

    // ------------------------------------------------------------------------
    // Open root files and store trees in map
    for (auto const &run : runs) {
        std::cout << "Load tree for run " << run << std::endl;
        for (auto const &window : windows) {
            auto key = std::make_pair(run, window);

            TString filename = Form(rootfileFormat.Data(), rootfileLocation.Data(), run, window);
            TFile *file = new TFile(filename.Data());
            TTree *tree = (TTree*) file->Get("T");
            trees[key] = tree;
        }
    }

    // ------------------------------------------------------------------------
    // Calculate for each run

    // TCanvas *cTEff = new TCanvas("cTEff", "cTEff", 600, 600);;

    for (auto const &run : runs) {
        std::cout << "Calculate efficiency for run " << run << std::endl;

        // PDF filename
        // TString pdfFilename = Form("run_%d_TEfficiency.pdf", run);

        // open PDF; "filename.pdf["
        // cTEff->Print((pdfFilename+"[").Data());

        // Calculate efficiency for each window
        for (auto const &window : windows) {
            auto key = std::make_pair(run, window);

            TString hShouldName = Form("h_run%d_should_window_%d", run, window);
            TString hDidName    = Form("h_run%d_did_window_%d", run, window);

            // Draw should
            TString drawShould = Form("P.gtr.dp>>%s(1,-10,15)", hShouldName.Data());
            shouldN[key] = trees[key]->Draw(drawShould.Data(), cutShould);

            // Draw did
            TString drawDid = Form("P.gtr.dp>>%s(1,-10,15)", hDidName.Data());
            didN[key] = trees[key]->Draw(drawDid.Data(), cutDid);

            eff[key] = Double_t(didN[key])/Double_t(shouldN[key]);

            TH1F* hShould = (TH1F*) gDirectory->Get(hShouldName.Data());
            TH1F* hDid    = (TH1F*) gDirectory->Get(hDidName.Data());

            efficiencyCalculators[key] = new TEfficiency(*hDid, *hShould);

            // Draw TEfficiency and write page to PDF
            // efficiencyCalculators[key]->GetCopyTotalHisto()->Draw();
            // cTEff->Print(pdfFilename.Data());
            // efficiencyCalculators[key]->GetCopyPassedHisto()->Draw();
            // cTEff->Print(pdfFilename.Data());
        }

        // close PDF; "filename.pdf]"
        // cTEff->Print((pdfFilename+"]").Data());
    }

    // ------------------------------------------------------------------------
    // Print efficiencies for other analyses
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    TString printme = Form("run,window,efficiency,efficiencyErrorUp,efficiencyErrorLow,did,should,eff");
    ofs << printme << std::endl;

    for (auto const &run : runs) {
        std::cout << "Print efficiency for run " << run << std::endl;
        for (auto const &window : windows) {
            auto key = std::make_pair(run, window);

            Double_t thisE    = efficiencyCalculators[key]->GetEfficiency(1);
            Double_t thisEUp  = efficiencyCalculators[key]->GetEfficiencyErrorUp(1);
            Double_t thisELow = efficiencyCalculators[key]->GetEfficiencyErrorLow(1);

            TString printme = Form("%d,%d,%f,%f,%f,%d,%d,%f", run, window, thisE, thisEUp, thisELow,
                    didN[key], shouldN[key], eff[key]);
            ofs << printme << std::endl;
        }
    }
    ofs.close();
}
