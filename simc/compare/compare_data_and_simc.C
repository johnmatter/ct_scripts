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

// This script creates histograms to compare quantities in
// data and simc (both with and without radiative corrections)
void compare_data_and_simc() {
    //-------------------------------------------------------------------------------------------------------------------------
    // Specifics of kinematics, where are the data, etc.

    std::vector<Double_t> Q2s = {8, 9.5, 11.5, 14.3};
    std::vector<TString> targets = {"C12_thick", "C12_thin", "LH2"};
    std::vector<TString> collimators = {"pion", "large"};
    std::vector<TString> radcorrSettings = {"with", "without"};

    // Key is <Q^2, target, collimator, radcorr>
    // Order chosen in decreasing difficulty to change in the hall.
    std::map<std::tuple<Int_t, TString, TString, TString>, Double_t> normfac;

    // Key is <Q^2, target, collimator, radcorr, branch>
    std::map<std::tuple<Double_t, TString, TString, TString, TString>, TH1*> histos;

    // Cuts to apply to simc
    std::map<TString, TCut> cuts;
    cuts["C12_thick"] = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    cuts["C12_thin"]  = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    // cuts["LH2"]       = "Em<0.1 && abs(Pm)<0.1 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    cuts["LH2"]       = "Em<0.05 && abs(Pm)<0.1 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";

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

    TString branchDescription;
    std::vector<TString> branchDescriptions;
    std::map<TString, Int_t> nBins;
    std::map<TString, Double_t> loBin;
    std::map<TString, Double_t> hiBin;
    std::map<TString, TString> simcBranch;
    std::map<TString, TString> dataBranch;

    // Pmiss
    branchDescription = "Pmiss";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = 0.0;
    hiBin[branchDescription] = 0.5;
    simcBranch[branchDescription] = "Pm";
    dataBranch[branchDescription] = "P.kin.secondary.pmiss";

    // Pmiss_z
    branchDescription = "PmissZ";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -1.0;
    hiBin[branchDescription] = +1.0;
    simcBranch[branchDescription] = "Pm";
    dataBranch[branchDescription] = "P.kin.secondary.pmiss_z";

    // Pmiss_par
    branchDescription = "PmissPar";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -1.0;
    hiBin[branchDescription] = +1.0;
    simcBranch[branchDescription] = "Pmpar";
    dataBranch[branchDescription] = "P.kin.secondary.?";

    // Emiss
    branchDescription = "Emiss";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -0.2;
    hiBin[branchDescription] = +0.2;
    simcBranch[branchDescription] = "Em";
    dataBranch[branchDescription] = "P.kin.secondary.emiss";

    // Emiss_nuc
    branchDescription = "Emiss_nuc";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -0.2;
    hiBin[branchDescription] = +0.2;
    simcBranch[branchDescription] = "Em";
    dataBranch[branchDescription] = "P.kin.secondary.emiss_nuc";

    // W
    branchDescription = "W";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -2;
    hiBin[branchDescription] = +2;
    simcBranch[branchDescription] = "W";
    dataBranch[branchDescription] = "P.kin.primary.W";

    // Q2
    branchDescription = "Q2";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 100;
    loBin[branchDescription] = 5;
    hiBin[branchDescription] = 20;
    simcBranch[branchDescription] = "Q2";
    dataBranch[branchDescription] = "P.kin.primary.Q2";

    // hsdelta
    branchDescription = "HMSDelta";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -10;
    hiBin[branchDescription] = +10;
    simcBranch[branchDescription] = "hsdelta";
    dataBranch[branchDescription] = "H.gtr.dp";

    // hsytar
    branchDescription = "HMSytar";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -10;
    hiBin[branchDescription] = +10;
    simcBranch[branchDescription] = "hsytar";
    dataBranch[branchDescription] = "H.gtr.ph";

    // hsxptar
    branchDescription = "HMSxptar";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -0.1;
    hiBin[branchDescription] = +0.1;
    simcBranch[branchDescription] = "hsxptar";
    dataBranch[branchDescription] = "H.gtr.th";

    // hsyptar
    branchDescription = "HMSyptar";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -0.1;
    hiBin[branchDescription] = +0.1;
    simcBranch[branchDescription] = "hsyptar";
    dataBranch[branchDescription] = "H.gtr.ph";

    // ssdelta
    branchDescription = "SHMSDelta";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -10;
    hiBin[branchDescription] = +10;
    simcBranch[branchDescription] = "ssdelta";
    dataBranch[branchDescription] = "P.gtr.dp";

    // ssytar
    branchDescription = "SHMSytar";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -10;
    hiBin[branchDescription] = +10;
    simcBranch[branchDescription] = "hsytar";
    simcBranch[branchDescription] = "ssytar";
    dataBranch[branchDescription] = "P.gtr.ph";

    // ssxptar
    branchDescription = "SHMSxptar";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -0.1;
    hiBin[branchDescription] = +0.1;
    simcBranch[branchDescription] = "ssxptar";
    dataBranch[branchDescription] = "P.gtr.th";

    // ssyptar
    branchDescription = "SHMSyptar";
    branchDescriptions.push_back(branchDescription);
    nBins[branchDescription] = 50;
    loBin[branchDescription] = -0.1;
    hiBin[branchDescription] = +0.1;
    simcBranch[branchDescription] = "ssyptar";
    dataBranch[branchDescription] = "P.gtr.ph";

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
    // Loop over kinematics and create histograms
    TTree *T;
    Int_t N;
    TString drawMe, weightStr, cutStr;
    TString histoname;
    TString branch;
    for(auto const b: branchDescriptions) {
        for(const auto& Q2: Q2s) {
            std::cout << "Histogramming " << b << " for Q^2=" << Q2 << std::endl;
            for(const auto& target: targets) {
                for(const auto& collimator: collimators) {
                    for(const auto& radcorr: radcorrSettings) {
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

                        cutStr = Form("(%s)*(%s)", weightStr.Data(), cuts[target].GetTitle());

                        T->Draw(drawMe.Data(), weightStr.Data(), "goff");
                        histos[histoKey] = (TH1F*) gDirectory->Get(histoname.Data());
                    }
                }
            }
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
    canvas->SetLogy(1);

    for(auto const b: branchDescriptions) {
        for(const auto& Q2: Q2s) {
            std::cout << "Printing " << b << " for Q^2=" << Q2 << std::endl;
            for(const auto& target: targets) {
                for(const auto& collimator: collimators) {
                    auto withKey    = std::make_tuple(Q2, target, collimator, "with", b);
                    auto withoutKey = std::make_tuple(Q2, target, collimator, "without", b);

                    if ((histos[withKey]==nullptr) || (histos[withoutKey]==nullptr)) {
                        std::cout << "Histo not found. Skip printing " << Q2 << "," << target << "," << collimator << "," << branchDescription << std::endl;
                        continue;
                    }

                    histos[withKey]->SetLineColor(kRed);
                    histos[withoutKey]->SetLineColor(kGreen);

                    // Draw
                    histos[withKey]->Draw();
                    histos[withoutKey]->Draw("SAME");

                    // Add a title
                    TString thisTitle = Form("%s, Q^2=%.1f, %s, %s collimator", b.Data(), Q2, target.Data(), collimator.Data());
                    TPaveLabel *text = new TPaveLabel(0.0, 0.95, 0.7, 1.0, thisTitle.Data(), "brNDC");
                    text->Draw();

                    // gPad->BuildLegend();

                    canvas->Print(pdfFilename.Data());
                }
            }
        }
    }

    // close PDF
    canvas->Print((pdfFilename+"]").Data());

}
