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
    // Get info about where data live, how files are named
    rapidjson::Value& data = dom["data"];
    runlistDir       = data["runlistDir"].GetString();
    rootfilesDir     = data["rootfilesDir"].GetString();

    // Get kinematics information
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
        TString rootfileTemplate = kinematics[i]["rootfileTemplate"].GetString();

        // (2) Store
        names.push_back(name);
        Q2s[name]               = Q2;
        hmsMomenta[name]        = hmsMomentum;
        shmsMomenta[name]       = shmsMomentum;
        hmsAngles[name]         = hmsAngle;
        shmsAngles[name]        = shmsAngle;
        targets[name]           = target;
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

    // Loop over vector of names and load the root files listed in the run lists
    std::cout << std::endl;
    std::cout << "Loading TChains" << std::endl;
    std::cout << std::right
              << std::setw(30) << "name"
              << std::setw(10) << "Q2"
              << std::setw(10) << "target" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (auto const &name : names) {
        std::cout << std::right
                  << std::setw(30) << name
                  << std::setw(10) << Q2s[name]
                  << std::setw(10) << targets[name] << std::endl;

        // Initialize this chain
        chains[name] = new TChain("T");

        // Open run list
        runlistFilename = Form("%s/%s", runlistDir.Data(), runlists[name].Data());
        // TODO: should check if file exists
        runlist.open(runlistFilename.Data());

        // Read list line-by-line and add each root file
        while(1) {
            // Read line
            runlist >> runNumber;
            if (!runlist.good()) {break;} // end if file's done

            // Add root file to chain
            rootfilename = Form(rootfileTemplates[name], rootfilesDir.Data(), runNumber);
            // TODO: should check if file exists
            chains[name]->Add(rootfilename);
            runs[name].push_back(runNumber);
        }
        runlist.close();
    }
    std::cout << std::endl;
}

// Clear
void CTData::Clear() {
    if (chains.size()>0) {
        for (auto& chain : chains) { delete chain.second; }
    }
    chains.clear();

    runlistDir.Clear();
    rootfilesDir.Clear();

    names.clear();
    Q2s.clear();
    hmsMomenta.clear();
    shmsMomenta.clear();
    hmsAngles.clear();
    shmsAngles.clear();
    targets.clear();
    runlists.clear();
    runs.clear();
    rootfileTemplates.clear();
}

// Test that chains were all loaded successfully
bool CTData::TestChains() {
    bool status = true;
    for (auto const &name : names) {
        if (chains[name]==NULL) {
            std::cerr << "NULL TChain : " << name << std::endl;
            status = false;
        }
    }

    return status;
}
