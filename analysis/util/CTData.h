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
        // Constructor takes argument spec, which is either COIN, SHMS, or HMS
        // Default is coincidence data
        CTData(TString spec="COIN");

        // Copy constructor
        CTData(const CTData& ctdata);

        ~CTData();

        TChain* GetChain(const char* target, int Q2);

    private:
        std::map<std::pair<const char*, int>, TChain*> chains;

        TString runlistDir = "/home/jmatter/ct_scripts/runlists";
        TString runlistTemplate = "%s/runs_%s_Q2_%d";

        TString rootfilesDir = "/home/jmatter/hallc_replay/ROOTfiles";
        TString rootfileTemplate;

        TString rootfileTemplateCOIN = "%s/coin_replay_production_%d_-1.root";
        TString rootfileTemplateSHMS = "%s/shms_coin_replay_production_%d_1000000.root";
        TString rootfileTemplateHMS  = "%s/hms_coin_replay_production_%d_-1.root";

        std::vector<TString> targets = {"LH2", "C12", "AL"};
        std::vector<int> Q2s = {8, 10, 12, 14};
        std::map<int,double> Q2Actual = {{8, 8.0}, {10, 9.5}, {12, 11.5}, {14, 14.3}};
};

#endif
