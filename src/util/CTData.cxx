#include "CTData.h"

// Destructor
CTData::~CTData() {
    Clear();
}

// Copy constructor
CTData::CTData(const CTData& ctdata) {
    Copy(ctdata);
}

// Assignment operator
CTData& CTData::operator=(const CTData& ctdata) {
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
CTData::CTData(TString spec, TString config) {
    // Read config.json
    Configure(config);

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

    // TODO: convert runlists to json
    // Loop over <target,Q^2> pairs and load the root files listed in the run lists
    for (auto const &t : targets) {
        for (auto const &q : Q2s) {
            std::cout << "Loading data for " << t << ", Q^2=" << q<< std::endl;
            // Make key for map containing our TChains
            std::pair<TString, Int_t> key = std::make_pair(t, q);

            // Initialize this chain
            chains[key] = new TChain("T");

            // Open run list
            runlistFilename = Form("%s/%s", runlistDir.Data(), runlists[key].Data());
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
                runs[key].push_back(runNumber);
            }
            runlist.close();
        }
    }
}

// Read config.json
void CTData::Configure(TString config) {
    // Read file into one long string for rapidjson to parse
    std::ifstream jsonFile(config);
    std::string jsonString((std::istreambuf_iterator<char>(jsonFile)),
            std::istreambuf_iterator<char>());

    // Parse JSON
    rapidjson::Document dom;
    dom.Parse(jsonString.c_str());

    // TODO: asserts with IsString() etc
    // Get info about where data live
    rapidjson::Value& data = dom["data"];
    runlistDir   = data["runlistDir"].GetString();
    rootfilesDir = data["rootfilesDir"].GetString();
    rootfileTemplateCOIN = data["rootfileTemplateCOIN"].GetString();
    rootfileTemplateSHMS = data["rootfileTemplateSHMS"].GetString();
    rootfileTemplateHMS  = data["rootfileTemplateHMS"].GetString();

    // Get kinematics information
    rapidjson::Value& kinematics = dom["kinematics"];
    for (rapidjson::SizeType i=0; i<kinematics.Size(); i++) {
        // (1) Read
        TString runlist = kinematics[i]["runlist"].GetString();
        TString target  = kinematics[i]["target"].GetString();
        Int_t Q2        = kinematics[i]["Q2"].GetInt();
        Double_t Q2A    = kinematics[i]["Q2Actual"].GetDouble();

        // (2) Store
        runlists[std::make_pair(target,Q2)] = runlist;
        // Only add to the vector if not already there
        Bool_t empty = (targets.size()==0);
        Bool_t exists = (std::find(targets.begin(),targets.end(),target) != targets.end());
        if (empty || !exists) {
            targets.push_back(target);
        }
        empty = (Q2s.size()==0);
        exists = (std::find(Q2s.begin(),Q2s.end(),Q2) != Q2s.end());
        if (empty || !exists) {
            Q2s.push_back(Q2);
            Q2Actual.insert(std::make_pair(Q2,Q2A));
        }
    }

}

// Get chain for specified target and Q^2
TChain* CTData::GetChain(TString target, Int_t Q2) {
    // TODO: check if chain pointer is valid
    std::pair<TString, Int_t> key = std::make_pair(target, Q2);
    return chains[key];
}

// Get runs for specified target and Q^2
std::vector<Int_t> CTData::GetRuns(TString target, Int_t Q2) {
    std::pair<TString, Int_t> key = std::make_pair(target, Q2);
    return runs[key];
}

// Test that chains were all loaded successfully
bool CTData::TestChains() {
    bool status = true;
    for (auto const &t : targets) {
        for (auto const &q : Q2s) {
            std::pair<TString, Int_t> key = std::make_pair(t, q);
            if (chains[key]==NULL) {
                std::cerr << "NULL TChain : " << t << "," << q << std::endl;
                status = false;
            }
        }
    }

    return status;
}
