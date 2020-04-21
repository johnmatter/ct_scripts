#include <utility>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include <TCanvas.h>
#include <TFile.h>
#include <TROOT.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCut.h>

void draw_simc_hms_fp() {

    std::vector<Double_t> q2s = {9.5, 11.5};

    std::map<Double_t, TFile*> fsimc;
    fsimc[9.5]   = new TFile("/Volumes/ssd750/ct/simc/worksim/with_radcorr/pion_collimator/h1_q_95.root");
    fsimc[11.5]  = new TFile("/Volumes/ssd750/ct/simc/worksim/with_radcorr/large_collimator/h1_q_115.root");

    std::map<Double_t, TH2F*> histos;

    std::map<Double_t, TCut> ppidcut, hpidcut, emisscut, pdeltacut, hdeltacut, pslopecut, hslopecut, ztarcut;

    emisscut[9.5]  = "Em<0.030";
    emisscut[11.5] = "Em<0.030";

    hdeltacut[9.5]  = "-5.0 < hsdelta && hsdelta < 6.5";
    hdeltacut[11.5] = "-6.0 < hsdelta && hsdelta < 8.0";

    pdeltacut[9.5]  = "-3.5 < ssdelta && ssdelta < 7.5";
    pdeltacut[11.5] = "-4.0 < ssdelta && ssdelta < 6.0";

    hslopecut[9.5]  = "-0.035 < hsxptar && hsxptar < 0.040 && -0.015 < hsyptar && hsyptar < 0.015";
    hslopecut[11.5] = "-0.050 < hsxptar && hsxptar < 0.060 && -0.015 < hsyptar && hsyptar < 0.020";

    pslopecut[9.5]  = "-0.045 < ssxptar && ssxptar < 0.040 && -0.020 < ssyptar && ssyptar < 0.020";
    pslopecut[11.5] = "-0.045 < ssxptar && ssxptar < 0.040 && -0.020 < ssyptar && ssyptar < 0.020";

    std::map<Double_t, Double_t> normfac;
    normfac[9.5]  = 0.902120E+07;
    normfac[11.5] = 0.158321E+08;

    TFile fWrite("simc_hms_fp.root", "recreate");

    for (auto const q2: q2s) {
        TTree *T;
        fsimc[q2]->GetObject("h666", T);

        // simc weighting
        Int_t N = T->GetEntries();
        TString weightCut = Form("Weight*%f/%d", normfac[q2], N);

        TCut cut = emisscut[q2] * hdeltacut[q2] * pdeltacut[q2] * hslopecut[q2] * pslopecut[q2] * weightCut;

        TString histoName = Form("hms_fp_q2_%.1f", q2);

        TString drawMe = Form("hsxfp:hsyfp>>%s(27,-12,15,45,-20,25)", histoName.Data());

        T->Draw(drawMe.Data(), cut, "goff");

        histos[q2] = (TH2F*) gDirectory->Get(histoName.Data());

        histos[q2]->Write();
    }

    fWrite.Close();
}
