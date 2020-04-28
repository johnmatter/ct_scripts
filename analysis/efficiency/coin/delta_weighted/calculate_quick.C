#include <fstream>
#include <vector>
#include <map>

#include <TCanvas.h>
#include <TMath.h>
#include <TH1F.h>

#include <CTData.h>
#include <CTCuts.h>

// This macro calculates the efficiency of our PID detectors using the "quick"
// cuts There is no weighting by acceptance, but you could use the output .root
// of this script to calculate weighted efficiencies.
int main() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Efficiency infrastructure
    std::map<TString, Int_t> nDid;
    std::map<TString, Int_t> nShould;
    std::map<TString, Double_t> efficiencies;
    std::map<TString, Double_t> efficiencyErrors;

    // Events-per-delta histograms
    std::map<TString, TH1*> deltaHistograms;
    TString histoName, drawStr;

    // Which detectors
    std::vector<TString> detectors = {"pCer","hCal","hCer"};

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // Where are we saving the output?
    TString csvFilename = "quick/ct_quick_efficiency.csv";

    // Open file so we can save histograms
    TFile *f = new TFile("by_run/ct_quick_efficiency.root", "RECREATE");

    // ------------------------------------------------------------------------
    // Calculate efficiencies as a function of delta
    for (auto const &k : kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            for (auto const &d : detectors) {
                TString key = Form("%s_%d_%s", k.Data(), run, d.Data());

                std::cout << "Calculating:    " << key << std::endl;

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

                // Create TChain
                TString rootFilename = Form(data->GetRootfileTemplate(k),
                                            data->GetRootfileDirectory().Data(),
                                            run);
                TChain *T = new TChain("T");
                T->Add(rootFilename.Data());

                // ------------------
                // Should
                // Draw
                histoName = Form("%s_should", key.Data());
                drawStr = Form("%s>>%s(%d,%f,%f)",
                               scanBranch.Data(), histoName.Data(),
                               scanBins, scanLo, scanHi);
                T->Draw(drawStr.Data(), cutShould, "goff");
                // Add to map and write to .root output
                deltaHistograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
                deltaHistograms[histoName]->Write();
                nShould[key] = deltaHistograms[histoName]->Integral(1, scanBins);

                // ------------------
                // Did
                // Draw
                histoName = Form("%s_did", key.Data());
                drawStr = Form("%s>>%s(%d,%f,%f)",
                               scanBranch.Data(), histoName.Data(),
                               scanBins, scanLo, scanHi);
                T->Draw(drawStr.Data(), cutDid, "goff");
                // Add to map and write to .root output
                deltaHistograms[histoName] = (TH1F*) gDirectory->Get(histoName.Data());
                deltaHistograms[histoName]->Write();
                nDid[key] = deltaHistograms[histoName]->Integral(1, scanBins);

                // Calculate efficiency = k/N
                Double_t k = Double_t(nDid[key]);
                Double_t N = Double_t(nShould[key]);
                efficiencies[key]     = k/N;
                efficiencyErrors[key] = (1/N)*TMath::Sqrt(k*(1-(k/N))); // binomial errors
                // efficiencyErrors[key] = TMath::Sqrt(TMath::Power(k,2)*(k+N)/TMath::Power(N,3)); // Poisson errors
            }
        }
    }

    f->Close();

    // ------------------------------------------------------------------------
    // Save scalar efficiencies to CSV
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    ofs << "kinematics,run,detector,target,Q2,collimator,"
        << "hmsAngle,shmsAngle,hmsMomentum,shmsMomentum,"
        << "did,should,efficiency,efficiencyError" << std::endl;
    for (auto &k: kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            for (auto const &d : detectors) {
                TString key = Form("%s_%d_%s", k.Data(), run, d.Data());

                std::cout << "Writing to csv: " << key << std::endl;

                // Spectrometer info
                TString target        = data->GetTarget(k);
                Double_t Q2           = data->GetQ2(k);
                TString collimator    = data->GetCollimator(k);
                Double_t hmsAngle     = data->GetHMSAngle(k);
                Double_t shmsAngle    = data->GetSHMSAngle(k);
                Double_t hmsMomentum  = data->GetHMSMomentum(k);
                Double_t shmsMomentum = data->GetSHMSMomentum(k);

                // Efficiency info
                Int_t    did             = nDid[key];
                Int_t    should          = nShould[key];
                Double_t efficiency      = efficiencies[key];
                Double_t efficiencyError = efficiencyErrors[key];

                // Form print string
                TString printMe = Form("%s,%d,%s,%s,%f,%s,%f,%f,%f,%f,%d,%d,%f,%f",
                                        k.Data(), run, d.Data(), target.Data(), Q2, collimator.Data(),
                                        hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
                                        did, should, efficiency, efficiencyError);
                ofs << printMe << std::endl;
            }
        }
    }

    ofs.close();

    return 0;
}
