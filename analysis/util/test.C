#include <CTData.h>

void test() {
    CTData* data = new CTData("COIN");
    
    TString t = "LH2";
    int q = 8;

    std::cout << "get" << std::endl;
    TChain* chain = data->GetChain(t.Data(),q);
    std::cout << "got" << std::endl;

    std::cout << chain->GetEntries() << std::endl;
    //chain->Draw("H.cer.xAtCer:H.cer.yAtCer",cutShould);
}
