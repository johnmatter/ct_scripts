#ifndef CTDATA_H
#define CTDATA_H

#include <fstream>

#include <TChain.h>

#include "CTChains.h"
#include "CTConfig.h"

class CTData {
    public:
        CTData () {}
        ~CTData();

        void Load();
        TChain* GetChain(const char* target, int Q2);
    private:
        CTChains* chains;

};

#endif
