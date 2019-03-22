#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>

#include <Efficiency2D.h>
#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

// This macro calculates the efficiency of PID detectors relevant to
// our experiment. Efficiencies are weighted by number of events per
// delta bin.
//
// Each detector's efficiency vs delta plot is saved in a PDF.
// A summary of the scalar event-weighted efficiencies are saved as a CSV.

// Forward declaration
void calculate_all_2D();

// Wrapper for compilation
int main() {
    calculate_all_2D();

    return 0;
}

// For interactive
void calculate_all_2D() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Efficiency infrastructure
    std::map<TString, Efficiency2D*> efficiencyCalculators2D; // weighted by uncertainty
    std::map<TString, Efficiency0D*> efficiencyCalculators0D; // unweighted
    std::map<TString, Double_t> efficiencies;
    std::map<TString, Double_t> efficiencyErrors;

    // Events-per-delta histograms for weighting
    std::map<TString, TH2*> positionHistograms;

    // Which detectors
    std::vector<TString> detectors = {"pNGCer","pHGCer","hCal","hCer"};

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
            TString key_2D = Form("%s_%s_2D", k.Data(), d.Data());
            TString key_0D = Form("%s_%s_0D", k.Data(), d.Data());

            TCut cutShould = cuts->Get(Form("%sShould", d.Data()));
            TCut cutDid    = cuts->Get(Form("%sDid",    d.Data()));

            // Target-specific cuts
            // if (k.Contains("LH2")) {
            //     cutShould = cutShould && cuts->Get("pLH2PMissCut")
            //                           && cuts->Get("pLH2EMissCut");
            // }

            efficiencyCalculators2D[key_2D] = new Efficiency2D(key_2D.Data());
            efficiencyCalculators0D[key_0D] = new Efficiency0D(key_0D.Data());

            // Set appropriate scan branch by detector
            TString  xBranch, yBranch;
            Int_t    xBins, yBins;
            Double_t xLo, yLo;
            Double_t xHi, yHi;
            if (d=="hCal") {
                xBranch = "H.cal.xtrack";
                xBins = 3;
                xLo = -60;
                xHi = +60;
                yBranch = "H.cal.ytrack";
                yBins = 3;
                yLo = -30;
                yHi = +30;
            }
            if (d=="hCer") {
                xBranch = "H.cer.xAtCer";
                xBins = 3;
                xLo = -60;
                xHi = +60;
                yBranch = "H.cer.yAtCer";
                yBins = 3;
                yLo = -30;
                yHi = +30;
            }
            if (d=="pNGCer") {
                xBranch = "P.ngcer.xAtCer";
                xBins = 3;
                xLo = -30;
                xHi = +30;
                yBranch = "P.ngcer.yAtCer";
                yBins = 3;
                yLo = -30;
                yHi = +30;
            }
            if (d=="pHGCer") {
                xBranch = "P.hgcer.xAtCer";
                xBins = 3;
                xLo = -30;
                xHi = +30;
                yBranch = "P.hgcer.yAtCer";
                yBins = 3;
                yLo = -30;
                yHi = +30;
            }
            efficiencyCalculators2D[key_2D]->SetScanBranches(xBranch, yBranch);
            efficiencyCalculators2D[key_2D]->SetScanRange(xBins, xLo, xHi, yBins, yLo, yHi);

            // Set TChain
            TChain* chain = data->GetChain(k.Data());
            efficiencyCalculators2D[key_2D]->SetChain(chain);
            efficiencyCalculators0D[key_0D]->SetChain(chain);

            // Set cuts
            efficiencyCalculators2D[key_2D]->SetShouldCut(cutShould);
            efficiencyCalculators2D[key_2D]->SetDidCut(cutDid);
            efficiencyCalculators0D[key_0D]->SetShouldCut(cutShould);
            efficiencyCalculators0D[key_0D]->SetDidCut(cutDid);

            // Initialize and calculate
            efficiencyCalculators2D[key_2D]->SetEvents(-1);
            efficiencyCalculators2D[key_2D]->Init();
            // efficiencyCalculators2D[key_2D]->Calculate();
            efficiencyCalculators0D[key_0D]->SetEvents(-1);
            efficiencyCalculators0D[key_0D]->Init();
            // efficiencyCalculators0D[key_0D]->Calculate();

            // Formatting
            efficiencyCalculators2D[key_2D]->SetTitle(key_2D.Data());
            efficiencyCalculators0D[key_0D]->SetTitle(key_0D.Data());
        }
    }

    // ------------------------------------------------------------------------
    // Create TH2s for each (kinematics,detector) pair.
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            TString key_2D = Form("%s_%s_2D", k.Data(), d.Data());

            TString printMe = Form("Plotting for %s", key_2D.Data());
            std::cout << printMe << std::endl;

            // Get data
            TChain* chain = data->GetChain(k.Data());

            // Which spectrometer?
            TString drawMe;
            TString histoName = Form("%s_position", key_2D.Data());
            if (d=="hCal") {
                drawMe = Form("H.cal.xtrack:H.cal.ytrack>>%s(100,-30,30,100,-60,60)",histoName.Data());
            }
            if (d=="hCer") {
                drawMe = Form("H.cer.xAtCer:H.cer.yAtCer>>%s(100,-30,30,100,-60,60)",histoName.Data());
            }
            if (d=="pNGCer") {
                drawMe = Form("P.ngcer.xAtCer:P.ngcer.yAtCer>>%s(100,-20,20,100,-30,30)",histoName.Data());
            }
            if (d=="pHGCer") {
                drawMe = Form("P.hgcer.xAtCer:P.hgcer.yAtCer>>%s(100,-20,20,100,-30,30)",histoName.Data());
            }

            // Which cuts?
            // TCut drawCut;
            // if (k.Contains("LH2")) {
            //     drawCut = cuts->Get("coinCutsLH2");
            // }
            // if (k.Contains("C12")) {
            //     drawCut = cuts->Get("coinCutsC12");
            // }
            TCut drawCut= cuts->Get(Form("%sShould", d.Data()));

            // Draw
            chain->Draw(drawMe.Data(), drawCut, "goff");

            // Add to map
            positionHistograms[key_2D] = (TH2*) gDirectory->Get(histoName.Data());

            // Set title
            positionHistograms[key_2D]->SetTitle(key_2D.Data());
            positionHistograms[key_2D]->GetXaxis()->SetTitle("Y");
            positionHistograms[key_2D]->GetYaxis()->SetTitle("X");
        }
    }

    // ------------------------------------------------------------------------
    // Calculate scalar event-weighted efficiencies
    // Save efficiency values per bin in a csv for sanity checking

    // std::ofstream ofsPerBin;
    // ofsPerBin.open(csvPerBinFilename.Data());
    // ofsPerBin << "kinematics,detector,target,Q2,xBinCenter,yBinCenter,efficiency,weight,"
    //           << "efficiencyErrorMax,efficiencyErrorUp,efficiencyErrorLo,"
    //           << "nShould,nDid" << std::endl;

    // for (auto const &k : kinematics) {
    //     TString target        = data->GetTarget(k);
    //     Double_t Q2           = data->GetQ2(k);
    //     for (auto const &d : detectors) {
    //         TString key_2D = Form("%s_%s_2D", k.Data(), d.Data());
    //         TEfficiency* tEff = efficiencyCalculators2D[key_2D]->GetTEfficiency();

    //         TString printMe = Form("Calculating event-weighted efficiency for %s", key_2D.Data());
    //         std::cout << printMe << std::endl;

    //         Int_t nXBins = tEff->GetCopyTotalHisto()->GetNbinsX();
    //         Int_t nYBins = tEff->GetCopyTotalHisto()->GetNbinsY();

    //         Double_t efficiency_i;
    //         Double_t efficiencyErrorMax_i;
    //         Double_t efficiencyErrorUp_i;
    //         Double_t efficiencyErrorLo_i;
    //         Int_t    nShould_i;
    //         Int_t    nDid_i;
    //         Double_t binxCenter_i;
    //         Double_t binyCenter_i;
    //         Double_t weight_i;
    //         Double_t weightSum = 0;
    //         Double_t weightSquaredSum = 0;
    //         Double_t weightedEfficiency = 0 ;
    //         Double_t weightedUncertainty;

    //         // These bins are indexed from 1 to N
    //         for (int binx=1; binx<=nXBins; binx++) {
    //             for (int biny=1; biny<=nYBins; biny++) {
    //                 // Get global bin
    //                 Int_t bing = tEff->GetGlobalBin(binx,biny);

    //                 // Pick larger of the efficiency asymmetric errors for our weight
    //                 efficiencyErrorUp_i = tEff->GetEfficiencyErrorUp(bing);
    //                 efficiencyErrorLo_i = tEff->GetEfficiencyErrorLow(bing);
    //                 efficiencyErrorMax_i = std::max(efficiencyErrorUp_i, efficiencyErrorLo_i);

    //                 // Get number of events in this bin
    //                 nShould_i = tEff->GetCopyTotalHisto()->GetBinContent(binx,biny);
    //                 nDid_i    = tEff->GetCopyPassedHisto()->GetBinContent(binx,biny);

    //                 // Get bin center
    //                 binxCenter_i = tEff->GetCopyTotalHisto()->GetXaxis()->GetBinCenter(binx);
    //                 binyCenter_i = tEff->GetCopyTotalHisto()->GetYaxis()->GetBinCenter(biny);

    //                 // Get weight
    //                 // Skip if no events in bin
    //                 if (nShould_i>0) {
    //                     weight_i = 1/pow(efficiencyErrorMax_i,2);
    //                 } else {
    //                     weight_i = 0;
    //                 }

    //                 // Add to weight sums
    //                 weightSum        += weight_i;
    //                 weightSquaredSum += pow(weight_i,2);

    //                 // Get efficiency and weight it
    //                 efficiency_i        = tEff->GetEfficiency(bing);
    //                 weightedEfficiency += weight_i * efficiency_i;

    //                 // Print to csv
    //                 ofsPerBin << k << "," << d << "," << target << "," << Q2
    //                           << "," << binxCenter_i << "," << binyCenter_i
    //                           << "," << efficiency_i << "," << weight_i
    //                           << "," << efficiencyErrorMax_i
    //                           << "," << efficiencyErrorUp_i << "," << efficiencyErrorLo_i
    //                           << "," << nShould_i << "," << nDid_i << std::endl;
    //             }
    //         }

    //         weightedEfficiency /= weightSum;
    //         weightedUncertainty = 1/sqrt(weightSquaredSum);

    //         efficiencies[key_2D]     = weightedEfficiency;
    //         efficiencyErrors[key_2D] = weightedUncertainty;
    //     }
    // }

    // ofsPerBin.close();

    // ------------------------------------------------------------------------
    // Save scalar efficiencies to CSV
    // std::ofstream ofs;
    // ofs.open(csvFilename.Data());

    // ofs << "kinematics,detector,target,Q2,"
    //           << "hmsAngle,shmsAngle,hmsMomentum,shmsMomentum,"
    //           << "efficiency,efficiencyError,"
    //           << "efficiencyUnweighted,efficiencyUnweightedErrorUp,efficiencyUnweightedErrorLo" << std::endl;
    // for (auto &k: kinematics) {
    //     for (auto const &d : detectors) {
    //         TString key_2D = Form("%s_%s_2D", k.Data(), d.Data());
    //         TString key_0D = Form("%s_%s_0D", k.Data(), d.Data());

    //         // Spectrometer info
    //         TString target        = data->GetTarget(k);
    //         Double_t Q2           = data->GetQ2(k);
    //         Double_t hmsAngle     = data->GetHMSAngle(k);
    //         Double_t shmsAngle    = data->GetSHMSAngle(k);
    //         Double_t hmsMomentum  = data->GetHMSMomentum(k);
    //         Double_t shmsMomentum = data->GetSHMSMomentum(k);

    //         // Efficiency info
    //         Double_t efficiency      = efficiencies[key_2D];
    //         Double_t efficiencyError = efficiencyErrors[key_2D];
    //         Double_t efficiencyUnweighted = efficiencyCalculators0D[key_0D]->GetEfficiency();
    //         Double_t efficiencyUnweightedErrorUp = efficiencyCalculators0D[key_0D]->GetEfficiencyErrorUp();
    //         Double_t efficiencyUnweightedErrorLo = efficiencyCalculators0D[key_0D]->GetEfficiencyErrorLow();

    //         // Form print string
    //         TString printMe = Form("%s,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
    //                                 k.Data(), d.Data(), target.Data(), Q2,
    //                                 hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
    //                                 efficiency, efficiencyError,
    //                                 efficiencyUnweighted,
    //                                 efficiencyUnweightedErrorUp,
    //                                 efficiencyUnweightedErrorLo);
    //         ofs << printMe << std::endl;
    //     }
    // }

    // ofs.close();

    // ------------------------------------------------------------------------
    // Save to PDF
    TCanvas* cEff = new TCanvas("cEff", "Efficiency", 1024, 640);

    cEff->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["

    // Print per detector per kinematics
    for (auto const &d : detectors) {
        for (auto const &k : kinematics) {
            TString key_2D = Form("%s_%s_2D", k.Data(), d.Data());

            TString printMe = Form("Saving %s to PDF", key_2D.Data());
            std::cout << printMe << std::endl;

            // Efficiency vs position
            // efficiencyCalculators2D[key_2D]->GetTEfficiency()->Draw();
            // cEff->Print(pdfFilename.Data()); // write page to PDF

            // Should fire N vs position
            positionHistograms[key_2D]->Draw("colz");
            cEff->Print(pdfFilename.Data()); // write page to PDF
        }
    }

    cEff->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"
}
