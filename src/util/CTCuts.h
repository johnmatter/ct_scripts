#ifndef CTCUTS_H
#define CTCUTS_H

#include <streambuf>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <map>

#include <TROOT.h>
#include <TCut.h>
#include <TChain.h>
#include <TString.h>

#include "../rapidjson/document.h"

// JSON File should look like:
// {
//     "cuts" : [
//         {
//             "name" : "cal",
//             "cut"  : "\"H.cal.etottracknorm>0.7&&H.cal.etottracknorm<1.5\""
//         },
//         {
//             "name" : "cer",
//             "cut"  : "\"H.cer.npeSume>1.0\""
//         },
//         {
//             "name" : "pid",
//             "cut"  : "cal && cer"
//         },
//         {
//             "name" : "clean",
//             "cut"  : "pid && \"H.dc.ntrack>0 && H.hodo.goodstarttime==1\""
//         }
//     ]
// }
//
// Note the escaped quotation marks in particular.

class CTCuts {
    public:
        ~CTCuts();
        CTCuts();
        CTCuts(TString json);
        CTCuts(const CTCuts& ctcuts);

        CTCuts& operator=(const CTCuts& ctcuts);

        void Clear();
        void Copy(const CTCuts& ctcuts);

        TCut Get(TString cutName) { return fCuts[cutName]; };
        void Add(TString cutName, TString cutRhs);

        void SetJson(TString json);

        Int_t NCuts() { return fCuts.size(); };

    private:
        // fCuts takes the name of a cut and returns the TCut
        std::map<TString, TCut> fCuts;

        // Location of json file containing cut specifications
        TString fJsonFile;

        // Actual JSON data
        rapidjson::Document fJson;

        void ParseJson();
        void AddJsonCuts();
};

#endif
