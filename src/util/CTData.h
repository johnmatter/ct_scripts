#ifndef CTDATA_H
#define CTDATA_H

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <map>

#include <TChain.h>
#include <TString.h>

class CTData {
    public:
        ~CTData();                    // destructor
        CTData(TString spec="COIN");  // constructor; argument is COIN, SHMS, or HMS
        CTData(const CTData& ctdata); // copy constructor

        CTData& operator=(const CTData& ctdata); // assignment operator

        void Clear();
        void Copy(const CTData& ctdata);

        TChain* GetChain(TString target, Int_t Q2);

        Double_t GetQ2Actual(Int_t q) { return Q2Actual[q]; };

        bool TestChains();

    private:
        std::map<std::pair<TString, Int_t>, TChain*> chains;

        TString runlistDir = "/home/jmatter/ct_scripts/runlists";
        TString runlistTemplate = "%s/runs_%s_Q2_%d";

        TString rootfilesDir = "/work/hallc/e1206107/CT12GeV/ct_replay/ROOTfiles";
        TString rootfileTemplate;

        TString rootfileTemplateCOIN = "%s/coin_replay_production_%d_-1.root";
        TString rootfileTemplateSHMS = "%s/shms_coin_replay_production_%d_-1.root";
        TString rootfileTemplateHMS  = "%s/hms_coin_replay_production_%d_-1.root";

        std::vector<TString> targets = {"LH2", "C12", "AL"};
        std::vector<Int_t> Q2s = {8, 10, 12, 14};
        std::map<Int_t,Double_t> Q2Actual = {{8, 8.0}, {10, 9.5}, {12, 11.5}, {14, 14.3}};
};

#endif
