#include <utility>
#include <map>

#include <TCut.h>

#include <CTData.h>
#include <Efficiency1D.h>

void plot_hcer() {
    CTData *data = new CTData("COIN");
    data->Load(); 

    std::map<std::pair<TString, int>, Efficiency1D*> efficiencies;

    TString scanBranch = "H.gtr.dp";
    TCut cutShould = "H.gtr.beta>0.8 && H.gtr.beta<1.3 && H.gtr.index>-1 && H.cal.etottracknorm > 0.75 && H.cal.etottracknorm < 1.5";
    TCut cutDid = "H.cer.npeSum>0.5";
    for (auto const &t : targets) {
        for (auto const &q : Q2s) {
            std::pair<TString, int> key = std::make_pair(t,q);

            efficiencies[key] = new Efficiency1D();

            efficiencies[key]->SetChain(data->GetChain(t.Data(),q));

            efficiencies[key]->SetScanRange(100, -20, 20);
            efficiencies[key]->SetScanBranch(scanBranch);

            efficiencies[key]->SetShouldCut(cutShould);
            efficiencies[key]->SetDidCut(cutDid);

            efficiencies[key]->SetEvents(-1);
            efficiencies[key]->Init();
            efficiencies[key]->Calculate();
        }
    }
}
