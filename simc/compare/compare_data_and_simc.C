#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>

#include <TPad.h>
#include <TVirtualPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TPaveLabel.h>
#include <TCut.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TObjArray.h>

#include "CTData.h"
#include "CTCuts.h"

// This script creates histograms to compare quantities in
// data and simc (both with and without radiative corrections)
//
// The default arguments do NOT plot simc without radiative corrections.
void compare_data_and_simc(Int_t drawData=1, Int_t drawSimcWithRadcorr=1, Int_t drawSimcWithoutRadcorr=0) {
    //-------------------------------------------------------------------------------------------------------------------------
    // Specifics of kinematics, where are the data, etc.

    std::vector<Double_t> Q2s = {8, 9.5, 11.5, 14.3};
    std::vector<TString> targets = {"C12_thick", "C12_thin", "LH2"};
    std::vector<TString> collimators = {"pion", "large"};
    std::vector<TString> radcorrSettings = {"with", "without"};

    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::vector<TString> kinematics = data->GetNames();

    // Key is <Q^2, target, collimator, radcorr>
    // Order chosen in decreasing difficulty to change in the hall.
    std::map<std::tuple<Int_t, TString, TString, TString>, Double_t> normfac;

    // Key is <Q^2, target, collimator, radcorr, branch>
    std::map<std::tuple<Double_t, TString, TString, TString, TString>, TH1*> histos;

    // Cuts to apply to simc
    std::map<TString, TCut> simcCuts;
    simcCuts["C12_thick"] = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    simcCuts["C12_thin"]  = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    // simcCuts["LH2"]       = "Em<0.1 && abs(Pm)<0.1 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    simcCuts["LH2"]       = "Em<0.05 && abs(Pm)<0.1 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";

    std::map<TString, TString> targetFilenameMap;
    targetFilenameMap["C12_thick"]="c12";
    targetFilenameMap["C12_thin"]="c12thin";
    targetFilenameMap["LH2"]="h1";

    std::map<Double_t, Int_t> Q2FilenameMap;
    Q2FilenameMap[8]=8;
    Q2FilenameMap[9.5]=95;
    Q2FilenameMap[11.5]=115;
    Q2FilenameMap[14.3]=143;

    TString rootfileDirectory = "/work/hallc/e1206107/jmatter/simc_gfortran/worksim/";
    TString rootfileFormat = Form("%s/%%s_radcorr/%%s_collimator/%%s_q_%%d.root", rootfileDirectory.Data());

    TString normfacFile = "/home/jmatter/ct_scripts/simc/yield/normfac.csv";

    TString pdfFilename = "/home/jmatter/ct_scripts/simc/compare/histos.pdf";

    //-------------------------------------------------------------------------------------------------------------------------
    // What branches do we want to plot?
    // TODO: store these in a json

    TString branchDescription;
    std::vector<TString> branchDescriptions;
    std::map<TString, Int_t> nBins;
    std::map<TString, Double_t> loBin;
    std::map<TString, Double_t> hiBin;
    std::map<TString, TString> simcBranch;
    std::map<TString, TString> dataBranch;
    std::map<TString, Bool_t> logy;

    // Pmiss
    branchDescription = "P_m";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 60;
    loBin[branchDescription] = -0.3;
    hiBin[branchDescription] = +0.3;
    simcBranch[branchDescription] = "TMath::Sign(1,Pmpar)*Pm";
    dataBranch[branchDescription] = "TMath::Sign(1,P.kin.secondary.pmiss_z)*P.kin.secondary.pmiss";
    logy[branchDescription] = false;

    // // Pmiss
    // branchDescription = "Pmiss";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = 0.0;
    // hiBin[branchDescription] = 0.5;
    // simcBranch[branchDescription] = "Pm";
    // dataBranch[branchDescription] = "P.kin.secondary.pmiss";
    // logy[branchDescription] = false;

    // // Pmiss_z
    // branchDescription = "PmissZ";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -1.0;
    // hiBin[branchDescription] = +1.0;
    // simcBranch[branchDescription] = "Pm";
    // dataBranch[branchDescription] = "P.kin.secondary.pmiss_z";
    // logy[branchDescription] = false;

    // // Pmiss_par
    // branchDescription = "PmissPar";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -1.0;
    // hiBin[branchDescription] = +1.0;
    // simcBranch[branchDescription] = "Pmpar";
    // dataBranch[branchDescription] = "P.kin.secondary.?";
    // logy[branchDescription] = false;

    // // Emiss
    // branchDescription = "Emiss";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -0.2;
    // hiBin[branchDescription] = +0.2;
    // simcBranch[branchDescription] = "Em";
    // dataBranch[branchDescription] = "P.kin.secondary.emiss";
    // logy[branchDescription] = false;

    // // Emiss_nuc
    // branchDescription = "Emiss_nuc";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -0.2;
    // hiBin[branchDescription] = +0.2;
    // simcBranch[branchDescription] = "Em";
    // dataBranch[branchDescription] = "P.kin.secondary.emiss_nuc";
    // logy[branchDescription] = false;

    // // W
    // branchDescription = "W";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -2;
    // hiBin[branchDescription] = +2;
    // simcBranch[branchDescription] = "W";
    // dataBranch[branchDescription] = "P.kin.primary.W";
    // logy[branchDescription] = false;

    // // Q2
    // branchDescription = "Q2";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 100;
    // loBin[branchDescription] = 5;
    // hiBin[branchDescription] = 20;
    // simcBranch[branchDescription] = "Q2";
    // dataBranch[branchDescription] = "P.kin.primary.Q2";
    // logy[branchDescription] = false;

    // // hsdelta
    // branchDescription = "HMSDelta";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -10;
    // hiBin[branchDescription] = +10;
    // simcBranch[branchDescription] = "hsdelta";
    // dataBranch[branchDescription] = "H.gtr.dp";
    // logy[branchDescription] = false;

    // // hsytar
    // branchDescription = "HMSytar";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -10;
    // hiBin[branchDescription] = +10;
    // simcBranch[branchDescription] = "hsytar";
    // dataBranch[branchDescription] = "H.gtr.ph";
    // logy[branchDescription] = false;

    // // hsxptar
    // branchDescription = "HMSxptar";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -0.1;
    // hiBin[branchDescription] = +0.1;
    // simcBranch[branchDescription] = "hsxptar";
    // dataBranch[branchDescription] = "H.gtr.th";
    // logy[branchDescription] = false;

    // // hsyptar
    // branchDescription = "HMSyptar";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -0.1;
    // hiBin[branchDescription] = +0.1;
    // simcBranch[branchDescription] = "hsyptar";
    // dataBranch[branchDescription] = "H.gtr.ph";
    // logy[branchDescription] = false;

    // // ssdelta
    // branchDescription = "SHMSDelta";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -10;
    // hiBin[branchDescription] = +10;
    // simcBranch[branchDescription] = "ssdelta";
    // dataBranch[branchDescription] = "P.gtr.dp";
    // logy[branchDescription] = false;

    // // ssytar
    // branchDescription = "SHMSytar";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -10;
    // hiBin[branchDescription] = +10;
    // simcBranch[branchDescription] = "hsytar";
    // simcBranch[branchDescription] = "ssytar";
    // dataBranch[branchDescription] = "P.gtr.ph";
    // logy[branchDescription] = false;

    // // ssxptar
    // branchDescription = "SHMSxptar";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -0.1;
    // hiBin[branchDescription] = +0.1;
    // simcBranch[branchDescription] = "ssxptar";
    // dataBranch[branchDescription] = "P.gtr.th";
    // logy[branchDescription] = false;

    // // ssyptar
    // branchDescription = "SHMSyptar";
    // branchDescriptions.push_back(branchDescription);
    // nBins[branchDescription] = 50;
    // loBin[branchDescription] = -0.1;
    // hiBin[branchDescription] = +0.1;
    // simcBranch[branchDescription] = "ssyptar";
    // dataBranch[branchDescription] = "P.gtr.ph";
    // logy[branchDescription] = false;

    //-------------------------------------------------------------------------------------------------------------------------
    // Read CSV containing normfacs
    std::ifstream ifs(normfacFile.Data(), std::ifstream::in);
    TString line, thisCollimator, thisTarget, thisRadcorr;
    Int_t thisQ2;
    Double_t thisNormfac;

    line.ReadLine(ifs); // skip header
    while(ifs.good()) {
        line.ReadLine(ifs);

        TObjArray *toks;
        TObjString *tok;
        toks = line.Tokenize(",");

        // If line wasn't tokenizable
        if (toks->IsEmpty()) {
            continue;
        }

        // Does this include radiative corrections?
        tok = (TObjString*)(toks->At(0));
        thisRadcorr = string(tok->String());
        thisRadcorr.ReplaceAll("_radcorr","");

        // Collimator
        tok = (TObjString*)(toks->At(1));
        thisCollimator = string(tok->String());

        // Target
        tok = (TObjString*)(toks->At(2));
        thisTarget = string(tok->String());

        // Q^2
        tok = (TObjString*)(toks->At(3));
        thisQ2 = stoi(string(tok->String()));

        // normfac
        tok = (TObjString*)(toks->At(4));
        thisNormfac = stof(string(tok->String()));

        // Store
        normfac[std::make_tuple(thisQ2, thisTarget, thisCollimator, thisRadcorr)] = thisNormfac;
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Loop over kinematics and create histograms for simc
    TTree *T;
    Int_t N;
    TString drawMe, weightStr, cutStr;
    TString histoname;
    TString branch;

    for(auto const &b: branchDescriptions) {
        for(auto const &Q2: Q2s) {
            for(auto const &target: targets) {
                for(auto const &collimator: collimators) {

                    std::cout << "Histogramming simc branch " << b << " for "
                              << "Q^2="        << Q2         << ", "
                              << "target="     << target     << ", "
                              << "collimator=" << collimator << std::endl;

                    for(auto const &radcorr: radcorrSettings) {
                        auto histoKey = std::make_tuple(Q2, target, collimator, radcorr, b);

                        // Form filename
                        // TString rootfileFormat = Form("%s/%%s_radcorr/%%s_collimator/%%s_q_%%d.root", rootfileDirectory.Data());
                        TString rootfilename = Form(rootfileFormat, radcorr.Data(), collimator.Data(), targetFilenameMap[target].Data(), Q2FilenameMap[Q2]);

                        // Try to load, skip if it doesn't exist. Some combos of (Q2,target,collimator)
                        // won't exist because we didn't simulate them.
                        TFile *file = TFile::Open(rootfilename.Data());
                        if(file==nullptr) {
                            continue;
                        }

                        // Get tree
                        file->GetObject("h666", T);
                        N = T->GetEntries();

                        // Get normfac
                        auto normfacKey = std::make_tuple(Q2FilenameMap[Q2], targetFilenameMap[target], collimator, radcorr);
                        thisNormfac = normfac[normfacKey];

                        // Which branch?
                        branch = simcBranch[b];

                        // Draw histogram
                        histoname = Form("Q2_%.1f_%s_%scollimator_%sradcorr_%s", Q2, target.Data(), collimator.Data(), radcorr.Data(), branchDescription.Data());

                        drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(),
                                                          histoname.Data(),
                                                          nBins[b],
                                                          loBin[b],
                                                          hiBin[b]
                                                          );

                        weightStr = Form("Weight*%f/%d", thisNormfac, N);

                        cutStr = Form("(%s)*(%s)", weightStr.Data(), simcCuts[target].GetTitle());

                        T->Draw(drawMe.Data(), cutStr.Data(), "goff");
                        histos[histoKey] = (TH1F*) gDirectory->Get(histoname.Data());

                        // Scale to integral
                        // histos[histoKey]->Scale(1/histos[histoKey]->Integral());

                        // Labels
                        histos[histoKey]->GetXaxis()->SetTitle(b.Data());
                    }
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Loop over kinematics and create histograms for data
    for(auto const &b: branchDescriptions) {
        for(auto const &k: kinematics) {
            std::cout << "Histogramming data branch " << b << " for " << k << std::endl;

            Double_t Q2 = data->GetQ2(k);
            TString target = data->GetTarget(k);
            TString collimator = data->GetCollimator(k);

            branch = dataBranch[b];

            auto histoKey = std::make_tuple(Q2, target, collimator, "data", b);

            // Draw histogram
            histoname = Form("Q2_%.1f_%s_%scollimator_data_%s", Q2, target.Data(), collimator.Data(), branchDescription.Data());

            drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(),
                                              histoname.Data(),
                                              nBins[b],
                                              loBin[b],
                                              hiBin[b]
                                              );

            TCut cut="";
            if (data->GetTarget(k).Contains("C12")) {
                cut = cuts->Get("coinCutsC12");
            }
            if (data->GetTarget(k).Contains("LH2")) {
                cut = cuts->Get("coinCutsLH2");
            }
            if (cut=="") {
                std::cout << "Couldn't find cut for kinematics " << k << std::endl;
            }

            data->GetChain(k)->Draw(drawMe.Data(), cut, "goff");
            histos[histoKey] = (TH1F*) gDirectory->Get(histoname.Data());

            // Scale to integral
            // histos[histoKey]->Scale(1/histos[histoKey]->Integral());

            // Labels
            histos[histoKey]->GetXaxis()->SetTitle(b.Data());
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Loop over kinematics and print histos to PDF

    gStyle->SetOptTitle(kFALSE);
    gStyle->SetOptStat(0);
    // gStyle->SetPalette(kVisibleSpectrum);

    // open PDF
    TCanvas* canvas = new TCanvas("canvas", "compare", 640, 480);
    canvas->Print((pdfFilename+"[").Data());

    for(auto const &b: branchDescriptions) {
        for (auto const &k: kinematics) {
            std::cout << "Printing " << b << " for " << k << std::endl;

            Double_t Q2 = data->GetQ2(k);
            TString target = data->GetTarget(k);
            TString collimator = data->GetCollimator(k);

            auto dataKey    = std::make_tuple(Q2, target, collimator, "data", b);
            auto withKey    = std::make_tuple(Q2, target, collimator, "with", b);
            auto withoutKey = std::make_tuple(Q2, target, collimator, "without", b);

            if ((histos[withKey]==nullptr) || (histos[withoutKey]==nullptr)) {
                std::cout << "Histo not found. Skip printing " << Q2 << "," << target << "," << collimator << "," << branchDescription << std::endl;
                continue;
            }

            // Normalize simc
            histos[withKey]->Scale(1/histos[withKey]->Integral());
            histos[withoutKey]->Scale(1/histos[withoutKey]->Integral());
            // Scale simc to data
            histos[withKey]->Scale(histos[dataKey]->Integral());
            histos[withoutKey]->Scale(histos[dataKey]->Integral());


            // Set colors
            histos[dataKey]->SetLineColor(kBlue);
            histos[withKey]->SetLineColor(kRed);
            histos[withoutKey]->SetLineColor(kGreen);

            // Are we using a logarithmic y axis?
            if (logy[b]==true) {
                canvas->SetLogy(1);
            } else {
                canvas->SetLogy(0);
            }

            // Draw
            // The string drawOpt starts empty, so that the first histo is *not* drawn
            // with the "SAME" option. After the first is drawn, drawOpt becomes "SAME"
            // for subsequent histos.
            TString drawOpt="";
            if (drawData==1) {
                histos[dataKey]->Draw("HIST");
                drawOpt = "SAME";
            }
            if (drawSimcWithRadcorr==1) {
                histos[withKey]->Draw(drawOpt.Data());
                drawOpt = "SAME";
            }
            if (drawSimcWithoutRadcorr==1) {
                histos[withoutKey]->Draw(drawOpt.Data());
            }

            // Add a title
            TString thisTitle = Form("%s, Q^2=%.1f, %s, %s collimator", b.Data(), Q2, target.Data(), collimator.Data());
            TPaveLabel *text = new TPaveLabel(0.0, 0.95, 0.7, 1.0, thisTitle.Data(), "brNDC");
            text->Draw();

            // gPad->BuildLegend();

            canvas->Print(pdfFilename.Data());
        }
    }

    // close PDF
    canvas->Print((pdfFilename+"]").Data());

}
