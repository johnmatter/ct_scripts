#include <CTData.h>
#include <CTCuts.h>

#include <TEventList.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTree.h>
#include <TFile.h>

#include <fstream>
#include <vector>
#include <map>

// This calculates PID efficiency per run
int main() {
    // Load data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Which detectors?
    std::vector<TString> detectors = {"hCal", "hCer", "pCer"};

    // cuts
    Bool_t kinematicsCut;

    // Store how many events passed and how many should have
    std::map<std::tuple<TString, TString, Int_t>, Int_t> nShould;
    std::map<std::tuple<TString, TString, Int_t>, Int_t> nDid;
    // Initialize
    for (auto const &k : kinematics) {
        for (auto const &run : data->GetRuns(k)) {
            for (auto const &detector: detectors) {
                nShould[std::make_tuple(k, detector, run)]=0;
                nDid[std::make_tuple(k, detector, run)]=0;
            }
        }
    }

    // Where are we saving data?
    TString csvFilename = "pid_per_run.csv";

    // ------------------------------------------------------------------------
    // Loop over kinematics, count should and did
    TString rootFilename;
    TFile *file;
    for (auto const &k : kinematics) {
        std::cout << "Processing " << k << std::endl;

        for (auto const &run : data->GetRuns(k)) {

            // Open file
            rootFilename = Form(data->GetRootfileTemplate(k),
                                data->GetRootfileDirectory().Data(),
                                run);
            file = new TFile(rootFilename.Data(), "READ");
            TTreeReader reader("T", file);

            TTreeReaderValue<double> pBeta(reader, "P.gtr.beta");
            TTreeReaderValue<double> hBeta(reader, "H.gtr.beta");
            TTreeReaderValue<double> pDelta(reader, "P.gtr.dp");
            TTreeReaderValue<double> hDelta(reader, "H.gtr.dp");
            TTreeReaderValue<double> hEtottracknorm(reader, "H.cal.etottracknorm");
            TTreeReaderValue<double> hCerNpe(reader, "H.cer.npeSum");
            TTreeReaderValue<double> pCerNpe(reader, "P.ngcer.npeSum");
            TTreeReaderValue<double> hHodStatus(reader, "H.hod.goodstarttime");
            TTreeReaderValue<double> pHodStatus(reader, "P.hod.goodstarttime");
            TTreeReaderValue<double> emiss(reader, "P.kin.secondary.emiss_nuc");
            TTreeReaderValue<double> pmiss(reader,"P.kin.secondary.pmiss");

            // HMS Cherenkov
            reader.Restart();
            while(reader.Next()) {
                // Emiss/pmiss cut
                if (data->GetTarget(k).Contains("C12")) {
                    kinematicsCut = (*emiss<0.08) && abs(*pmiss)<0.3;
                } else {
                    kinematicsCut = true;
                }

                if (
                    kinematicsCut && 
                    *pHodStatus==1 && 
                    (*pBeta < 1.4) && (*pBeta > 0.6) && 
                    (*pDelta < 12) && (*pDelta > -10) &&
                    *hHodStatus==1 && 
                    (*hBeta < 1.2) && (*hBeta > 0.8) && 
                    (*hDelta < 10) && (*hDelta > -10) && 
                    (*hEtottracknorm < 1.15) && (*hEtottracknorm > 0.8)
                   ) {
                      nShould[std::make_tuple(k, "hCer", run)]++;
                      if (*hCerNpe>0) {
                          nDid[std::make_tuple(k, "hCer", run)]++;
                      }
                }
            }

            // HMS Calorimeter
            reader.Restart();
            while(reader.Next()) {
                // Emiss/pmiss cut
                if (data->GetTarget(k).Contains("C12")) {
                    kinematicsCut = (*emiss<0.08) && abs(*pmiss)<0.3;
                } else {
                    kinematicsCut = true;
                }

                if (
                    kinematicsCut && 
                    *pHodStatus==1 && 
                    (*pBeta < 1.4) && (*pBeta > 0.6) && 
                    (*pDelta < 12) && (*pDelta > -10) &&
                    *hHodStatus==1 && 
                    (*hBeta < 1.2) && (*hBeta > 0.8) && 
                    (*hDelta < 10) && (*hDelta > -10) && 
                    (*hCerNpe>0)
                   ) {
                      nShould[std::make_tuple(k, "hCal", run)]++;
                      if ((*hEtottracknorm < 1.15) && (*hEtottracknorm > 0.8)) {
                          nDid[std::make_tuple(k, "hCal", run)]++;
                      }
                }
            }

            // SHMS Cherenkov
            reader.Restart();
            while(reader.Next()) {
                // Emiss/pmiss cut
                if (data->GetTarget(k).Contains("C12")) {
                    kinematicsCut = (*emiss<0.08) && abs(*pmiss)<0.3;
                } else {
                    kinematicsCut = true;
                }

                if (
                    kinematicsCut && 
                    *pHodStatus==1 && 
                    (*pBeta < 1.4) && (*pBeta > 0.6) && 
                    (*pDelta < 12) && (*pDelta > -10) &&
                    *hHodStatus==1 && 
                    (*hBeta < 1.2) && (*hBeta > 0.8) && 
                    (*hDelta < 10) && (*hDelta > -10) && 
                    (*hCerNpe>0)
                   ) {
                      nShould[std::make_tuple(k, "pCer", run)]++;
                      if (*pCerNpe<0.1) {
                          nDid[std::make_tuple(k, "pCer", run)]++;
                      }
                }
            }

            delete file;
        }
    }

    // ------------------------------------------------------------------------
    // Output data

    // open csv
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // print header
    ofs << "kinematics,target,Q2,collimator,run,detector,should,did" << std::endl;
    for (auto const &k : kinematics) {
        std::cout << "Printing " << k << std::endl;
        for (auto const &run : data->GetRuns(k)) {
            for (auto const &detector: detectors) {
                ofs << k << ","
                    << data->GetTarget(k) << ","
                    << data->GetQ2(k) << ","
                    << data->GetCollimator(k) << ","
                    << run << "," << detector << ","
                    << nShould[std::make_tuple(k,detector,run)] << ","
                    << nDid[std::make_tuple(k,detector,run)]
                    << std::endl;
            }
        }
    }

    // close csv
    ofs.close();
}
