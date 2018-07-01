#ifndef CTCHAINS_H
#define CTCHAINS_H

#include <map>

#include <TChain.h>
#include <TTree.h>

class CTChains {
    public:
        CTChains() {};
        ~CTChains();

        void Add(const char* target, int Q2, const char* rootFile);
        TChain* GetChain(const char* target, int Q2);

    private:
        std::map<std::pair<const char*, int>, TChain*> chains;
};

#endif
