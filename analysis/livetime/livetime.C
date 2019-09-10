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

std::map<Int_t, Double_t> getBeamCutValues();

void livetime() {

    //-------------------------------------------------------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_edtmdecode.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // CSV to save
    TString csvFilename = "livetime.csv";

    // root file to save histograms to
    TString rootSaveFilename = "histos.root";

    // Needed for reading data
    TString rootFilename;
    TFile *fRead, *fWrite;
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

    std::map<Int_t, Double_t> beamCut = getBeamCutValues();

    TString histoName, drawStr, cutStr;

    //-------------------------------------------------------------------------------------------------------------------------
    // Count triggers, calculate livetime

    fWrite = new TFile(rootSaveFilename, "RECREATE");

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
            fRead  = new TFile(rootFilename.Data(), "READ");
            T  = (TTree*) fRead->Get("T");
            TS = (TTree*) fRead->Get("TSP");

            // Set scaler branch addresses
            for (auto const &scaler: scalers) {
                TS->SetBranchAddress(scaler, &(thisRead[scaler]));
            }

            // ----------------------------------------------------------------
            // Get current trip threshold
            histoName = Form("run%d_%s", run, bcmScaler.Data());
            TS->Draw(Form("%s>>%s(200,0,100)", bcmScaler.Data(), histoName.Data()),
                     Form("%s>0", bcmScaler.Data()),
                     "goff");
            bcmHistos[run] = (TH1F*) gDirectory->Get(histoName.Data());
            // fit = bcmHistos[run]->Fit("gaus", "S");
            // averageCurrent = fit->Parameter(1);

            averageCurrent = beamCut[run];
            std::cout << Form("bcmCut=%f", averageCurrent) << std::endl;

            fWrite->WriteObject(bcmHistos[run], histoName.Data());

            // ----------------------------------------------------------------
            // Loop over scalers and count triggers
            for (int i=0; i<TS->GetEntries(); i++) {
                if (i%1000==0) {
                    std::cout << "Scaler progress: " << setprecision(3) << 100*Double_t(i)/TS->GetEntries() << "%" << std::endl;
                }
                TS->GetEntry(i);

                // If current not 5% below average, increment scalerTotal by an amount
                // equal to the difference between this read and the previous one
                if (thisRead[bcmScaler] >= (0.95 * averageCurrent)) {
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
            // Draw histograms, count triggers, write histos to disk

            cutStr  = Form("(%s)>=(0.95*%f)", bcmBranch.Data(), averageCurrent);

            // Draw pTRIG6
            histoName = Form("run%d_%s", run, physBranch.Data());;
            drawStr = Form("%s>>%s", physBranch.Data(), histoName.Data());
            T->Draw(drawStr.Data(), cutStr.Data(), "goff");
            histosPerRun[std::make_tuple(run, physBranch)] = (TH1F*) gDirectory->Get(histoName.Data());
            fWrite->WriteObject(histosPerRun[std::make_tuple(run, physBranch)], histoName.Data());

            // Draw pEDTM
            histoName = Form("run%d_%s", run, edtmBranch.Data());;
            drawStr = Form("%s>>%s", edtmBranch.Data(), histoName.Data());
            T->Draw(drawStr.Data(), cutStr.Data(), "goff");
            histosPerRun[std::make_tuple(run, edtmBranch)] = (TH1F*) gDirectory->Get(histoName.Data());
            fWrite->WriteObject(histosPerRun[std::make_tuple(run, edtmBranch)], histoName.Data());

            Int_t count, lowerBin, upperBin;

            // Integrate pTRIG6
            histo = histosPerRun[std::make_tuple(run, physBranch)];
            acceptedTrigs[std::make_tuple(run, physBranch)] = 0;
            for (int i=0; i<trigLowerLimits[physBranch].size(); i++) {
                lowerBin = histo->FindBin(trigLowerLimits[physBranch][i]);
                upperBin = histo->FindBin(trigupperLimits[physBranch][i]);
                count = histo->Integral(lowerBin, upperBin);
                std::cout << Form("Integrate pTRIG6_tdcTime from %f to %f: %d", trigLowerLimits[physBranch][i], trigUpperLimits[physBranch][i], count) << std::endl;
                acceptedTrigs[std::make_tuple(run, physBranch)] += count;
            }

            // Integrate EDTM
            histo = histosPerRun[std::make_tuple(run, edtmBranch)];
            acceptedTrigs[std::make_tuple(run, edtmBranch)] = 0;
            for (int i=0; i<trigLowerLimits[edtmBranch].size(); i++) {
                lowerBin = histo->FindBin(trigLowerLimits[edtmBranch][i]);
                upperBin = histo->FindBin(trigupperLimits[edtmBranch][i]);
                count = histo->Integral(lowerBin, upperBin);
                std::cout << Form("Integrate pEDTM_tdcTime from %f to %f: %d", trigLowerLimits[edtmBranch][i], trigUpperLimits[edtmBranch][i], count) << std::endl;
                acceptedTrigs[std::make_tuple(run, edtmBranch)] += count;
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
            fRead->Close();
            delete fRead;
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Print live time

    std::cout << "Print livetime and scalers to csv" << std::endl;

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

        // We didn't have EDTM for Q^2=8 GeV^2 data, so skip for now
        // There are ways to do this without the EDTM, but I don't have it
        // implemented in this script yet.
        if (data->GetQ2(k)==8) {
            std::cout << "Skip " << k << std::endl;
            continue;
        }

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
    fWrite->Close();

    //-------------------------------------------------------------------------------------------------------------------------
    // Print histos
    fRead = new TFile(rootSaveFilename.Data(),"READ");

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

            // We didn't have EDTM for Q^2=8 GeV^2 data, so skip for now
            // There are ways to do this without the EDTM, but I don't have it
            // implemented in this script yet.
            if (data->GetQ2(k)==8) {
                std::cout << "Skip " << k << std::endl;
                continue;
            }

            for (auto const &run : data->GetRuns(k)) {
                histoName = Form("run%d_%s", run, branch.Data());
                histo = (TH1F*) gDirectory->Get(histoName.Data());
                if (histo==nullptr) {
                    std::cout << Form("nullputr histo for run %d, %s", run, branch.Data())<< std::endl;
                    continue;
                }
                std::cout << Form("Printing histo for run %d, %s", run, branch.Data())<< std::endl;

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

std::map<Int_t, Double_t> getBeamCutValues() {
    std::map<Int_t, Double_t> beamCut;

    beamCut[2278] = 14;
    beamCut[2279] = 14;
    beamCut[2280] = 15;
    beamCut[2281] = 15;
    beamCut[2283] = 15;
    beamCut[2284] = 45;
    beamCut[2285] = 45;
    beamCut[2286] = 45;
    beamCut[2290] = 44;
    beamCut[2291] = 44;
    beamCut[2292] = 44;
    beamCut[2293] = 39;
    beamCut[2294] = 39;
    beamCut[2295] = 39;
    beamCut[2296] = 39;
    beamCut[2297] = 39;
    beamCut[2298] = 40;
    beamCut[2299] = 40;
    beamCut[2300] = 25;
    beamCut[2301] = 40;
    beamCut[2303] = 40;
    beamCut[2304] = 40;
    beamCut[2305] = 40;
    beamCut[2306] = 40;
    beamCut[2308] = 40;
    beamCut[2309] = 40;
    beamCut[2310] = 15;
    beamCut[2311] = 15;
    beamCut[2312] = 40;
    beamCut[2313] = 40;
    beamCut[2314] = 40;
    beamCut[2315] = 40;
    beamCut[2316] = 40;
    beamCut[2317] = 40;
    beamCut[2318] = 40;
    beamCut[2319] = 40;
    beamCut[2320] = 40;
    beamCut[2321] = 40;
    beamCut[2322] = 40;
    beamCut[2323] = 40;
    beamCut[2324] = 40;
    beamCut[2325] = 40;
    beamCut[2343] = 40;
    beamCut[2344] = 35;
    beamCut[2345] = 40;
    beamCut[2346] = 40;
    beamCut[2347] = 40;
    beamCut[2348] = 45;
    beamCut[2349] = 45;
    beamCut[2350] = 40;
    beamCut[2351] = 36;
    beamCut[2352] = 40;
    beamCut[2355] = 15;
    beamCut[2356] = 40;
    beamCut[2357] = 15;
    beamCut[2358] = 35;
    beamCut[2359] = 35;
    beamCut[2361] = 35;
    beamCut[2362] = 35;
    beamCut[2406] = 14;
    beamCut[2407] = 14;
    beamCut[2408] = 14;
    beamCut[2409] = 14;
    beamCut[2410] = 14;
    beamCut[2411] = 14;
    beamCut[2412] = 14;
    beamCut[2414] = 14;
    beamCut[2415] = 14;
    beamCut[2416] = 14;
    beamCut[2417] = 14;
    beamCut[2418] = 14;
    beamCut[2419] = 14;
    beamCut[2420] = 5;
    beamCut[2421] = 5;
    beamCut[2423] = 2;
    beamCut[2424] = 4;
    beamCut[2425] = 39;
    beamCut[2426] = 48;
    beamCut[2427] = 49;
    beamCut[2428] = 30;
    beamCut[2429] = 20;
    beamCut[2430] = 7;
    beamCut[2431] = 7;
    beamCut[2432] = 7;
    beamCut[2433] = 7;
    beamCut[2434] = 23;
    beamCut[2435] = 25;
    beamCut[2446] = 7;
    beamCut[2447] = 25;
    beamCut[2448] = 7;
    beamCut[2449] = 15;
    beamCut[2450] = 19;
    beamCut[2452] = 29;
    beamCut[2453] = 34;
    beamCut[2456] = 48;
    beamCut[2457] = 30;
    beamCut[2458] = 4;
    beamCut[2459] = 20;
    beamCut[2460] = 48;
    beamCut[2461] = 19;
    beamCut[2462] = 4;
    beamCut[2463] = 4;
    beamCut[2464] = 4;
    beamCut[3179] = 6;
    beamCut[3180] = 49;
    beamCut[3181] = 49;
    beamCut[3183] = 49;
    beamCut[3184] = 49;
    beamCut[3186] = 49;
    beamCut[3187] = 49;
    beamCut[3188] = 49;
    beamCut[3193] = 39;
    beamCut[3198] = 59;
    beamCut[3199] = 59;
    beamCut[3201] = 49;

    return beamCut;

}
