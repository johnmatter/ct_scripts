#include <vector>
#include <map>

#include <TF1.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TVectorF.h>
#include <TGraphErrors.h>

#include <CTData.h>
#include <CTCuts.h>
#include <Efficiency0D.h>

// Plot efficiency vs pi:e ratio for each kinematics
// Extrapolate to zero pi:e ratio to get "true" efficiency
void hcer() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::map<TString, Efficiency0D*> efficiencies;
    std::map<TString, Double_t> pionElectronRatios;
    std::map<TString, Double_t> pionElectronRatioErrors;

    TCut electronCut = cuts->Get("hElec");
    TCut pionCut = cuts->Get("hPi");

    TCut cutShould = cuts->Get("hCerShould");
    TCut cutDid = cuts->Get("hCerDid");

    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Calculate pi:e ratios
    // This requires creating a TEventList for events passing an electron cut,
    // and one TEventList for events passing a pion cut. Then the ratio of the
    // number of events in this lists gives us pi:e
    TEventList *elist;
    TString elistName;
    TString elistDraw;
    for (auto const &k: kinematics) {
        TChain* chain = data->GetChain(k);
        std::cout << k << " chain is at " << chain << std::endl;

        // Get number of electrons
        elistName.Form("%s_electronElist", k.Data());
        elistDraw.Form(">>%s", elistName.Data());
        chain->Draw(elistDraw, electronCut);
        elist = (TEventList*) gDirectory->Get(elistName.Data());
        Double_t nElectrons = elist->GetN();

        // Get number of pions
        elistName.Form("%s_pionElist", k.Data());
        elistDraw.Form(">>%s", elistName.Data());
        chain->Draw(elistDraw, pionCut);
        elist = (TEventList*) gDirectory->Get(elistName.Data());
        Double_t nPions = elist->GetN();

        // Ratio
        pionElectronRatios[k] = nPions/nElectrons;
        // Error
        // If k&N are Poisson, then uncertainty in k/N is sqrt(k^2*N(k+N)/N^3)
        pionElectronRatioErrors[k] = sqrt(pow(nPions,2)*(nPions+nElectrons)/pow(nElectrons,3));

        // Debug
        TString printMe = Form("%s pi:e = %f", k.Data(), pionElectronRatios[k]);
        std::cout << printMe << std::endl;
    }

    // Calculate efficiencies
    for (auto const &k : kinematics) {
        efficiencies[k]->SetChain(data->GetChain(k));

        efficiencies[k]->SetShouldCut(cutShould);
        efficiencies[k]->SetDidCut(cutDid);

        efficiencies[k]->SetEvents(-1);
        efficiencies[k]->Init();
        efficiencies[k]->Calculate();

        efficiencies[k]->SetTitle(k.Data());
    }

    // Arrange data for TGraphErrors
    Int_t numberOfPoints = kinematics.size();
    TVectorF x(numberOfPoints );
    TVectorF y(numberOfPoints );
    TVectorF ex(numberOfPoints );
    TVectorF ey(numberOfPoints );
    for (int i=0; i<numberOfPoints; i++) {
        TString k = kinematics[i];

        // X-axis is pi:e
        x[i] = pionElectronRatios[k];
        ex[i] = pionElectronRatioErrors[k];

        // Y-axis is efficiency
        y[i] = efficiencies[k]->GetEfficiency();
        // Since we have asymmetric errors here, we'll use the larger one
        Double_t errorUp  = efficiencies[k]->GetEfficiencyErrorUp();
        Double_t errorLow = efficiencies[k]->GetEfficiencyErrorLow();
        ey[i] = (errorUp>errorLow) ? errorUp : errorLow;
    }

    // Get fit
    TGraphErrors *gr = new TGraphErrors(x,y,ex,ey);
    TF1 *f1 = new TF1("f1", "pol1", 0, 15);
    gr->Fit("f1", "V"); // verbose
    Double_t intercept = f1->GetParameter(0);
    Double_t slope =  f1->GetParameter(1);
    std::cout << Form("intercept : %f", intercept) << std::endl;
    std::cout << Form("slope     : %f", slope) << std::endl;

    // Plot
    TCanvas* cHcal = new TCanvas("cHcal", "HCal Efficiency", 1024, 640);
    cHcal->Print("hcal_delta.pdf["); // open PDF
    gr->Draw("A");                   // draw graph on canvas
    cHcal->Print("hcal_delta.pdf");  // print page
    cHcal->Print("hcal_delta.pdf]"); // close PDF
}
