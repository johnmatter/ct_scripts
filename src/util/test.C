#include <CTData.h>
#include <CTCuts.h>

void test() {
    CTData* data = new CTData("/home/jmatter/ct_scripts/data.json");
    CTCuts* cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    if (data->TestChains() == true) {
        TString kinematics = "LH2_Q2_8";
        std::cout << "Kinematics: " << kinematics << std::endl;

        std::vector<Int_t> runs = data->GetRuns(kinematics);
        std::cout << "Runs:";
        for (auto run : runs) {
            std::cout << " " << run;
        }
        std::cout << std::endl;

        TChain* chain = data->GetChain(kinematics);
        std::cout << "Chain has nentries:" << chain->GetEntries() << std::endl;

        TString drawStr = "H.gtr.dp>>h(100,-20,20)";
        TCut cut = cuts->Get("coinCutsLH2");

        std::cout << "Test draw" << std::endl;
        std::cout << drawStr << std::endl;
        cut.Print();

        chain->Draw(drawStr,cut);
    } else {
        std::cout << "Failure." << std::endl;
    }
}
