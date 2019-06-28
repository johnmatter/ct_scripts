#include <map>
#include <vector>
#include <iostream>
#include <TCut.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
void calculate_yield() {
    std::vector<Double_t> Q2s = {8, 9.5, 11.5, 14.3};
    std::vector<TString> targets = {"C12_thick", "C12_thin", "LH2"};
    std::vector<TString> collimators = {"pion", "large"};

    std::map<TString, TCut> cuts;
    cuts["C12_thick"] = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
    cuts["C12_thin"]  = "Em<0.08 && abs(Pm)<0.3 && hsdelta<8 && hsdelta>-8 && ssdelta<15 && ssdelta>-10";
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

    TString rootfileDirectory = "/work/hallc/e1206107/jmatter/simc_gfortran/worksim/with_radcorr";
    TString rootfileFormat = Form("%s/%%s_collimator/%%s_q_%%d.root", rootfileDirectory.Data());

    TTree *T;
    Int_t N;
    Double_t normfac;
    TString drawMe, weightStr;
    for(const auto& collimator: collimators) {
        for(const auto& Q2: Q2s) {
            for(const auto& target: targets) {
                // Form filename
                TString rootfilename = Form(rootfileFormat, collimator.Data(), targetFilenameMap[target].Data(), Q2FilenameMap[Q2]);
                std::cout << "Trying to load " << rootfilename << std::endl;

                // Try to load
                TFile *file = TFile::Open(rootfilename.Data());
                if(file==nullptr) {
                    std::cout << "Couldn't load" << std::endl;
                    continue;
                }

                // Get tree
                file->GetObject("h666", T);
                N = T->GetEntries();

                drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(), histoname.Data(),
                                                  nbins, lobin, hibin);
                weightStr = Form("Weight*%f/%d", normfac, N);
                h666->Draw(drawMe.Data(), weightStr.Data())
            }
        }
    }
}
