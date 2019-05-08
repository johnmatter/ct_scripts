#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>

#include <Efficiency1D.h>
#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

// This macro calculates the efficiency of PID detectors relevant to
// our experiment. Efficiencies are weighted by number of events per
// delta bin.
//
// Each detector's efficiency vs delta plot is saved in a PDF.
// A summary of the scalar event-weighted efficiencies are saved as a CSV.
int main() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Efficiency infrastructure
    std::map<TString, Efficiency1D*> efficiencyCalculators1D; // weighted by delta bins' uncertainty
    std::map<TString, Efficiency0D*> efficiencyCalculators0D; // unweighted
    std::map<TString, Double_t> efficiencies;
    std::map<TString, Double_t> efficiencyErrors;

    // Events-per-delta histograms for weighting
    std::map<TString, TH1*> deltaHistograms;

    // Which detectors
    std::vector<TString> detectors = {"pCer","hCal","hCer"};

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14",
                                       "LH2_Q2_10_pion_collimator",
                                       "LH2_Q2_10_large_collimator",
                                       "LH2_Q2_14_large_collimator",
                                       "LH2_Q2_14_pion_collimator",
                                       "C12_Q2_14_pion_collimator",
                                       "C12_Q2_14_large_collimator"};

    // Where are we saving the output?
    TString csvFilename = "ct_event_weighted_efficiency.csv";
    TString pdfFilename = "ct_event_weighted_efficiency.pdf";
    TString csvPerBinFilename = "ct_event_weighted_efficiency_per_bin.csv";

    // ------------------------------------------------------------------------
    // Calculate efficiencies as a function of delta
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            TString key_1D = Form("%s_%s_1D", k.Data(), d.Data());
            TString key_0D = Form("%s_%s_0D", k.Data(), d.Data());

            TCut cutShould = cuts->Get(Form("%sShould", d.Data()));
            TCut cutDid    = cuts->Get(Form("%sDid",    d.Data()));

            // Target-specific cuts
            if (k.Contains("LH2")) {
                cutShould = cutShould && cuts->Get("pLH2EMissPMissCut");
            }
            if (k.Contains("C12")) {
                cutShould = cutShould && cuts->Get("pC12EMissPMissCut");
            }


            efficiencyCalculators1D[key_1D] = new Efficiency1D(key_1D.Data());
            efficiencyCalculators0D[key_0D] = new Efficiency0D(key_0D.Data());

            // Set appropriate scan branch by spectrometer
            // The first character should be either p or h, denoting SHMS or HMS
            TString scanBranch;
            Int_t scanBins;
            Double_t scanLo;
            Double_t scanHi;
            switch (d[0]) {
                case 'h':
                    scanBranch = "H.gtr.dp";
                    scanBins = 16;
                    scanLo = -8;
                    scanHi = +8;
                    break;
                case 'p':
                    scanBranch = "P.gtr.dp";
                    scanBins = 25;
                    scanLo = -10;
                    scanHi = +15;
                    break;
            }
            efficiencyCalculators1D[key_1D]->SetScanBranch(scanBranch);
            efficiencyCalculators1D[key_1D]->SetScanRange(scanBins, scanLo, scanHi);

            // Set TChain
            TChain* chain = data->GetChain(k.Data());
            efficiencyCalculators1D[key_1D]->SetChain(chain);
            efficiencyCalculators0D[key_0D]->SetChain(chain);

            // Set cuts
            efficiencyCalculators1D[key_1D]->SetShouldCut(cutShould);
            efficiencyCalculators1D[key_1D]->SetDidCut(cutDid);
            efficiencyCalculators0D[key_0D]->SetShouldCut(cutShould);
            efficiencyCalculators0D[key_0D]->SetDidCut(cutDid);

            // Initialize and calculate
            efficiencyCalculators1D[key_1D]->SetEvents(-1);
            efficiencyCalculators1D[key_1D]->Init();
            efficiencyCalculators1D[key_1D]->Calculate();
            efficiencyCalculators0D[key_0D]->SetEvents(-1);
            efficiencyCalculators0D[key_0D]->Init();
            efficiencyCalculators0D[key_0D]->Calculate();

            // Formatting
            efficiencyCalculators1D[key_1D]->SetTitle(key_1D.Data());
            efficiencyCalculators0D[key_0D]->SetTitle(key_0D.Data());
        }
    }

    // ------------------------------------------------------------------------
    // Create delta TH1 for each (kinematics,detector) pair.
    // This is redundant, but it makes the coding easier
    // and costs very little in terms of time and space.
    // So I'm doing it this way.
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            TString key_1D = Form("%s_%s_1D", k.Data(), d.Data());

            TString printMe = Form("Plotting delta for %s", key_1D.Data());
            std::cout << printMe << std::endl;

            // Get data
            TChain* chain = data->GetChain(k.Data());

            // Which spectrometer?
            TString drawBranch;
            Int_t drawBins;
            Double_t drawLo;
            Double_t drawHi;
            switch (d[0]) {
                case 'h':
                    drawBranch = "H.gtr.dp";
                    drawBins = 16;
                    drawLo = -8;
                    drawHi = +8;
                case 'p':
                    drawBranch = "P.gtr.dp";
                    drawBins = 25;
                    drawLo = -10;
                    drawHi = +15;
            }

            // Which cuts?
            TCut drawCut;
            if (k.Contains("LH2")) {
                drawCut = cuts->Get("coinCutsLH2");
            }
            if (k.Contains("C12")) {
                drawCut = cuts->Get("coinCutsC12");
            }

            // Draw
            TString histoName = Form("%s_delta", key_1D.Data());
            TString drawMe    = Form("%s>>%s(%d,%f,%f)",
                                  drawBranch.Data(), histoName.Data(),
                                  drawBins, drawLo, drawHi);
            chain->Draw(drawMe.Data(), drawCut, "goff");

            // Add to map
            deltaHistograms[key_1D] = (TH1*) gDirectory->Get(histoName.Data());
        }
    }

    // ------------------------------------------------------------------------
    // Calculate scalar event-weighted efficiencies
    // Save efficiency values per bin in a csv for sanity checking

    std::ofstream ofsPerBin;
    ofsPerBin.open(csvPerBinFilename.Data());
    ofsPerBin << "kinematics,detector,target,Q2,deltaBinCenter,efficiency,weight,efficiencyErrorMax"
              << ",efficiencyErrorUp,efficiencyErrorLo,nShould,nDid,goodEvents" << std::endl;

    for (auto const &k : kinematics) {
        TString target        = data->GetTarget(k);
        Double_t Q2           = data->GetQ2(k);
        for (auto const &d : detectors) {
            TString key_1D = Form("%s_%s_1D", k.Data(), d.Data());
            TEfficiency* tEff = efficiencyCalculators1D[key_1D]->GetTEfficiency();

            TString printMe = Form("Calculating event-weighted efficiency for %s", key_1D.Data());
            std::cout << printMe << std::endl;

            Int_t nBins = tEff->GetCopyTotalHisto()->GetNbinsX();

            Double_t efficiency_i;
            Double_t efficiencyErrorMax_i;
            Double_t efficiencyErrorUp_i;
            Double_t efficiencyErrorLo_i;
            Int_t    nShould_i;
            Int_t    nDid_i;
            Int_t    events_i;
            Double_t binCenter_i;
            Double_t weight_i;
            Double_t weightSum = 0;
            Double_t weightSquaredSum = 0;
            Double_t weightedEfficiency = 0 ;
            Double_t weightedUncertainty;

            // These bins are indexed from 1 to N
            for (int bin=1; bin<=nBins; bin++) {

                // Pick larger of the efficiency asymmetric errors for our weight
                efficiencyErrorUp_i = tEff->GetEfficiencyErrorUp(bin);
                efficiencyErrorLo_i = tEff->GetEfficiencyErrorLow(bin);
                efficiencyErrorMax_i = std::max(efficiencyErrorUp_i, efficiencyErrorLo_i);

                // Get number of events in this bin
                nShould_i = tEff->GetCopyTotalHisto()->GetBinContent(bin);
                nDid_i    = tEff->GetCopyPassedHisto()->GetBinContent(bin);
                events_i  = deltaHistograms[key_1D]->GetBinContent(bin);

                // Get bin center
                binCenter_i = tEff->GetCopyTotalHisto()->GetXaxis()->GetBinCenter(bin);

                // Get weight
                // Skip if no events in bin
                if (nShould_i>0) {
                    weight_i = 1/pow(efficiencyErrorMax_i,2);
                } else {
                    weight_i = 0;
                }

                // Add to weight sums
                weightSum        += weight_i;
                weightSquaredSum += pow(weight_i,2);

                // Get efficiency and weight it
                efficiency_i        = tEff->GetEfficiency(bin);
                weightedEfficiency += weight_i * efficiency_i;

                // Print to csv
                ofsPerBin << k << "," << d << "," << target << "," << Q2
                          << "," << binCenter_i << "," << efficiency_i << "," << weight_i
                          << "," << efficiencyErrorMax_i
                          << "," << efficiencyErrorUp_i << "," << efficiencyErrorLo_i
                          << "," << nShould_i << "," << nDid_i << "," << events_i << std::endl;
            }

            weightedEfficiency /= weightSum;
            weightedUncertainty = 1/sqrt(weightSquaredSum);

            efficiencies[key_1D]     = weightedEfficiency;
            efficiencyErrors[key_1D] = weightedUncertainty;
        }
    }

    ofsPerBin.close();

    // ------------------------------------------------------------------------
    // Save scalar efficiencies to CSV
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    ofs << "kinematics,detector,target,Q2,"
              << "hmsAngle,shmsAngle,hmsMomentum,shmsMomentum,"
              << "efficiency,efficiencyError,"
              << "efficiencyUnweighted,efficiencyUnweightedErrorUp,efficiencyUnweightedErrorLo" << std::endl;
    for (auto &k: kinematics) {
        for (auto const &d : detectors) {
            TString key_1D = Form("%s_%s_1D", k.Data(), d.Data());
            TString key_0D = Form("%s_%s_0D", k.Data(), d.Data());

            // Spectrometer info
            TString target        = data->GetTarget(k);
            Double_t Q2           = data->GetQ2(k);
            Double_t hmsAngle     = data->GetHMSAngle(k);
            Double_t shmsAngle    = data->GetSHMSAngle(k);
            Double_t hmsMomentum  = data->GetHMSMomentum(k);
            Double_t shmsMomentum = data->GetSHMSMomentum(k);

            // Efficiency info
            Double_t efficiency      = efficiencies[key_1D];
            Double_t efficiencyError = efficiencyErrors[key_1D];
            Double_t efficiencyUnweighted = efficiencyCalculators0D[key_0D]->GetEfficiency();
            Double_t efficiencyUnweightedErrorUp = efficiencyCalculators0D[key_0D]->GetEfficiencyErrorUp();
            Double_t efficiencyUnweightedErrorLo = efficiencyCalculators0D[key_0D]->GetEfficiencyErrorLow();

            // Form print string
            TString printMe = Form("%s,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                                    k.Data(), d.Data(), target.Data(), Q2,
                                    hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
                                    efficiency, efficiencyError,
                                    efficiencyUnweighted,
                                    efficiencyUnweightedErrorUp,
                                    efficiencyUnweightedErrorLo);
            ofs << printMe << std::endl;
        }
    }

    ofs.close();

    // ------------------------------------------------------------------------
    // Save to PDF
    TCanvas* cEff = new TCanvas("cEff", "Efficiency", 1024, 640);

    cEff->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["

    // Print one page per detector per kinematics
    for (auto const &d : detectors) {
        for (auto const &k : kinematics) {
            TString key_1D = Form("%s_%s_1D", k.Data(), d.Data());

            TString printMe = Form("Plotting efficiency vs delta for %s", key_1D.Data());
            std::cout << printMe << std::endl;

            efficiencyCalculators1D[key_1D]->GetTEfficiency()->Draw();
            cEff->Print(pdfFilename.Data()); // write page to PDF
        }
    }

    cEff->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"

    return 0;
}
