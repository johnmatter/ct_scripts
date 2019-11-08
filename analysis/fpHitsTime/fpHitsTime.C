#include <CTData.h>
#include <CTCuts.h>

#include <TCanvas.h>
#include <TH1F.h>

#include <vector>
#include <tuple>
#include <map>

#include <string.h>

void fpHitsTime() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Key is <kinematics, branch>
    std::map<std::tuple<TString, TString>, TH1F*> histograms;

    // Where are we saving data?
    TString pdfFilename;

    TChain* t;
    TString branch, histoName, drawStr;
    TH1F* histo;

    std::vector<TString> branches = {"P.hod.fpHitsTime", "H.hod.fpHitsTime"};

    Int_t nBins=400;
    Double_t lowerBin=-50;
    Double_t upperBin=+50;

    // ------------------------------------------------------------------------
    // Loop over kinematics
    for (auto const &k : kinematics) {
        t = data->GetChain(k);

        for (auto const &branch: branches) {
            std::cout << Form("Processing %s, %s", k.Data(), branch.Data()) << std::endl;

            // Create histogram
            histoName = Form("%s_%s", k.Data(), branch.Data());
            drawStr = Form("%s>>%s(%d,%f,%f)", branch.Data(), histoName.Data(),
                                               nBins, lowerBin, upperBin);

            t->Draw(drawStr.Data());
            histograms[std::make_tuple(k,branch)] = (TH1F*) gDirectory->Get(histoName.Data());
            histo = histograms[std::make_tuple(k,branch)];

        } // loop over branches
    } // loop over kinematics

    // ------------------------------------------------------------------------
    // Print to PDF

    TCanvas* c1 = new TCanvas("c1", "canvas", 640, 480);
    c1->SetLogz();

    // print header
    for (auto const &branch: branches) {
        pdfFilename = Form("%s.pdf", branch.Data());

        // open
        c1->Print((pdfFilename+"[").Data());

        for (auto const &k : kinematics) {
            histograms[std::make_tuple(k,branch)]->Draw("colz");
            c1->Print(pdfFilename.Data());
        }

        // close
        c1->Print((pdfFilename+"]").Data());
    }

}
