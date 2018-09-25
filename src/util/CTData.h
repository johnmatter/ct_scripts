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
        CTData(TString config="");               // constructor
        CTData(const CTData& ctdata);            // copy constructor
        CTData& operator=(const CTData& ctdata); // assignment
        ~CTData();                               // destructor

        void Copy(const CTData& ctdata); // Copy method
        void Configure(TString config);  // Read config info from json
        void Load();                     // Load data after object is configured
        void Clear();                    // Clear data

        bool TestChains();               // Test that chains are non-null

        // Various GetX() methods
        std::vector<TString> GetNames() { return names; };
        std::vector<Int_t> GetRuns(TString name) { return runs[name]; };
        TChain* GetChain(TString name) { return chains[name]; };
        TString GetTarget(TString name) { return targets[name]; };
        Double_t GetQ2(TString name) { return Q2s[name]; };
        Double_t GetHMSMomentum(TString name) { return hmsMomenta[name]; };
        Double_t GetSHMSMomentum(TString name) { return shmsMomenta[name]; };
        Double_t GetHMSAngle(TString name) { return hmsAngles[name]; };
        Double_t GetSHMSAngle(TString name) { return shmsAngles[name]; };

    private:
        TString configJson;
        TString runlistDir;
        TString rootfilesDir;

        // This information is read by Config()
        // TODO: Implement this as someCTKinematics class rather than parallel maps
        std::vector<TString> names;
        std::map<TString, Double_t> Q2s;
        std::map<TString, Double_t> hmsMomenta;
        std::map<TString, Double_t> shmsMomenta;
        std::map<TString, Double_t> hmsAngles;
        std::map<TString, Double_t> shmsAngles;
        std::map<TString, TString> targets;
        std::map<TString, TString> runlists;
        std::map<TString, TString> rootfileTemplates;

        // This data is read by Load()
        std::map<TString, std::vector<Int_t>> runs;
        std::map<TString, TChain*> chains;
};

#endif
