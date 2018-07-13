#include <CTData.h>

void test() {
    CTData* data = new CTData("COIN");
    
    if (data->TestChains() == true) {
        TString t = "LH2";
        Int_t q = 8;

        std::cout << "Getting chain: " << t << ", " << q << std::endl;
        TChain* chain = data->GetChain(t,q);

        std::cout << "Has nentries:" << chain->GetEntries() << std::endl;

        std::cout << "Test draw" << std::endl;
        chain->Draw("H.gtr.dp>>h(100,-20,20)");
    } else {
        std::cout << "Failure." << std::endl;
    }
}
