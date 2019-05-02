#include <vector>
#include <map>

#include <TCut.h>
#include <TVectorF.h>

#include <CTData.h>
#include <CTCuts.h>
#include <Efficiency0D.h>

// Display pi:e ratio for all kinematics
void print_ratios() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::map<TString, Int_t> pions;
    std::map<TString, Int_t> electrons;
    std::map<TString, Double_t> pionElectronRatios;
    std::map<TString, Double_t> pionElectronRatioErrors;

    TCut electronCut = cuts->Get("hElec");
    TCut pionCut = cuts->Get("hPi");

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

        pions[k] = nPions;
        electrons[k] = nElectrons;

        // Ratio
        pionElectronRatios[k] = nPions/nElectrons;
        // Error
        // If k&N are Poisson, then uncertainty in k/N is sqrt(k^2*N(k+N)/N^3)
        // I'm not sure this is the correct calculation.
        pionElectronRatioErrors[k] = sqrt(pow(nPions,2)*(nPions+nElectrons)/pow(nElectrons,3));
    }

    // Print to screen
    std::cout << "kinematics,target,Q2,hmsAngle,shmsAngle,hmsMomentum,shmsMomentum,nPi,nE,pi_to_e,pi_to_e_error" << std::endl;
    for (auto &k: kinematics) {
        // Spectrometer info
        TString target        = data->GetTarget(k);
        Double_t Q2           = data->GetQ2(k);
        Double_t hmsAngle     = data->GetHMSAngle(k);
        Double_t shmsAngle    = data->GetSHMSAngle(k);
        Double_t hmsMomentum  = data->GetHMSMomentum(k);
        Double_t shmsMomentum = data->GetSHMSMomentum(k);

        // Ratio info
        Int_t pi              = pions[k];
        Int_t e               = electrons[k];
        Double_t ratio        = pionElectronRatios[k];
        Double_t ratioError   = pionElectronRatioErrors[k];

        // Form print string
        TString printMe = Form("%s,%s,%f,%f,%f,%f,%f,%d,%d,%f,%f",
                                k.Data(), target.Data(), Q2,
                                hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
                                pi, e, ratio, ratioError);
        std::cout << printMe << std::endl;
    }
}
