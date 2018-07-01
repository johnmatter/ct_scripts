#include "CTData.h"

CTData::~CTData() {
}

void CTData::Load() {

    std::ifstream runlist;
    TString runlistFilename;
    Int_t runNumber;
    TString rootfile;

    chains = new CTChains();

    // Loop over (target,Q^2) pairs and load the rootfiles listed in runlists
    for (auto const &t : targets) {
        for (auto const &q : Q2s) {
            runlistFilename = Form(runlistTemplate, runlistDir.Data(), t.Data(), q);
            runlist.open(runlistFilename.Data());

            while(1) {
                // Read line until file's done
                runlist >> runNumber;
                if (!runlist.good()) {break;}

                // Add rootfile to chain
                rootfile = Form(coinProductionTemplate, rootfilesDir.Data(), runNumber);
                chains->Add(t.Data(), q, rootfile.Data());
            }
            runlist.close();
        }
    }
}

TChain* CTData::GetChain(const char* target, int Q2) {
    return chains->GetChain(target, Q2);
}
