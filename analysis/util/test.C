#include <CTData.h>

void test() {
    CTData *data = new CTData("COIN");

    TString t = "LH2";
    int q = 8;

    TChain *chain = data->GetChain(t.Data(),q);
    std::cout << chain->GetEntries() << std::endl;
    //chain->Draw("H.cer.xAtCer:H.cer.yAtCer",cutShould);
}
