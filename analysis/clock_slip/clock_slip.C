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

void clock_slip() {
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
    TString rootSaveFilename = "clock_slip_histos.root";
    TFile *fWrite;

    TChain* t;
    TString branch, histoName, drawStr, thisPMT;
    TH1F* histo;

    std::vector<TString> branches;

    // Explicitly list branches
    branches.push_back("H.cal.1pr.goodNegAdcTdcDiffTime");
    branches.push_back("H.cal.1pr.goodPosAdcTdcDiffTime");
    branches.push_back("H.cal.2ta.goodNegAdcTdcDiffTime");
    branches.push_back("H.cal.2ta.goodPosAdcTdcDiffTime");
    branches.push_back("H.cal.3ta.goodPosAdcTdcDiffTime");
    branches.push_back("H.cal.4ta.goodPosAdcTdcDiffTime");
    branches.push_back("H.cer.goodAdcTdcDiffTime");
    branches.push_back("H.hod.1x.GoodNegAdcTdcDiffTime");
    branches.push_back("H.hod.1x.GoodPosAdcTdcDiffTime");
    branches.push_back("H.hod.1y.GoodNegAdcTdcDiffTime");
    branches.push_back("H.hod.1y.GoodPosAdcTdcDiffTime");
    branches.push_back("H.hod.2x.GoodNegAdcTdcDiffTime");
    branches.push_back("H.hod.2x.GoodPosAdcTdcDiffTime");
    branches.push_back("H.hod.2y.GoodNegAdcTdcDiffTime");
    branches.push_back("H.hod.2y.GoodPosAdcTdcDiffTime");
    // branches.push_back("P.aero.goodNegAdcTdcDiffTime");
    // branches.push_back("P.aero.goodPosAdcTdcDiffTime");
    // branches.push_back("P.cal.fly.goodAdcTdcDiffTime");
    // branches.push_back("P.cal.pr.goodNegAdcTdcDiffTime");
    // branches.push_back("P.cal.pr.goodPosAdcTdcDiffTime");
    // branches.push_back("P.hgcer.goodAdcTdcDiffTime");
    branches.push_back("P.hod.1x.GoodNegAdcTdcDiffTime");
    branches.push_back("P.hod.1x.GoodPosAdcTdcDiffTime");
    branches.push_back("P.hod.1y.GoodNegAdcTdcDiffTime");
    branches.push_back("P.hod.1y.GoodPosAdcTdcDiffTime");
    branches.push_back("P.hod.2x.GoodNegAdcTdcDiffTime");
    branches.push_back("P.hod.2x.GoodPosAdcTdcDiffTime");
    branches.push_back("P.hod.2y.GoodNegAdcTdcDiffTime");
    branches.push_back("P.hod.2y.GoodPosAdcTdcDiffTime");
    branches.push_back("P.ngcer.goodAdcTdcDiffTime");

    // Bin range for histograms
    std::map<TString, Double_t> loBin;
    std::map<TString, Double_t> hiBin;
    Double_t binWidth = 0.1; // 100 ps
    Double_t lo, hi;
    Int_t nBins, binContent;


    loBin["H.cal.1pr.goodNegAdcTdcDiffTime"] = -150 ; hiBin["H.cal.1pr.goodNegAdcTdcDiffTime"] = -50 ;
    loBin["H.cal.1pr.goodPosAdcTdcDiffTime"] = -150 ; hiBin["H.cal.1pr.goodPosAdcTdcDiffTime"] = -50 ;
    loBin["H.cal.2ta.goodNegAdcTdcDiffTime"] = -150 ; hiBin["H.cal.2ta.goodNegAdcTdcDiffTime"] = -50 ;
    loBin["H.cal.2ta.goodPosAdcTdcDiffTime"] = -150 ; hiBin["H.cal.2ta.goodPosAdcTdcDiffTime"] = -50 ;
    loBin["H.cal.3ta.goodPosAdcTdcDiffTime"] = -150 ; hiBin["H.cal.3ta.goodPosAdcTdcDiffTime"] = -50 ;
    loBin["H.cal.4ta.goodPosAdcTdcDiffTime"] = -150 ; hiBin["H.cal.4ta.goodPosAdcTdcDiffTime"] = -50 ;
    loBin["H.cer.goodAdcTdcDiffTime"]        = 80   ; hiBin["H.cer.goodAdcTdcDiffTime"]        = 110 ;
    loBin["H.hod.1x.GoodNegAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.1x.GoodNegAdcTdcDiffTime"]  = -25 ;
    loBin["H.hod.1x.GoodPosAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.1x.GoodPosAdcTdcDiffTime"]  = -25 ;
    loBin["H.hod.1y.GoodNegAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.1y.GoodNegAdcTdcDiffTime"]  = -25 ;
    loBin["H.hod.1y.GoodPosAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.1y.GoodPosAdcTdcDiffTime"]  = -25 ;
    loBin["H.hod.2x.GoodNegAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.2x.GoodNegAdcTdcDiffTime"]  = -25 ;
    loBin["H.hod.2x.GoodPosAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.2x.GoodPosAdcTdcDiffTime"]  = -25 ;
    loBin["H.hod.2y.GoodNegAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.2y.GoodNegAdcTdcDiffTime"]  = -25 ;
    loBin["H.hod.2y.GoodPosAdcTdcDiffTime"]  = -75  ; hiBin["H.hod.2y.GoodPosAdcTdcDiffTime"]  = -25 ;
    loBin["P.aero.goodNegAdcTdcDiffTime"]    = -200 ; hiBin["P.aero.goodNegAdcTdcDiffTime"]    = 200 ;
    loBin["P.aero.goodPosAdcTdcDiffTime"]    = -200 ; hiBin["P.aero.goodPosAdcTdcDiffTime"]    = 200 ;
    loBin["P.cal.fly.goodAdcTdcDiffTime"]    = -200 ; hiBin["P.cal.fly.goodAdcTdcDiffTime"]    = 200 ;
    loBin["P.cal.pr.goodNegAdcTdcDiffTime"]  = -200 ; hiBin["P.cal.pr.goodNegAdcTdcDiffTime"]  = 200 ;
    loBin["P.cal.pr.goodPosAdcTdcDiffTime"]  = -200 ; hiBin["P.cal.pr.goodPosAdcTdcDiffTime"]  = 200 ;
    loBin["P.hgcer.goodAdcTdcDiffTime"]      = -200 ; hiBin["P.hgcer.goodAdcTdcDiffTime"]      = 200 ;
    loBin["P.hod.1x.GoodNegAdcTdcDiffTime"]  = -50  ; hiBin["P.hod.1x.GoodNegAdcTdcDiffTime"]  = 0   ;
    loBin["P.hod.1x.GoodPosAdcTdcDiffTime"]  = -50  ; hiBin["P.hod.1x.GoodPosAdcTdcDiffTime"]  = 0   ;
    loBin["P.hod.1y.GoodNegAdcTdcDiffTime"]  = -25  ; hiBin["P.hod.1y.GoodNegAdcTdcDiffTime"]  = 25  ;
    loBin["P.hod.1y.GoodPosAdcTdcDiffTime"]  = -25  ; hiBin["P.hod.1y.GoodPosAdcTdcDiffTime"]  = 25  ;
    loBin["P.hod.2x.GoodNegAdcTdcDiffTime"]  = -25  ; hiBin["P.hod.2x.GoodNegAdcTdcDiffTime"]  = 25  ;
    loBin["P.hod.2x.GoodPosAdcTdcDiffTime"]  = -25  ; hiBin["P.hod.2x.GoodPosAdcTdcDiffTime"]  = 25  ;
    loBin["P.hod.2y.GoodNegAdcTdcDiffTime"]  = -25  ; hiBin["P.hod.2y.GoodNegAdcTdcDiffTime"]  = 25  ;
    loBin["P.hod.2y.GoodPosAdcTdcDiffTime"]  = 0    ; hiBin["P.hod.2y.GoodPosAdcTdcDiffTime"]  = 50  ;
    loBin["P.ngcer.goodAdcTdcDiffTime"]      = -200 ; hiBin["P.ngcer.goodAdcTdcDiffTime"]      = 200 ;

    // ------------------------------------------------------------------------
    // Get list of ADC-TDC time diff branches if not explicitly specified above
    if (branches.empty()==true) {
        TObjArray *branchArray = (TObjArray*) data->GetChain("LH2_Q2_12")->GetListOfBranches()->Clone();

        t = data->GetChain(kinematics[0]);
        for (int i=0; i<branchArray->GetSize(); i++) {
            // Names are something like GoodPosAdcTdcDiffTime
            // Some have a lowercase a in ADC, so I search for dcTdc
            if ( strstr(branchArray->At(i)->GetName(), "dcTdc") ) {
                branch = branchArray->At(i++)->GetName();

                if (branch.Contains("Ndata")) {
                    branch.ReplaceAll("Ndata.","");
                }

                branches.push_back(branch);

                std::cout << branches.back() << std::endl;
            }
        }
    }

    // Remove any duplicates
    sort(branches.begin(), branches.end());
    branches.erase(unique(branches.begin(), branches.end()), branches.end());

    // ------------------------------------------------------------------------
    // Generate and save histograms as we go
    fWrite = new TFile(rootSaveFilename, "RECREATE");

    // Loop over kinematics
    for (auto const &k : kinematics) {
        t = data->GetChain(k);

        for (auto const &branch: branches) {
            std::cout << Form("Processing %s, %s", k.Data(), branch.Data()) << std::endl;

            // Set branches
            t->SetBranchAddress(Form("Ndata.%s", branch.Data()), &N);
            t->GetEntry(0);

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
