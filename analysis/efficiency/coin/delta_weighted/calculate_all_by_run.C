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
    std::vector<TString> kinematics = data->GetNames();

    // Where are we saving the output?
    TString csvFilename = "by_run/ct_event_weighted_efficiency.csv";
    TString csvPerBinFilename = "by_run/ct_event_weighted_efficiency_per_bin.csv";

    // ------------------------------------------------------------------------
    // Calculate efficiencies as a function of delta
    for (auto const &k : kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            for (auto const &d : detectors) {
                TString key_1D = Form("%s_%d_%s_1D", k.Data(), run, d.Data());
                TString key_0D = Form("%s_%d_%s_0D", k.Data(), run, d.Data());

                TCut cutShould = cuts->Get(Form("%sShould", d.Data()));
                TCut cutDid    = cuts->Get(Form("%sDid",    d.Data()));

                // Target-specific cuts
                // if (k.Contains("LH2")) {
                //     cutShould = cutShould && cuts->Get("pLH2EMissPMissCut");
                //     cutDid    = cutDid    && cuts->Get("pLH2EMissPMissCut");
                // }
                if (k.Contains("C12")) {
                    cutShould = cutShould && cuts->Get("pC12EMissPMissCut");
                    cutDid    = cutDid    && cuts->Get("pC12EMissPMissCut");
                }


                efficiencyCalculators1D[key_1D] = new Efficiency1D(key_1D.Data());
                efficiencyCalculators0D[key_0D] = new Efficiency0D(key_0D.Data());

                // Set appropriate scan branch by spectrometer
                // The first character of detector name, d, should be either p or h, denoting SHMS or HMS
                TString scanBranch;
                Int_t scanBins;
                Double_t scanLo;
                Double_t scanHi;
                switch (d[0]) {
                    case 'h':
                        scanBranch = "H.gtr.dp";
                        scanBins = 20;
                        scanLo = -10;
                        scanHi = +10;
                        break;
                    case 'p':
                        scanBranch = "P.gtr.dp";
                        scanBins = 22;
                        scanLo = -10;
                        scanHi = +12;
                        break;
                }
                efficiencyCalculators0D[key_0D]->SetScanBranch(scanBranch);
                efficiencyCalculators0D[key_0D]->SetScanRange(scanLo, scanHi);
                efficiencyCalculators1D[key_1D]->SetScanBranch(scanBranch);
                efficiencyCalculators1D[key_1D]->SetScanRange(scanBins, scanLo, scanHi);

                // Set TChain
                TString rootFilename = Form(data->GetRootfileTemplate(k),
                                            data->GetRootfileDirectory().Data(),
                                            run);
                TChain *chain = new TChain("T");
                chain->Add(rootFilename.Data());
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
    }

    // ------------------------------------------------------------------------
    // Calculate scalar event-weighted efficiencies
    // Save efficiency values per bin in a csv for sanity checking

    std::ofstream ofsPerBin;
    ofsPerBin.open(csvPerBinFilename.Data());
    ofsPerBin << "kinematics,run,detector,target,Q2,collimator,deltaBinCenter,"
              << "efficiency,weight,efficiencyErrorMax,"
              << "efficiencyErrorUp,efficiencyErrorLo,nShould,nDid" << std::endl;

    for (auto const &k : kinematics) {
        TString target        = data->GetTarget(k);
        Double_t Q2           = data->GetQ2(k);
        TString collimator    = data->GetCollimator(k);

        for (auto const &run : data->GetRuns(k)) {

            for (auto const &d : detectors) {
                TString key_1D = Form("%s_%d_%s_1D", k.Data(), run, d.Data());
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
                    ofsPerBin << k << "," << run << "," << d << "," << target << "," << Q2 << "," << collimator
                              << "," << binCenter_i << "," << efficiency_i << "," << weight_i
                              << "," << efficiencyErrorMax_i
                              << "," << efficiencyErrorUp_i << "," << efficiencyErrorLo_i
                              << "," << nShould_i << "," << nDid_i << std::endl;
                }

                weightedEfficiency /= weightSum;
                weightedUncertainty = 1/sqrt(weightSum);

                efficiencies[key_1D]     = weightedEfficiency;
                efficiencyErrors[key_1D] = weightedUncertainty;
            }
        }
    }

    ofsPerBin.close();

    // ------------------------------------------------------------------------
    // Save scalar efficiencies to CSV
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    ofs << "kinematics,run,detector,target,Q2,collimator,"
        << "hmsAngle,shmsAngle,hmsMomentum,shmsMomentum,"
        << "efficiency,efficiencyError,"
        << "efficiencyUnweighted,efficiencyUnweightedErrorUp,efficiencyUnweightedErrorLo,"
        << "did,should"
        << std::endl;
    for (auto &k: kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            for (auto const &d : detectors) {
                TString key_1D = Form("%s_%d_%s_1D", k.Data(), run, d.Data());
                TString key_0D = Form("%s_%d_%s_0D", k.Data(), run, d.Data());

                // Spectrometer info
                TString target        = data->GetTarget(k);
                Double_t Q2           = data->GetQ2(k);
                TString collimator    = data->GetCollimator(k);
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

                Int_t did         = efficiencyCalculators0D[key_0D]->GetTEfficiency()->GetCopyPassedHisto()->GetBinContent(1);
                Int_t should      = efficiencyCalculators0D[key_0D]->GetTEfficiency()->GetCopyTotalHisto()->GetBinContent(1);

                // Form print string
                TString printMe = Form("%s,%d,%s,%s,%f,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d",
                                        k.Data(), run, d.Data(), target.Data(), Q2, collimator.Data(),
                                        hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
                                        efficiency, efficiencyError,
                                        efficiencyUnweighted,
                                        efficiencyUnweightedErrorUp,
                                        efficiencyUnweightedErrorLo,
                                        did,
                                        should);
                ofs << printMe << std::endl;
            }
        }
    }

    ofs.close();

    return 0;
}
