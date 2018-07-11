#include "CTData.h"

// Destructor
CTData::~CTData() {
    std::cout << "destructor" << std::endl;
    Clear();
}

// Copy constructor
CTData::CTData(const CTData& ctdata) {
    std::cout << "copy constructor" << std::endl;
    Copy(ctdata);
}

// Assignment operator
CTData& CTData::operator=(const CTData& ctdata) {
    std::cout << "assignment" << std::endl;
    // Protect against self-assignment
    if (this != &ctdata) {
        Clear();      // prevents memory leak
        Copy(ctdata); // copy object
    }

    return *this; // for cases like a=b=c
}

// Clear
void CTData::Clear() {
    if (chains.size()>0) {
        for (auto& chain : chains) { delete chain.second; }
    }
}

// Copy other CTData object to this one
void CTData::Copy(const CTData& ctdata) {
    for (auto& chain : ctdata.chains) {
        // chain.first is the key: pair<const char* target,int Q^2>
        // chain.second is the value: a TChain*

        // Allocate memory
        chains[chain.first] = new TChain("T");

        // Copy TChain
        chains[chain.first] = chain.second;
    }
}

// Constructor
CTData::CTData(TString spec) {
    std::cout << "constructor" << std::endl;
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
            std::cout << chains[key] << std::endl;

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

// Get chain for specified target and Q^2
TChain* CTData::GetChain(const char* target, int Q2) {
    // TODO: check if chain pointer is valid 
    std::cout << chains[std::make_pair(target,Q2)] << std::endl;
    return chains[std::make_pair(target,Q2)];
}
