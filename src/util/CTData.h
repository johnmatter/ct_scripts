#ifndef CTDATA_H
#define CTDATA_H

#include <streambuf>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <map>

#include <TChain.h>
#include <TString.h>

#include "../rapidjson/document.h"

class CTData {
    public:
        ~CTData(); // destructor
        CTData(TString spec="COIN", TString config="");  // constructor; argument is COIN, SHMS, or HMS
        CTData(const CTData& ctdata); // copy constructor

        CTData& operator=(const CTData& ctdata); // assignment operator

        void Clear();
        void Copy(const CTData& ctdata);

        void Configure(TString config);

        TChain* GetChain(TString target, Int_t Q2);
        std::vector<Int_t> GetRuns(TString target, Int_t Q2);

        Double_t GetQ2Actual(Int_t q) { return Q2Actual[q]; };

        bool TestChains();

    private:
        std::map<std::pair<TString, Int_t>, TChain*> chains;
        std::map<std::pair<TString, Int_t>, std::vector<Int_t>> runs;

        TString runlistDir;
        TString rootfilesDir;
        TString rootfileTemplate;
        TString rootfileTemplateCOIN;
        TString rootfileTemplateSHMS;
        TString rootfileTemplateHMS;

        // TODO: Write a CTKinematics class instead of using parallel vectors
        std::map<std::pair<TString, Int_t>, TString> runlists;
        std::vector<TString> targets;
        std::vector<Int_t> Q2s;
        std::map<Int_t,Double_t> Q2Actual;
};

#endif
