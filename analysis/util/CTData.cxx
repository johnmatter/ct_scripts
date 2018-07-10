#include "CTData.h"

// Default constructor
CTData::CTData(TString spec) {
    // Which data are we loading?
    // Template filenames are defined in the header
    if (spec.EqualTo("COIN")) { rootfileTemplate = rootfileTemplateCOIN; }
    if (spec.EqualTo("SHMS")) { rootfileTemplate = rootfileTemplateSHMS; }
    if (spec.EqualTo("HMS"))  { rootfileTemplate = rootfileTemplateHMS;  }

    // Define variables used to read each run's root file
    std::ifstream runlist;
    TString runlistFilename;
    Int_t runNumber;
    TString rootfilename;

    // Loop over <target,Q^2> pairs and load the root files listed in the run lists
    for (auto const &t : targets) {
        std::cout << "Loading data for " << t << std::endl; 
        for (auto const &q : Q2s) {
            // Make key for map containing our TChains
            std::pair<const char*, int> key = std::make_pair(t.Data(), q);

            // Initialize this chain
            chains[key] = new TChain("T");

            // Open run list
            runlistFilename = Form(runlistTemplate, runlistDir.Data(), t.Data(), q);
            // TODO: should check if file exists
            runlist.open(runlistFilename.Data());

            // Read list line-by-line and add each root file
            while(1) {
                // Read line
                runlist >> runNumber;
                if (!runlist.good()) {break;} // end if file's done

                // Add root file to chain
                rootfilename = Form(rootfileTemplate, rootfilesDir.Data(), runNumber);
                // TODO: should check if file exists
                chains[key]->Add(rootfilename);
            }
            runlist.close();
        }
    }
    std::cout << "Finished loading" << std::endl; 
}

// Copy constructor
CTData::CTData(const CTData& ctdata) {
    for (auto& chain : ctdata.chains) {
        // Allocate memory
        chains[chain.first] = new TChain("T");

        // Copy chain
        chains[chain.first] = chain.second;
    }
}

// Destructor
CTData::~CTData() {
    if (chains.size()>0) {
        for (auto& chain : chains) { delete chain.second; }
    }
}

// Get chain for specified target and Q^2
TChain* CTData::GetChain(const char* target, int Q2) {
    // TODO: check if chain pointer is valid 
    std::cout << chains[std::make_pair(target,Q2)] << std::endl;
    return chains[std::make_pair(target,Q2)];
}
