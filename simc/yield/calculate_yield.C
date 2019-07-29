#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>

#include <TH1.h>
#include <TCut.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TObjArray.h>
void calculate_yield() {
    //-------------------------------------------------------------------------------------------------------------------------
    // Specifics of kinematics, where are the data, etc.

    std::vector<Double_t> Q2s = {8, 9.5, 11.5, 14.3};
    std::vector<TString> targets = {"C12_thick", "C12_thin", "LH2"};
    std::vector<TString> collimators = {"pion", "large"};

    // Key is <Q^2, target, collimator>
    // Order chosen in decreasing difficulty to change in the hall.
    std::map<std::tuple<Double_t, TString, TString>, Double_t> yield;
    std::map<std::tuple<Int_t, TString, TString>, Double_t> normfac;

    std::map<TString, TCut> cuts;
    // cuts["C12_thick"] = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    // cuts["C12_thin"]  = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    // cuts["LH2"]       = "Em<0.1 && abs(Pm)<0.1 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    // cuts["LH2"]       = "Em<0.05 && abs(Pm)<0.1 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    cuts["C12_thick"] = "Em<0.08 && abs(Pm)<0.3";
    cuts["C12_thin"]  = "Em<0.08 && abs(Pm)<0.3";
    cuts["LH2"]       = "Em<0.1 && abs(Pm)<0.1";

    std::map<TString, TString> targetFilenameMap;
    targetFilenameMap["C12_thick"]="c12";
    targetFilenameMap["C12_thin"]="c12thin";
    targetFilenameMap["LH2"]="h1";

    std::map<Double_t, Int_t> Q2FilenameMap;
    Q2FilenameMap[8]=8;
    Q2FilenameMap[9.5]=95;
    Q2FilenameMap[11.5]=115;
    Q2FilenameMap[14.3]=143;

    TString rootfileDirectory = "/work/hallc/e1206107/jmatter/simc_gfortran/worksim/with_radcorr";
    TString rootfileFormat = Form("%s/%%s_collimator/%%s_q_%%d.root", rootfileDirectory.Data());

    TString normfacFile = "/home/jmatter/ct_scripts/simc/yield/normfac.csv";

    // Where to save yield
    TString csvFilename = "/home/jmatter/ct_scripts/simc/yield/simc.csv";

    //-------------------------------------------------------------------------------------------------------------------------
    // Read CSV containing normfacs
    std::ifstream ifs(normfacFile.Data(), std::ifstream::in);
    TString line, thisCollimator, thisTarget;
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

        // Skip if not normfac from with_radcorr directory
        // The without_radcorr is mostly just useful for comparing emiss&pmiss
        // radiative tails in simc vs data. For our transparency calculations,
        // we want to include the radiative corrections.
        tok = (TObjString*)(toks->At(0));
        if (tok->String() == "without_radcorr") {
            continue;
        }

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
        normfac[std::make_tuple(thisQ2, thisTarget, thisCollimator)] = thisNormfac;
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Loop over kinematics and calculate yield
    TTree *T;
    Int_t N;
    TH1F* histo;
    TString drawMe, weightStr, cutStr;
    TString histoname;
    TString branch = "hsdelta";
    Int_t nbins = 25;
    Double_t lobin = -15;
    Double_t hibin = 10;
    for(const auto& Q2: Q2s) {
        for(const auto& target: targets) {
            for(const auto& collimator: collimators) {
                // Form filename
                TString rootfilename = Form(rootfileFormat, collimator.Data(), targetFilenameMap[target].Data(), Q2FilenameMap[Q2]);

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
                thisNormfac = normfac[std::make_tuple(Q2FilenameMap[Q2], targetFilenameMap[target], collimator)];

                // Draw histogram
                histoname = Form("Q2_%.1f_%s_%s_%s", Q2, target.Data(), collimator.Data(), branch.Data());

                drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(), histoname.Data(),
                                                  nbins, lobin, hibin);

                weightStr = Form("Weight*%f/%d", thisNormfac, N);

                cutStr = Form("(%s)*(%s)", weightStr.Data(), cuts[target].GetTitle());

                T->Draw(drawMe.Data(), cutStr.Data(), "goff");
                histo = (TH1F*) gDirectory->Get(histoname.Data());

                // Get yield from integral. Bin=0 underflow. Bin n+1=overflow
                auto key = std::make_tuple(Q2, target, collimator);
                yield[key] = histo->Integral(1,nbins);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------
    // Loop over kinematics and print yield to csv

    // open csv
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    ofs << "Q2,normfac,target,collimator,yield" << std::endl;
    for(const auto& Q2: Q2s) {
        for(const auto& target: targets) {
            for(const auto& collimator: collimators) {
                auto key = std::make_tuple(Q2, target, collimator);

                // Some keys won't have corresponding yields, as noted above
                if(yield.count(key)==1) {
                    thisNormfac = normfac[std::make_tuple(Q2FilenameMap[Q2], targetFilenameMap[target], collimator)];
                    ofs << Form("%.1f,%.1f,%s,%s,%f", Q2, thisNormfac, target.Data(), collimator.Data(), yield[key]) << std::endl;
                }
            }
        }
    }

    ofs.close();
}
