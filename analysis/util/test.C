#include <CTData.h>

void test() {
    CTData* data = new CTData("COIN");
    
    if (data->TestChains() == true) {
        TString t = "LH2";
        Int_t q = 8;
        std::cout << "getting chain" << std::endl;
        TChain* chain = data->GetChain(t,q);
        std::cout << "got nentries:" << std::endl;
        std::cout << chain->GetEntries() << std::endl;
    } else {
        std::cout << "Failure." << std::endl;
    }
}
