#include <CTData.h>
#include <CTCuts.h>

void test() {
    CTData* data = new CTData("COIN", "/home/jmatter/ct_scripts/data.json");
    CTCuts* cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    if (data->TestChains() == true) {
        TString t = "LH2";
        Int_t q = 8;

        std::cout << "Kinematics: " << t << ", Q^2 = " << q << " GeV^2" << std::endl;

        std::vector<Int_t> runs = data->GetRuns(t,q);
        std::cout << "Runs:";
        for (auto run : runs) {
            std::cout << " " << run;
        }
        std::cout << std::endl;

        TChain* chain = data->GetChain(t,q);
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
