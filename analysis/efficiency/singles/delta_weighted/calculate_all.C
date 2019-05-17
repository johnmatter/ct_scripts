#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>
#include <TH1.h>

#include <Efficiency1D.h>
#include <Efficiency0D.h>
#include <CTData.h>
#include <CTCuts.h>

// This macro calculates the efficiency of PID detectors relevant to
// our experiment. Efficiencies are weighted by each bin's uncertainty
//
// A summary of the scalar event-weighted efficiencies are saved as a CSV.
void calculate_all() {

    // Load data and cuts
    CTData *hmsData = new CTData("/home/jmatter/ct_scripts/ct_hms_singles_data.json");
    CTData *shmsData = new CTData("/home/jmatter/ct_scripts/ct_shms_singles_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Efficiency infrastructure
    std::map<TString, Efficiency1D*> efficiencyCalculators1D; // weighted by delta bins' uncertainty
    std::map<TString, Efficiency0D*> efficiencyCalculators0D; // unweighted
    std::map<TString, Double_t> efficiency;
    std::map<TString, Double_t> efficiencyError;

    // Which detectors
    std::vector<TString> detectors = {"pCer","hCal","hCer"};

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14",
                                       "C12_Q2_14_large_singles",
                                       "C12_Q2_14_pion_singles",
                                       "C12_Q2_10_thin",
                                       "C12_Q2_10_thick"};

    // Where are we saving the output?
    TString outDir = "/home/jmatter/ct_scripts/analysis/efficiency/singles/delta_weighted";
    TString csvFilename = Form("%s/ct_event_weighted_efficiency.csv", outDir.Data());
    TString pdfFilename = Form("%s/ct_event_weighted_efficiency.pdf", outDir.Data());
    TString csvPerBinFilename = Form("%s/ct_event_weighted_efficiency_per_bin.csv", outDir.Data());

    // ------------------------------------------------------------------------
    // Calculate efficiencies as a function of delta
    Double_t eff, err, weight;
    Int_t nDid;
    Int_t nShould;
    Double_t numerator;
    Double_t denominator;
    TH1 *hShould, *hDid;
    TString histoNameDid, histoNameShould;
    TString drawMeDid, drawMeShould;
    TString key;
    TCut cutShould, cutDid;

    // Binning for weighting
    Int_t hBins = 10;
    Int_t hLo   = -10;
    Int_t hHi   = +10;
    Int_t pBins = 10;
    Int_t pLo   = -10;
    Int_t pHi   = +10;

    // Loop over kinematics and detectors and calculate effieciency
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            key = Form("%s_%s", d.Data(), k.Data());
            std::cout << "Calculating " << key << std::endl;

            histoNameShould = Form("hShould_%s", key.Data());
            histoNameDid    = Form("hDid_%s",    key.Data());

            // HMS Cherenkov
            if (d=="hCer") {
                cutShould = cuts->Get("hBetaCut") && cuts->Get("hCalCut") && "H.cal.eprtrack>0.1";
                cutDid = cutShould && cuts->Get("hCerCut");
            }
            // HMS Calorimeter
            if (d=="hCal") {
                cutShould = cuts->Get("hBetaCut") && cuts->Get("hCerCut") && "H.cal.eprtrack>0.1";
                cutDid = cutShould && cuts->Get("hCalCut");
            }
            // SHMS NG Cherenkov
            if (d=="pCer") {
                cutShould = cuts->Get("pBetaCut") && cuts->Get("pHGCerCut") && "P.cal.eprtrack<0.1";
                cutDid = cutShould && cuts->Get("pNGCerCut");
            }

            // Draw; assume detector prefix h=HMS, p=SHMS
            switch(d[0]) {
                case 'h':
                    drawMeShould = Form("H.gtr.dp>>%s(%d,%d,%d)", histoNameShould.Data(), hBins, hLo, hHi);
                    drawMeDid    = Form("H.gtr.dp>>%s(%d,%d,%d)", histoNameDid.Data(),    hBins, hLo, hHi);
                    hmsData->GetChain(k)->Draw(drawMeShould.Data(), cutShould, "goff");
                    hmsData->GetChain(k)->Draw(drawMeDid.Data(),    cutDid, "goff");
                    break;
                case 'p':
                    drawMeShould = Form("P.gtr.dp>>%s(%d,%d,%d)", histoNameShould.Data(), pBins, pLo, pHi);
                    drawMeDid    = Form("P.gtr.dp>>%s(%d,%d,%d)", histoNameDid.Data(),    pBins, pLo, pHi);
                    shmsData->GetChain(k)->Draw(drawMeShould.Data(), cutShould, "goff");
                    shmsData->GetChain(k)->Draw(drawMeDid.Data(),    cutDid, "goff");
                    break;
            }

            // Get pointers to TH1s
            hShould = (TH1*)gDirectory->Get(histoNameShould.Data());
            hDid    = (TH1*)gDirectory->Get(histoNameDid.Data());

            // Create TEfficiency
            TEfficiency *peff = new TEfficiency(*hDid,*hShould);

            // Calculate weighted efficiency
            numerator=0;
            denominator=0;
            for (Int_t n=1; n<=hShould->GetNbinsX(); n++) {
                // Get max error for weighting
                err = peff->GetEfficiencyErrorLow(n);
                if (peff->GetEfficiencyErrorUp(n) > err) {
                    err = peff->GetEfficiencyErrorUp(n);
                }

                // Calculate weight
                weight = 1/(err*err);

                // Update numerator and denominator
                numerator += weight*peff->GetEfficiency(n);
                denominator += weight;
            }
            efficiency[key]      = numerator/denominator;
            efficiencyError[key] = 1/sqrt(denominator);
        }
    }

    // Loop over kinematics and detectors and print effieciency
    std::cout << std::endl;
    std::cout << "kinematics,detector,efficiency,efficiencyError" <<std::endl;
    for (auto const &k : kinematics) {
        for (auto const &d : detectors) {
            key = Form("%s_%s", d.Data(), k.Data());
            std::cout << k
               << "," << d
               << "," << efficiency[key]
               << "," << efficiencyError[key]
               << std::endl;
        }
    }
}
