#include "CTData.h"

// Constructor
CTData::CTData(TString config) {
    configJson = config;
    // (1) Read config
    Configure(configJson);
    // (2) Load data specified by config
    Load();
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

// Destructor
CTData::~CTData() {
    Clear();
}

// Copy data from another object
void CTData::Copy(const CTData& ctdata) {
    configJson = ctdata.configJson;
    // (1) Read config
    Configure(configJson);
    // (2) Load data specified by config
    Load();
}

// Read configuration from json
void CTData::Configure(TString config) {
    std::cout << "Reading CTData config from " << config << std::endl;

    // Read file into one long string for rapidjson to parse
    std::ifstream jsonFile(config);
    std::string jsonString((std::istreambuf_iterator<char>(jsonFile)),
            std::istreambuf_iterator<char>());

    // Parse JSON
    rapidjson::Document dom;
    dom.Parse(jsonString.c_str());

    // TODO: asserts with IsString() etc
    // Get the following info:
    //  - where our data live
    //  - what runs we expect
    //  - what TTrees we expect the files to contain
    rapidjson::Value& data = dom["data"];
    runlistDir       = data["runlistDir"].GetString();
    rootfilesDir     = data["rootfilesDir"].GetString();
    rapidjson::Value& treeList  = data["chains"];
    assert(treeList.IsArray());
    for (rapidjson::SizeType i=0; i<treeList.Size(); i++)
        chainNames.push_back(treeList[i].GetString());

    // Get kinematics information and root filename format
    rapidjson::Value& kinematics = dom["kinematics"];
    for (rapidjson::SizeType i=0; i<kinematics.Size(); i++) {
        // (1) Read
        TString name             = kinematics[i]["name"].GetString();
        Double_t Q2              = kinematics[i]["Q2"].GetDouble();
        Double_t hmsMomentum     = kinematics[i]["hmsP"].GetDouble();
        Double_t shmsMomentum    = kinematics[i]["shmsP"].GetDouble();
        Double_t hmsAngle        = kinematics[i]["hmsAngle"].GetDouble();
        Double_t shmsAngle       = kinematics[i]["shmsAngle"].GetDouble();
        TString runlist          = kinematics[i]["runlist"].GetString();
        TString target           = kinematics[i]["target"].GetString();
        TString collimator       = kinematics[i]["collimator"].GetString();
        TString rootfileTemplate = kinematics[i]["rootfileTemplate"].GetString();

        // (2) Store
        kinematicsNames.push_back(name);
        Q2s[name]               = Q2;
        hmsMomenta[name]        = hmsMomentum;
        shmsMomenta[name]       = shmsMomentum;
        hmsAngles[name]         = hmsAngle;
        shmsAngles[name]        = shmsAngle;
        targets[name]           = target;
        collimators[name]       = collimator;
        runlists[name]          = runlist;
        rootfileTemplates[name] = rootfileTemplate;
    }
}

// Load info from Configure() into runlists and then TChains
void CTData::Load() {
    // Define variables used to read each run's root file
    std::ifstream runlist;
    TString runlistFilename;
    Int_t runNumber;
    TString rootfilename;

    // Loop over vector of kinematicsNames and load the root files listed in the run lists
    std::cout << std::endl;
    std::cout << "Loading TChains" << std::endl;
    std::cout << std::right
              << std::setw(30) << "kinematics"
              << std::setw(10) << "Q2"
              << std::setw(10) << "target"
              << std::setw(10) << "collimator"
              << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (auto const &kinematics : kinematicsNames) {
        std::cout << std::right
                  << std::setw(30) << kinematics
                  << std::setw(10) << Q2s[kinematics]
                  << std::setw(10) << targets[kinematics]
                  << std::setw(10) << collimators[kinematics]
                  << std::endl;

        // Initialize chains
        for (auto const &chain : chainNames) {
            auto key = std::make_pair(kinematics,chain);
            chains[key] = new TChain(chain);
        }

        // Open run list
        runlistFilename = Form("%s/%s", runlistDir.Data(), runlists[kinematics].Data());
        // TODO: should check if file exists
        runlist.open(runlistFilename.Data());

        // Read list line-by-line and add each root file
        while(1) {
            // Read line
            runlist >> runNumber;
            if (!runlist.good()) {break;} // end if file's done

            // Add root file to chains
            rootfilename = Form(rootfileTemplates[kinematics], rootfilesDir.Data(), runNumber);
            // TODO: should check if file exists
            for (auto const &chain : chainNames) {
                auto key = std::make_pair(kinematics,chain);
                chains[key]->Add(rootfilename);
            }
            runs[kinematics].push_back(runNumber);
        }
        runlist.close();
    }
    std::cout << std::endl;
}

// Clear all TChains
void CTData::ClearChains() {
    if (chains.size()>0) {
        for (auto& chain : chains) { delete chain.second; }
    }
    chains.clear();
}

// Clear all data
void CTData::Clear() {
    // Clear TChains
    this->ClearChains();

    runlistDir.Clear();
    rootfilesDir.Clear();

    kinematicsNames.clear();
    chainNames.clear();
    Q2s.clear();
    hmsMomenta.clear();
    shmsMomenta.clear();
    hmsAngles.clear();
    shmsAngles.clear();
    targets.clear();
    collimators.clear();
    runlists.clear();
    runs.clear();
    rootfileTemplates.clear();
}

// Test that chains were all loaded successfully
bool CTData::TestChains() {
    bool status = true;

    // Loop over all kinematics and check if all chains were loaded
    for (auto const &kinematics : kinematicsNames) {
        for (auto const &chain : chainNames) {
            auto key = std::make_pair(kinematics,chain);
            // TODO: fix this. None of ==NULL, ==nullptr, or ==0 seem to actually work.
            if (chains[key] == nullptr) {
                std::cerr << "Missing TChain in kinematics " << kinematics << " : " << chain << std::endl;
                status = false;
            }
        }
    }

    return status;
}

// Get a pointer to the requested TChain
// Default is chain="T"
TChain* CTData::GetChain(TString kinematics, TString chain) {
    // e.g. key = <"LH2_Q2_8","TSH">
    auto const key = std::make_pair(kinematics,chain);
    return chains[key];
}

// Set a new rootfile directory
void CTData::SetRootfileDirectory(TString newRootfilesDir) {
    rootfilesDir = newRootfilesDir;

    // We will need to reload all our TChains, so clear anything already there
    // Hopefully this will prevnt user error of "set new root dir, but forgot to reload the TChains"
    this->ClearChains();
}
