#ifndef CTCONFIG_H
#define CTCONFIG_H

#include <TString.h>
#include <vector>
#include <map>

TString runlistDir = "/home/jmatter/ct_scripts/runlists";
TString runlistTemplate = "%s/runs_%s_Q2_%d";

TString rootfilesDir = "/home/jmatter/hallc_replay/ROOTfiles";
TString coinProductionTemplate = "%s/coin_replay_production_%d_-1.root";

std::vector<TString> targets = {"LH2", "C12", "AL"};
std::vector<int> Q2s = {8, 10, 12, 14};
std::map<int,double> Q2Actual = {{8, 8.0}, {10, 9.5}, {12, 11.5}, {14, 14.3}};

#endif
