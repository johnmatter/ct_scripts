#include "CTCuts.h"

// Destructor
CTCuts::~CTCuts() {
    Clear();
}

// Copy constructor
CTCuts::CTCuts(const CTCuts& ctcuts) {
    Copy(ctcuts);
}

// Assignment operator
CTCuts& CTCuts::operator=(const CTCuts& ctcuts) {
    // Protect against self-assignment
    if (this != &ctcuts) {
        Clear();      // prevents memory leak
        Copy(ctcuts); // copy object
    }

    return *this; // for cases like a=b=c
}

// Clear
void CTCuts::Clear() {
    fCuts.clear();
    fJsonFile.Clear();
}

// Copy other CTCuts object to this one
void CTCuts::Copy(const CTCuts& ctcuts) {
    fCuts = ctcuts.fCuts;
    fJsonFile = ctcuts.fJsonFile;
}

// Constructor with specified JSON filename
CTCuts::CTCuts(TString json) {
    SetJson(json);
}

// Set JSON file, parse, and add the cuts
void CTCuts::SetJson(TString json) {
    fJsonFile = json;
    ParseJson();
    AddJsonCuts();
}

// Parse JSON
void CTCuts::ParseJson() {
    // Read file into one long string for rapidjson to parse
    std::ifstream jsonStream(fJsonFile);
    std::string jsonString((std::istreambuf_iterator<char>(jsonStream)),
            std::istreambuf_iterator<char>());

    // Parse JSON
    fJson.Parse(jsonString.c_str());
}

// Add cuts described by JSON data
void CTCuts::AddJsonCuts() {
    // See the CTCuts.h or README for description of json format

    rapidjson::Value& cuts = fJson["cuts"];
    for (rapidjson::SizeType i=0; i<cuts.Size(); i++) {
        // Read
        TString name = cuts[i]["name"].GetString();
        TString cut  = cuts[i]["cut"].GetString();

        // Add
        Add(name, cut);
    }
}

// Add a cut
void CTCuts::Add(TString cutName, TString cutRhs) {
    const char* name = cutName.Data();
    const char* rhs = cutRhs.Data();

    // (1) Construct TCut
    gROOT->ProcessLine(Form("TCut %s(%s);", name, rhs));

    // (2) Trickery to access a pointer to the cut
    gROOT->ProcessLine(Form("%s.SetName(\"%s\");", name, name));
    gROOT->ProcessLine(Form("gROOT->Append(&%s);", name));
    TCut *cutPointer = (TCut*) gROOT->FindObject(name);

    // (3) Add it to the map
    fCuts.insert(std::make_pair(name, *cutPointer));
}

