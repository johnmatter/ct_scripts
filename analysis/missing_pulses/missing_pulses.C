#include <CTData.h>
#include <CTCuts.h>

#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>
#include <TMath.h>
#include <TH2F.h>
#include <TH1F.h>

#include <vector>
#include <tuple>
#include <map>

#include <string.h>

// Peter Bosted email about missing pulses:
// "FADC amplitude missing due to another pulse overlapping in time with the
// pedestal time region. This is built into the firmware option that we are using.
// I correct by setting the amplitude to a typical default value. I do this for
// aerogel, cherenkovs, and hodoscopes, but not shower array or plane. This affects
// just a few percent of the events at 600 kHz."
//
//
// CT singles SHMS rates are below 100 kHz so I'm hoping this isn't a problem.
// This macro is an attempt to look at how many hodo pulses have 0 amplitude.
// Some of them are probably 0 because they should be (e.g. hodoscope paddle
// with no track passing through it, Cherenkov not firing because not electron.) 
// But this is a first look at distributions.

void missing_pulses() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Key is <kinematics, branch>
    std::map<std::tuple<TString, TString>, TH2F*> histograms;

    // For filling histograms
    Int_t N;

    // Where are we saving data?
    TString pdfFilename;
    TString rootSaveFilename = "missing_pulses_histos.root";
    TFile *fWrite;

    TChain* t;
    TString branch, histoName, drawStr, thisPMT;
    TH1F* histo;

    std::vector<TString> branches;

    // Explicitly list branches
    branches.push_back("P.hod.1x.posAdcPulseAmpRaw");
    branches.push_back("P.hod.2x.posAdcPulseAmpRaw");
    branches.push_back("P.hod.1y.posAdcPulseAmpRaw");
    branches.push_back("P.hod.2y.posAdcPulseAmpRaw");
    branches.push_back("P.hod.1x.negAdcPulseAmpRaw");
    branches.push_back("P.hod.2x.negAdcPulseAmpRaw");
    branches.push_back("P.hod.1y.negAdcPulseAmpRaw");
    branches.push_back("P.hod.2y.negAdcPulseAmpRaw");
    
    // How many paddles?
    std::map<TString, Double_t> nPaddles;
    nPaddles["P.hod.1x.posAdcPulseAmpRaw"] = 13;
    nPaddles["P.hod.2x.posAdcPulseAmpRaw"] = 14;
    nPaddles["P.hod.1y.posAdcPulseAmpRaw"] = 13;
    nPaddles["P.hod.2y.posAdcPulseAmpRaw"] = 21;
    nPaddles["P.hod.1x.negAdcPulseAmpRaw"] = 13;
    nPaddles["P.hod.2x.negAdcPulseAmpRaw"] = 14;
    nPaddles["P.hod.1y.negAdcPulseAmpRaw"] = 13;
    nPaddles["P.hod.2y.negAdcPulseAmpRaw"] = 21;
    
    // Bin range for histograms
    std::map<TString, Double_t> loBin;
    std::map<TString, Double_t> hiBin;
    Double_t binWidth = 1;
    Double_t lo, hi;
    Int_t nBins, binContent;

    for (auto const &branch: branches) {
        loBin[branch] = 0;
        hiBin[branch] = 4095;
    }
       
    // ------------------------------------------------------------------------
    // Generate and save histograms as we go
    fWrite = new TFile(rootSaveFilename, "RECREATE");

    // Loop over kinematics
    for (auto const &k : kinematics) {
        t = data->GetChain(k);

        for (auto const &branch: branches) {
            std::cout << Form("Processing %s, %s", k.Data(), branch.Data()) << std::endl;

            N = nPaddles[branch];

            hi = hiBin[branch];
            lo = loBin[branch];
            nBins = TMath::Floor((hi-lo)/binWidth);

            // Create histogram
            histoName = Form("%s_%s", k.Data(), branch.Data());
            histograms[std::make_tuple(k,branch)] = new TH2F(histoName.Data(), histoName.Data(), N, -0.5, N-0.5, nBins, lo, hi);

            // Fill histogram per PMT, then fill the TH2F
            for (int i=0; i<N; i++) {
                thisPMT = Form("%s[%d]", branch.Data(), i);

                histoName = Form("%s_%s_%d", k.Data(), branch.Data(), i);

                drawStr = Form("%s>>%s(%d,%f,%f)", thisPMT.Data(), histoName.Data(), nBins, lo, hi);

                t->Draw(drawStr.Data());

                histo = (TH1F*) gDirectory->Get(histoName.Data());

                // Write TH1F
                fWrite->WriteObject(histograms[std::make_tuple(k,branch)], histoName.Data());

                // Loop over bins
                for (int j=0; j<nBins; j++) {
                    binContent = histo->GetBinContent(j);
                    histograms[std::make_tuple(k,branch)]->SetBinContent(i,j,binContent);
                }
            }

            // Write TH2F
            histoName = Form("%s_%s", k.Data(), branch.Data());
            fWrite->WriteObject(histograms[std::make_tuple(k,branch)], histoName.Data());
        } // loop over branches
    } // loop over kinematics

    fWrite->Close();

    // ------------------------------------------------------------------------
    // Print to PDF
    gStyle->SetOptStat(0);
    TCanvas* c1 = new TCanvas("c1", "canvas", 640, 480);
    c1->SetLogz();

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
