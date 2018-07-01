#include "CTChains.h"

CTChains::~CTChains() {
    for (auto& chain : chains) {
        delete chain.second;
    }
}

void CTChains::Add(const char* target, int Q2, const char* rootFile) {
    // Key for map
    std::pair<const char*, int> key = std::make_pair(target, Q2);

    // If chain doesn't already exist, initialize it
    if (chains[key]==nullptr) { 
        TChain* newChain = new TChain("T");
        chains.insert(std::make_pair(key, newChain));
    }

    // Add file to the chain
    chains[key]->Add(rootFile);
}

TChain* CTChains::GetChain(const char* target, int Q2) {
    std::pair<const char*, int> key = std::make_pair(target, Q2);
    return chains[key];
}
