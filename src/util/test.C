#include <CTData.h>
#include <CTCuts.h>

void test() {
    CTData* data = new CTData("COIN");
    CTCuts* cuts = new CTCuts();

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

        std::cout << "Test draw" << std::endl;
        chain->Draw("H.gtr.dp>>h(100,-20,20)", cuts->GetCoinCut(t));
    } else {
        std::cout << "Failure." << std::endl;
    }
}
