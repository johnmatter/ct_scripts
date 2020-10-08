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

// {
void draw_ngc_npe(Double_t q2) {

    std::map<Double_t, TFile*> fcoin, fsing, fdummy;
    TTree *tcoin, *tsing, *tdummy;
    std::vector<Double_t> q2s = {11.5};

    // // Q^2 = 8
    // fcoin[8.0]  = new TFile("coin_replay_production_LH2_8_smallcoll.root");
    // fsing[8.0]  = new TFile("lh2_hms_singles_q2_8.root");
    // fdummy[8.0] = new TFile("dummy_hms_q2_8.root");

    // // Q^2 = 9.5
    // fcoin[9.5]  = new TFile("coin_replay_production_LH2_9.5_smallcoll.root");
    // fsing[9.5]  = new TFile("lh2_hms_singles_q2_9.5.root");
    // fdummy[9.5] = new TFile("dummy_hms_q2_9.5.root");

    // Q^2 = 11.5
    fcoin[11.5]  = new TFile("coin_replay_production_LH2_11.5_largecoll.root");
    fsing[11.5]  = new TFile("lh2_hms_singles_q2_11.5.root");
    fdummy[11.5] = new TFile("dummy_hms_q2_11.5.root");

    // // Q^2 = 14.3
    // fcoin[14.3]  = new TFile("coin_replay_production_LH2_14.3_largecoll.root");
    // fsing[14.3]  = new TFile("lh2_hms_singles_q2_14.3.root");
    // fdummy[14.3] = new TFile("dummy_hms_q2_14.3.root");

    std::map<Double_t, TCut> ppidcut, hpidcut, emisscut, pdeltacut, hdeltacut, pslopecut, hslopecut, ztarcut;
    TCut singlescut, coincut, coinWcut, singWcut;

    hpidcut[8.0]  = "H.cer.npeSum>5 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";
    hpidcut[9.5]  = "H.cer.npeSum>5 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";
    hpidcut[11.5] = "H.cer.npeSum>5 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";
    hpidcut[14.3] = "H.cer.npeSum>5 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";

    ppidcut[8.0]  = "P.ngcer.npeSum == 0";
    ppidcut[9.5]  = "P.ngcer.npeSum == 0";
    ppidcut[11.5] = "P.ngcer.npeSum == 0";
    ppidcut[14.3] = "P.ngcer.npeSum == 0";

    emisscut[8.0]  = "P.kin.secondary.emiss<0.03";
    emisscut[9.5]  = "P.kin.secondary.emiss<0.03";
    emisscut[11.5] = "P.kin.secondary.emiss<0.03";
    emisscut[14.3] = "P.kin.secondary.emiss<0.03";

    pdeltacut[8.0]  = "-0.0 < P.gtr.dp && P.gtr.dp < 4.0";
    pdeltacut[9.5]  = "-3.5 < P.gtr.dp && P.gtr.dp < 7.5";
    pdeltacut[11.5] = "-4.0 < P.gtr.dp && P.gtr.dp < 6.0";
    pdeltacut[14.3] = "-0.5 < P.gtr.dp && P.gtr.dp < 3.5";

    hdeltacut[8.0]  = "-4.0 < H.gtr.dp && H.gtr.dp < 5.0";
    hdeltacut[9.5]  = "-5.0 < H.gtr.dp && H.gtr.dp < 6.5";
    hdeltacut[11.5] = "-6.0 < H.gtr.dp && H.gtr.dp < 8.0";
    hdeltacut[14.3] = "-4.0 < H.gtr.dp && H.gtr.dp < 6.0";

    hslopecut[8.0]  = "-0.050 < H.gtr.th && H.gtr.th < 0.050 && -0.020 < H.gtr.ph && H.gtr.ph < 0.025";
    hslopecut[9.5]  = "-0.040 < H.gtr.th && H.gtr.th < 0.050 && -0.020 < H.gtr.ph && H.gtr.ph < 0.020";
    hslopecut[11.5] = "-0.060 < H.gtr.th && H.gtr.th < 0.070 && -0.020 < H.gtr.ph && H.gtr.ph < 0.025";
    hslopecut[14.3] = "-0.060 < H.gtr.th && H.gtr.th < 0.060 && -0.020 < H.gtr.ph && H.gtr.ph < 0.020";

    pslopecut[8.0]  = "-0.020 < P.gtr.th && P.gtr.th < 0.020 && -0.010 < P.gtr.ph && P.gtr.ph < 0.010";
    pslopecut[9.5]  = "-0.045 < P.gtr.th && P.gtr.th < 0.040 && -0.020 < P.gtr.ph && P.gtr.ph < 0.020";
    pslopecut[11.5] = "-0.045 < P.gtr.th && P.gtr.th < 0.040 && -0.020 < P.gtr.ph && P.gtr.ph < 0.020";
    pslopecut[14.3] = "-0.025 < P.gtr.th && P.gtr.th < 0.020 && -0.010 < P.gtr.ph && P.gtr.ph < 0.010";

    ztarcut[8.0]  = "abs(H.react.z)<4";
    ztarcut[9.5]  = "abs(H.react.z)<4";
    ztarcut[11.5] = "abs(H.react.z)<4";
    ztarcut[14.3] = "abs(H.react.z)<4";

    coinWcut = "0.90 < H.kin.primary.W && H.kin.primary.W < 1.00";
    singWcut = "0.90 < H.kin.W         && H.kin.W         < 1.00";

    std::cout << "get trees" << std::endl;
    tcoin  = (TTree*) fcoin[q2]->Get("T");
    tsing  = (TTree*) fsing[q2]->Get("T");
    tdummy = (TTree*) fdummy[q2]->Get("T");

    TCut openCut = "";
    TCut coinCut = hslopecut[q2] && hdeltacut[q2] && pslopecut[q2] && pdeltacut[q2] && coinWcut && hpidcut[q2] && ztarcut[q2];
    TCut hmsCut = hslopecut[q2] && hdeltacut[q2] && coinWcut && hpidcut[q2] && ztarcut[q2];

    tcoin->Draw("P.ngcer.npeSum>>h_coin_open(150,0,15)");
    tcoin->Draw("P.ngcer.npeSum>>h_coin_coincut(150,0,15)", coinCut);
    tcoin->Draw("P.ngcer.npeSum>>h_coin_hmscut(150,0,15)", hmsCut);

    TH1F* h_coin_open = (TH1F*) gDirectory->Get("h_coin_open");
    TH1F* h_coin_coincut = (TH1F*) gDirectory->Get("h_coin_coincut");
    TH1F* h_coin_hmscut = (TH1F*) gDirectory->Get("h_coin_hmscut");

    TString canvasName = Form("c_q2_%.1f", q2);
    TString padName = Form("pad_q2_%.1f", q2);
    TCanvas *c = new TCanvas(canvasName.Data(), canvasName.Data(), 500, 800);
    c->cd();
    TPad *pad = new TPad(padName.Data(), padName.Data(), 0, 0, 1, 1);
    pad->Divide(1,2);
    pad->Draw();

    pad->cd(1);
    h_coin_hmscut->SetTitle("NGC NPE Sum, coincidence data;NPE;Counts");
    h_coin_coincut->SetTitle("NGC NPE Sum, coincidence data;NPE;Counts");
    h_coin_open->SetTitle("NGC NPE Sum, coincidence data;NPE;Counts");
    h_coin_hmscut->SetLineColor(46);
    h_coin_hmscut->SetFillColor(46);
    h_coin_hmscut->SetFillStyle(3345);
    h_coin_coincut->SetLineColor(38);
    h_coin_coincut->SetFillColor(38);
    h_coin_coincut->SetFillStyle(3354);
    h_coin_open->SetLineColor(30);
    h_coin_open->SetFillColor(30);
    h_coin_open->SetFillStyle(3002);
    h_coin_open->Draw();
    h_coin_hmscut->Draw("SAME");
    h_coin_coincut->Draw("SAME");

    // Estimate pi+:proton ratio
    std::map<TString, Double_t> pions;
    std::map<TString, Double_t> protons;

    pions["open"]   = h_coin_open->Integral(h_coin_open->FindBin(0.1),h_coin_open->GetNbinsX()+1);
    protons["open"] = h_coin_open->Integral(h_coin_open->FindBin(0.0),h_coin_open->FindBin(0.1));

    pions["hmscut"]   = h_coin_hmscut->Integral(h_coin_hmscut->FindBin(0.1),h_coin_hmscut->GetNbinsX()+1);
    protons["hmscut"] = h_coin_hmscut->Integral(h_coin_hmscut->FindBin(0.0),h_coin_hmscut->FindBin(0.1));

    pions["coincut"]   = h_coin_coincut->Integral(h_coin_coincut->FindBin(0.1),h_coin_coincut->GetNbinsX()+1);
    protons["coincut"] = h_coin_coincut->Integral(h_coin_coincut->FindBin(0.0),h_coin_coincut->FindBin(0.1));

    std::cout << "cut, pions, protons, protons:pions" << std::endl;
    for (TString cut : {"open", "hmscut", "coincut"} ) {
        std::cout << Form("%s, %.0f, %.0f, %.2f", cut.Data(), pions[cut], protons[cut], protons[cut]/pions[cut]) << std::endl;
    }

    c->ToggleEventStatus();

    // Write
    TFile fWrite(Form("q2_%.1f_ngc_npe.root", q2), "recreate");

    h_coin_open->Write();
    h_coin_coincut->Write();
    h_coin_hmscut->Write();

    c->Write();

    fWrite.Close();
}
