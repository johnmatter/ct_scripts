#include <utility>
#include <fstream>
#include <map>

#include <TH1F.h>
#include <TCut.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TPaveLabel.h>
#include <TPaveStats.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

#include <CTData.h>

// This calculates livetime.
// It also creates histograms of triggers' raw TDC times, both with and without window cuts.
// This allows us to check if our windows are set correctly
// Big thanks to Carlos Yero for the inspiration for this script.
void livetime() {

    //-------------------------------------------------------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // CSV to save
    TString csvFilename = "livetime.csv";

    // Needed for reading data
    TString rootFilename;
    TFile *f;
    TTree *T, *TS;

    // Key is run number
    std::map<Int_t, TH1F*> bcmHistos;
    TFitResultPtr fit;
    TH1F *histo;

    // Store scaler reads and decoded data
    std::map<TString, Double_t> thisRead;
    std::map<TString, Double_t> prevRead;
    std::map<TString, Double_t> value;

    // Key is <run, scaler>
    std::map<std::tuple<Int_t, TString>, Double_t> scalerTotal;

    // Accepted triggers
    // Key is <run, trigger>
    std::map<std::tuple<Int_t, TString>, TH1F*> histosPerRun;
    std::map<std::tuple<Int_t, TString>, Int_t> acceptedTrigs;

    // Livetimes
    // Key is <run, lt> where lt is one of  {"edtm", "cpu", "phys"}
    std::map<std::tuple<Int_t, TString>, Double_t> livetime;

    // Data we need from TSP
    TString bcmScaler = "P.BCM4A.scalerCurrent";
    TString bcmQ = "P.BCM4A.scalerCharge";
    TString timer = "P.1MHz.scalerTime";
    TString physScaler = "P.pTRIG6.scaler"; // SHMS 3/4 & HMS 3/4 coincidence
    TString edtmScaler = "P.EDTM.scaler";
    std::vector<TString> scalers = {
               physScaler,
               edtmScaler,
               bcmScaler,  // Merely for bookkeeping. The written csv value is meaningless.
               bcmQ,       // This one is actually useful for calculating yield.
               timer,      // You can use this to get average current though.
               "evNumber"  // Currently, this total will be wrong in the csv.
              };

    // Mean threshold for removing trips
    Double_t averageCurrent;

    // Data we need from T
    TString physBranch = "T.coin.pTRIG6_ROC2_tdcTime";
    TString edtmBranch = "T.coin.pEDTM_tdcTime";
    TString bcmBranch = "P.bcm.bcm4a.AvgCurrent";

    std::vector<TString> trigBranches = {physBranch, edtmBranch};

    // tdcTime ranges to integrate
    std::map<TString, std::vector<Double_t>> trigLowerLimits;
    std::map<TString, std::vector<Double_t>> trigUpperLimits;
    trigLowerLimits[physBranch].push_back(272); // pTRIG6 self-timing + EDTM
    trigUpperLimits[physBranch].push_back(276); // pTRIG6 self-timing + EDTM
    trigLowerLimits[physBranch].push_back(283); // EDTM
    trigUpperLimits[physBranch].push_back(287); // EDTM
    trigLowerLimits[physBranch].push_back(329); // EDTM
    trigUpperLimits[physBranch].push_back(333); // EDTM
    trigLowerLimits[edtmBranch].push_back(91);
    trigUpperLimits[edtmBranch].push_back(94);
    trigLowerLimits[edtmBranch].push_back(108);
    trigUpperLimits[edtmBranch].push_back(112);
    trigLowerLimits[edtmBranch].push_back(154);
    trigUpperLimits[edtmBranch].push_back(158);

    TString histoName, drawStr, cutStr;

    //-------------------------------------------------------------------------------------------------------------------------
    // Count triggers, calculate livetime
    for (auto const &k : kinematics) {
        // We didn't have EDTM for Q^2=8 GeV^2 data, so skip for now
        // There are ways to do this without the EDTM, but I don't have it
        // implemented in this script yet.
        if (data->GetQ2(k)==8) {
            std::cout << "Skip " << k << std::endl;
            continue;
        }
        std::cout << "Begin " << k << std::endl;

        for (auto const &run : data->GetRuns(k)) {
            std::cout << "Run: " << run << "--------------------" << std::endl;

            // ----------------------------------------------------------------
            // Bookkeeping

            // Initialize scalers, branches, counts, etc.
            for (auto const &scaler: scalers) {
                thisRead[scaler] = 0;
                prevRead[scaler] = 0;
                scalerTotal[std::make_tuple(run,scaler)] = 0;
            }

            // Get trees for this run from the rootfile
            rootFilename = Form(data->GetRootfileTemplate(k),
                                data->GetRootfileDirectory().Data(),
                                run);
            f  = new TFile(rootFilename.Data(), "READ");
            T  = (TTree*) f->Get("T");
            TS = (TTree*) f->Get("TSP");

            // Set scaler branch addresses
            for (auto const &scaler: scalers) {
                TS->SetBranchAddress(scaler, &(thisRead[scaler]));
            }

            // ----------------------------------------------------------------
            // Get average current
            histoName = Form("%s_run%d", bcmScaler.Data(), run);
            TS->Draw(Form("%s>>%s(200,0,100)", bcmScaler.Data(), histoName.Data()),
                     Form("%s>0", bcmScaler.Data()),
                     "goff");
            bcmHistos[run] = (TH1F*) gDirectory->Get(histoName.Data());
            fit = bcmHistos[run]->Fit("gaus", "S");
            averageCurrent = fit->Parameter(1);

            // ----------------------------------------------------------------
            // Loop over scalers and count triggers
            for (int i=0; i<TS->GetEntries(); i++) {
                if (i%1000==0) {
                    std::cout << "Scaler progress: " << setprecision(3) << 100*Double_t(i)/TS->GetEntries() << "%" << std::endl;
                }
                TS->GetEntry(i);

                // If current not 5% below average, increment scalerTotal by an amount
                // equal to the difference between this read and the previous one
                if ((averageCurrent - thisRead[bcmScaler])/averageCurrent < 0.5) {
                    for (auto const &scaler: scalers) {
                        Double_t increment = (thisRead[scaler] - prevRead[scaler]);
                        scalerTotal[std::make_tuple(run,scaler)] += (increment);
                    }
                }

                // Store this scaler read for the next read
                for (auto const &scaler: scalers) {
                    prevRead[scaler] = thisRead[scaler];
                }
            }
            std::cout << "Scaler progress: 100%" << std::endl;

            // ----------------------------------------------------------------
            // Count triggers

            // Draw pTRIG6
            histoName = Form("%s_%d", physBranch.Data(), run);;
            drawStr = Form("%s>>%s", physBranch.Data(), histoName.Data());
            cutStr  = Form("((%f-%s)/%f)<0.5", averageCurrent, bcmBranch.Data(), averageCurrent);
            T->Draw(drawStr.Data(), cutStr.Data(), "goff");
            histosPerRun[std::make_tuple(run, physBranch)] = (TH1F*) gDirectory->Get(histoName.Data());

            // Draw pEDTM
            histoName = Form("%s_%d", edtmBranch.Data(), run);;
            drawStr = Form("%s>>%s", edtmBranch.Data(), histoName.Data());
            cutStr  = Form("((%f-%s)/%f)<0.5", averageCurrent, bcmBranch.Data(), averageCurrent);
            T->Draw(drawStr.Data(), cutStr.Data(), "goff");
            histosPerRun[std::make_tuple(run, edtmBranch)] = (TH1F*) gDirectory->Get(histoName.Data());

            // Integrate pTRIG6
            histo = histosPerRun[std::make_tuple(run, physBranch)];
            acceptedTrigs[std::make_tuple(run, physBranch)] = 0;
            for (int i=0; i<trigLowerLimits[physBranch].size(); i++) {
                std::cout << Form("Integrate pTRIG6_tdcTime from %f to %f", trigLowerLimits[physBranch][i], trigUpperLimits[physBranch][i]) << std::endl;
                acceptedTrigs[std::make_tuple(run, physBranch)] += histo->Integral(trigLowerLimits[physBranch][i], trigUpperLimits[physBranch][i]);
            }

            // Integrate EDTM
            histo = histosPerRun[std::make_tuple(run, physBranch)];
            acceptedTrigs[std::make_tuple(run, edtmBranch)] = 0;
            for (int i=0; i<trigLowerLimits[edtmBranch].size(); i++) {
                std::cout << Form("Integrate pEDTM_tdcTime from %f to %f", trigLowerLimits[edtmBranch][i], trigUpperLimits[edtmBranch][i]) << std::endl;
                acceptedTrigs[std::make_tuple(run, edtmBranch)] += histo->Integral(trigLowerLimits[edtmBranch][i], trigUpperLimits[edtmBranch][i]);
            }

            // ----------------------------------------------------------------
            // Calculate live time
            // scalerTotal is a Double_t so we should be fine without recasting anything
            Int_t physTrigN, physScalerN;
            Int_t edtmTrigN, edtmScalerN;

            edtmTrigN = acceptedTrigs[std::make_tuple(run,edtmBranch)];
            edtmScalerN = scalerTotal[std::make_tuple(run,edtmScaler)];
            physTrigN = acceptedTrigs[std::make_tuple(run,physBranch)];
            physScalerN = scalerTotal[std::make_tuple(run,physScaler)];

            std::cout << Form("edtm lt = %d/%d\n", edtmTrigN, edtmScalerN);
            std::cout << Form("cpu  lt = %d/%d\n", physTrigN, physScalerN);
            std::cout << Form("phys lt = (%d-%d)/(%d-%d)\n", physTrigN, edtmTrigN, physScalerN, edtmScalerN);

            livetime[std::make_tuple(run,"edtm")] = Double_t(edtmTrigN) / Double_t(edtmScalerN);
            livetime[std::make_tuple(run,"cpu")]  = Double_t(physTrigN) / Double_t(physScalerN);
            livetime[std::make_tuple(run,"phys")] = Double_t(physTrigN-edtmTrigN) / Double_t(physScalerN-edtmScalerN);

            // ----------------------------------------------------------------
            // Cleanup
            // Deleting the TFile frees the memory taken up by any objects Get()-ed form it
            f->Close();
            delete f;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Print live time

    // Open file
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // Print header
    ofs << "kinematics"           << ","
        << "target"               << ","
        << "Q2"                   << ","
        << "collimator"           << ","
        << "run"                  << ","
        << "branch"               << ","
        << "description"          << ","
        << "count"
        << std::endl;

    // Loop over kinematics
    for (auto const &k : kinematics) {
        // Loop over runs
        for (auto const &run : data->GetRuns(k)) {

            // Print scaler counts
            for (auto const &scaler: scalers) {
                ofs << k                      << ","
                    << data->GetTarget(k)     << ","
                    << data->GetQ2(k)         << ","
                    << data->GetCollimator(k) << ","
                    << run                    << ","
                    << scaler                 << ","
                    << "scaler"               << ","
                    << scalerTotal[std::make_tuple(run,scaler)]
                    << std::endl;
            }

            // Print physics trigger counts
            ofs << k                      << ","
                << data->GetTarget(k)     << ","
                << data->GetQ2(k)         << ","
                << data->GetCollimator(k) << ","
                << run                    << ","
                << physBranch             << ","
                << "integral"             << ","
                << acceptedTrigs[std::make_tuple(run, physBranch)]
                << std::endl;

            // Print EDTM trigger counts
            ofs << k                      << ","
                << data->GetTarget(k)     << ","
                << data->GetQ2(k)         << ","
                << data->GetCollimator(k) << ","
                << run                    << ","
                << edtmBranch             << ","
                << "integral"             << ","
                << acceptedTrigs[std::make_tuple(run, edtmBranch)]
                << std::endl;

            // Print livetime
            for (TString lt: {"phys","edtm","cpu"}) {
                ofs << k                      << ","
                    << data->GetTarget(k)     << ","
                    << data->GetQ2(k)         << ","
                    << data->GetCollimator(k) << ","
                    << run                    << ","
                    << lt                     << ","
                    << "livetime"             << ","
                    << livetime[std::make_tuple(run,lt)]
                    << std::endl;
            }
        } // end loop over runs
    } // end loop over kinematics
    ofs.close();

    //-------------------------------------------------------------------------------------------------------------------------
    // Write histos to disk
    f = new TFile("histos.root","RECREATE");
    f->Write();

    //-------------------------------------------------------------------------------------------------------------------------
    // Print histos
    std::vector<TLine*> windowMarker;
    TPaveLabel *text;
    TString pdfFilename;
    TString thisWindowLabel;
    TCanvas* canvas = new TCanvas("canvas", "compare", 700, 500);

    canvas->SetLogy();

    // Create one file with one page per run
    pdfFilename = "/home/jmatter/ct_scripts/analysis/livetime/histos_per_run.pdf";
    canvas->Print((pdfFilename+"[").Data());
    for (auto const &branch: trigBranches) {
        for (auto const &k : kinematics) {
            for (auto const &run : data->GetRuns(k)) {
                histo = histosPerRun[std::make_tuple(run, branch)];
                histo->SetLineColor(kBlue+2);
                histo->Draw();

                // Label
                thisWindowLabel = Form("%s run %d", k.Data(), run);
                text = new TPaveLabel(0.125, 0.64, 0.3, 0.68, thisWindowLabel.Data(), "brNDC");
                text->Draw();
                canvas->Modified();
                canvas->Update();

                // Print page without integral limit lines
                canvas->Print(pdfFilename.Data());

                // Add reference lines for integral limits
                canvas->Update();
                for (int i=0; i<trigLowerLimits[branch].size(); i++) {
                    windowMarker[i] = new TLine(trigLowerLimits[branch][i], canvas->GetUymin(),
                                                trigUpperLimits[branch][i], canvas->GetUymax());
                    windowMarker[i]->SetLineColor(kRed);
                    windowMarker[i]->SetLineStyle(7);
                    windowMarker[i]->Draw();
                }

                gPad->Update();

                // Print page with integral limit lines
                canvas->Print(pdfFilename.Data());

                for (int i=0; i<trigLowerLimits[branch].size(); i++) {
                    delete windowMarker[i];
                }
            }
        }
    }
    canvas->Print((pdfFilename+"]").Data());
}
