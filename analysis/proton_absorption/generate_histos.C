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
void generate_histos(Double_t q2) {
    // Double_t q2=8.0;
    // Double_t q2=9.5;
    // Double_t q2=11.5;
    // Double_t q2=14.3;

    std::map<Double_t, TFile*> fcoin, fsing, fdummy;
    TTree *tcoin, *tsing, *tdummy;
    std::vector<Double_t> q2s = {8.0, 9.5, 11.5, 14.3};

    // Q^2 = 8
    fcoin[8.0]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_8_smallcoll.root");
    fsing[8.0]  = new TFile("/Volumes/ssd750/ct/pass3/hms_coin_replay_production_2049_500000.root");
    fdummy[8.0] = new TFile("/Volumes/ssd750/ct/pass3/dummy_q2_8.root");

    // Q^2 = 9.5
    fcoin[9.5]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_9.5_smallcoll.root");
    fsing[9.5]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_hms_singles_q2_9.5.root");
    fdummy[9.5] = new TFile("/Volumes/ssd750/ct/pass3/dummy_q2_9.5.root");

    // Q^2 = 11.5
    fcoin[11.5]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_11.5_largecoll.root");
    fsing[11.5]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_hms_singles_q2_11.5.root");
    fdummy[11.5] = new TFile("/Volumes/ssd750/ct/pass3/dummy_q2_11.5.root");

    // Q^2 = 14.3
    fcoin[14.3]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_14.3_largecoll.root");
    fsing[14.3]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_hms_singles_q2_14.3.root");
    fdummy[14.3] = new TFile("/Volumes/ssd750/ct/pass3/dummy_q2_14.3.root");

    std::map<Double_t, TCut> hpidcut, emisscut, pdeltacut, hdeltacut, pslopecut, hslopecut, ztarcut;
    TCut singlescut, coincut, coinWcut, singWcut;

    hpidcut[8.0]  = "H.cer.npeSum>0 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";
    hpidcut[9.5]  = "H.cer.npeSum>0 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";
    hpidcut[11.5] = "H.cer.npeSum>0 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";
    hpidcut[14.3] = "H.cer.npeSum>0 && 0.9 < H.cal.etottracknorm && H.cal.etottracknorm < 1.1";

    emisscut[8.0]  = "P.kin.secondary.emiss<0.03";
    emisscut[9.5]  = "P.kin.secondary.emiss<0.03";
    emisscut[11.5] = "P.kin.secondary.emiss<0.03";
    emisscut[14.3] = "P.kin.secondary.emiss<0.03";

    pdeltacut[8.0]  = "-0.3 < P.gtr.dp && P.gtr.dp < 4.5";
    pdeltacut[9.5]  = "-3.5 < P.gtr.dp && P.gtr.dp < 7.5";
    pdeltacut[11.5] = "-3.4 < P.gtr.dp && P.gtr.dp < 6.0";
    pdeltacut[14.3] = "-0.7 < P.gtr.dp && P.gtr.dp < 4.0";

    hdeltacut[8.0]  = "-5.0 < H.gtr.dp && H.gtr.dp < 6.3";
    hdeltacut[9.5]  = "-5.5 < H.gtr.dp && H.gtr.dp < 6.5";
    hdeltacut[11.5] = "-6.5 < H.gtr.dp && H.gtr.dp < 8.0";
    hdeltacut[14.3] = "-6.0 < H.gtr.dp && H.gtr.dp < 7.0";

    hslopecut[8.0]  = "-0.056 < H.gtr.th && H.gtr.th < 0.056 && -0.030 < H.gtr.ph && H.gtr.ph < 0.035";
    hslopecut[9.5]  = "-0.045 < H.gtr.th && H.gtr.th < 0.047 && -0.023 < H.gtr.ph && H.gtr.ph < 0.022";
    hslopecut[11.5] = "-0.055 < H.gtr.th && H.gtr.th < 0.055 && -0.030 < H.gtr.ph && H.gtr.ph < 0.035";
    hslopecut[14.3] = "-0.065 < H.gtr.th && H.gtr.th < 0.070 && -0.030 < H.gtr.ph && H.gtr.ph < 0.030";

    pslopecut[8.0]  = "-0.025 < P.gtr.th && P.gtr.th < 0.022 && -0.012 < P.gtr.ph && P.gtr.ph < 0.013";
    pslopecut[9.5]  = "-0.046 < P.gtr.th && P.gtr.th < 0.040 && -0.020 < P.gtr.ph && P.gtr.ph < 0.019";
    pslopecut[11.5] = "-0.025 < P.gtr.th && P.gtr.th < 0.024 && -0.012 < P.gtr.ph && P.gtr.ph < 0.013";
    pslopecut[14.3] = "-0.025 < P.gtr.th && P.gtr.th < 0.023 && -0.010 < P.gtr.ph && P.gtr.ph < 0.013";

    ztarcut[8.0]  = "abs(H.react.z)<3";
    ztarcut[9.5]  = "abs(H.react.z)<3";
    ztarcut[11.5] = "abs(H.react.z)<3";
    ztarcut[14.3] = "abs(H.react.z)<3";

    coinWcut = "0.9<H.kin.primary.W && H.kin.primary.W<1";
    singWcut = "0.9<H.kin.W && H.kin.W<1";

    TFile fWrite(Form("q2_%.1f_canvas.root", q2), "recreate");


    std::cout << "get trees" << std::endl;
    tcoin  = (TTree*) fcoin[q2]->Get("T");
    tsing  = (TTree*) fsing[q2]->Get("T");
    tdummy = (TTree*) fdummy[q2]->Get("T");

    std::cout << "generate histos" << std::endl;
    // Missing energy
    tcoin->Draw("P.kin.secondary.emiss>>h_emiss(120,-0.02,0.1)");

    // Delta
    tcoin->Draw("P.gtr.dp:H.gtr.dp>>h_delta(200,-10,10,200,-10,10)", emisscut[q2],"colz");

    // xptar and yptar
    tcoin->Draw("P.gtr.th:P.gtr.ph>>h_pslope(70,-.035,.035,120,-.060,.060)", emisscut[q2]&&hdeltacut[q2]&&pdeltacut[q2],"colz");
    tcoin->Draw("H.gtr.th:H.gtr.ph>>h_hslope(100,-.1,.1,100,-.1,.1)", emisscut[q2]&&hdeltacut[q2]&&pdeltacut[q2],"colz");

    // ztar
    tdummy->Draw("H.react.z>>h_react_open(100,-10,10)");
    tdummy->Draw("H.react.z>>h_react_cut(100,-10,10)", hdeltacut[q2]&&hslopecut[q2]);

    singlescut = hpidcut[q2] && ztarcut[q2] && hdeltacut[q2] && hslopecut[q2];
    coincut    = hpidcut[q2] && ztarcut[q2] && hdeltacut[q2] && hslopecut[q2] && pdeltacut[q2] && pslopecut[q2];

    // W
    tcoin->Draw("H.kin.primary.W>>h_coinW_open(160,0.8,1.2)");
    tcoin->Draw("H.kin.primary.W>>h_coinW_singlescut(160,0.8,1.2)", singlescut);
    tcoin->Draw("H.kin.primary.W>>h_coinW_coincut(160,0.8,1.2)", coincut);
    tcoin->Draw("H.kin.primary.W>>h_coinW_inpeak(160,0.8,1.2)", singlescut && coinWcut);

    tsing->Draw("H.kin.W>>h_singW_open(80,0.8,1.2)");
    tsing->Draw("H.kin.W>>h_singW_cut(80,0.8,1.2)", singlescut);
    tsing->Draw("H.kin.W>>h_singW_inpeak(80,0.8,1.2)", singlescut && singWcut);
    tsing->Draw("H.kin.W>>h_singW_open_wide(80,0.0,2.0)");
    tsing->Draw("H.kin.W>>h_singW_cut_wide(80,0.0,2.0)", singlescut);
    tsing->Draw("H.kin.W>>h_singW_inpeak_wide(80,0.0,2.0)", singlescut && singWcut);

    // Get histos from memory
    TH1F* h_emiss             = (TH1F*) gDirectory->Get("h_emiss");
    TH1F* h_delta             = (TH1F*) gDirectory->Get("h_delta");
    TH1F* h_hslope            = (TH1F*) gDirectory->Get("h_hslope");
    TH1F* h_pslope            = (TH1F*) gDirectory->Get("h_pslope");
    TH1F* h_react_cut         = (TH1F*) gDirectory->Get("h_react_cut");
    TH1F* h_react_open        = (TH1F*) gDirectory->Get("h_react_open");
    TH1F* h_coinW_singlescut  = (TH1F*) gDirectory->Get("h_coinW_singlescut");
    TH1F* h_coinW_coincut     = (TH1F*) gDirectory->Get("h_coinW_coincut");
    TH1F* h_coinW_inpeak      = (TH1F*) gDirectory->Get("h_coinW_inpeak");
    TH1F* h_coinW_open        = (TH1F*) gDirectory->Get("h_coinW_open");
    TH1F* h_singW_cut         = (TH1F*) gDirectory->Get("h_singW_cut");
    TH1F* h_singW_inpeak      = (TH1F*) gDirectory->Get("h_singW_inpeak");
    TH1F* h_singW_open        = (TH1F*) gDirectory->Get("h_singW_open");
    TH1F* h_singW_cut_wide    = (TH1F*) gDirectory->Get("h_singW_cut_wide");
    TH1F* h_singW_inpeak_wide = (TH1F*) gDirectory->Get("h_singW_inpeak_wide");
    TH1F* h_singW_open_wide   = (TH1F*) gDirectory->Get("h_singW_open_wide");


    // Make things pretty
    h_singW_open->SetMinimum(0);
    h_singW_open_wide->SetMinimum(0);

    std::cout << "draw histos on canvas" << std::endl;
    TString canvasName = Form("c_q2_%.1f", q2);
    TString padName = Form("pad_q2_%.1f", q2);
    TCanvas *c = new TCanvas(canvasName.Data(), canvasName.Data(), 1200, 600);
    c->cd();
    TPad *pad = new TPad(padName.Data(), padName.Data(), 0, 0, 1, 1);
    pad->Divide(4,2);
    pad->Draw();

    pad->cd(1);
    h_emiss->SetTitle("E_miss [GeV];E_miss;Count");
    h_emiss->Draw();

    pad->cd(2);
    h_delta->SetTitle("Delta [%];HMS Delta;SHMS Delta");
    h_delta->Draw("colz");
    pad->GetPad(2)->SetGrid();

    pad->cd(3);
    h_hslope->SetTitle("HMS target variables [rad];yptar;xptar");
    h_hslope->Draw("colz");
    pad->GetPad(3)->SetGrid();

    pad->cd(4);
    h_pslope->SetTitle("SHMS target variables [rad];yptar;xptar");
    h_pslope->Draw("colz");
    pad->GetPad(4)->SetGrid();

    pad->cd(5);
    h_react_cut->SetTitle("ztar [cm];ztar;Count");
    h_react_open->SetTitle("ztar [cm];ztar;Count");
    h_react_cut->SetLineColor(46);
    h_react_cut->SetFillColor(46);
    h_react_cut->SetFillStyle(3345);
    h_react_open->SetLineColor(38);
    h_react_open->SetFillColor(38);
    h_react_open->SetFillStyle(3354);
    h_react_open->Draw();
    h_react_cut->Draw("SAME");

    pad->cd(6);
    h_coinW_singlescut->SetTitle("W [GeV];W;Counts");
    h_coinW_coincut->SetTitle("W [GeV];W;Counts");
    h_coinW_inpeak->SetTitle("W [GeV];W;Counts");
    h_coinW_open->SetTitle("W [GeV];W;Counts");
    h_coinW_singlescut->SetLineColor(46);
    h_coinW_singlescut->SetFillColor(46);
    h_coinW_singlescut->SetFillStyle(3345);
    h_coinW_coincut->SetLineColor(38);
    h_coinW_coincut->SetFillColor(38);
    h_coinW_coincut->SetFillStyle(3354);
    h_coinW_inpeak->SetLineColor(1);
    h_coinW_inpeak->SetFillColor(1);
    h_coinW_inpeak->SetFillStyle(3345);
    h_coinW_open->SetLineColor(30);
    h_coinW_open->SetFillColor(30);
    h_coinW_open->SetFillStyle(3002);
    h_coinW_open->Draw();
    h_coinW_singlescut->Draw("SAME");
    h_coinW_coincut->Draw("SAME");

    pad->cd(7);
    h_singW_cut->SetTitle("W [GeV];W;Counts");
    h_singW_inpeak->SetTitle("W [GeV];W;Counts");
    h_singW_open->SetTitle("W [GeV];W;Counts");
    h_singW_cut->SetLineColor(46);
    h_singW_cut->SetFillColor(46);
    h_singW_cut->SetFillStyle(3345);
    h_singW_inpeak->SetLineColor(1);
    h_singW_inpeak->SetFillColor(1);
    h_singW_inpeak->SetFillStyle(3354);
    h_singW_open->SetLineColor(30);
    h_singW_open->SetFillColor(30);
    h_singW_open->SetFillStyle(3002);
    h_singW_open->Draw();
    h_singW_cut->Draw("SAME");
    h_singW_inpeak->Draw("SAME");

    pad->cd(8);
    h_singW_cut_wide->SetTitle("W [GeV];W;Counts");
    h_singW_inpeak_wide->SetTitle("W [GeV];W;Counts");
    h_singW_open_wide->SetTitle("W [GeV];W;Counts");
    h_singW_cut_wide->SetLineColor(46);
    h_singW_cut_wide->SetFillColor(46);
    h_singW_cut_wide->SetFillStyle(3345);
    h_singW_inpeak_wide->SetLineColor(1);
    h_singW_inpeak_wide->SetFillColor(1);
    h_singW_inpeak_wide->SetFillStyle(3001);
    h_singW_open_wide->SetLineColor(30);
    h_singW_open_wide->SetFillColor(30);
    h_singW_open_wide->SetFillStyle(3354);
    h_singW_open_wide->Draw();
    h_singW_cut_wide->Draw("SAME");
    h_singW_inpeak_wide->Draw("SAME");

    c->ToggleEventStatus();

    std::cout << "print" << std::endl;
    c->Print(Form("q2_%.1f.pdf", q2));

    std::cout << "write" << std::endl;

    h_emiss->Write();
    h_delta->Write();
    h_hslope->Write();
    h_pslope->Write();
    h_react_cut->Write();
    h_react_open->Write();
    h_coinW_singlescut->Write();
    h_coinW_coincut->Write();
    h_coinW_inpeak->Write();
    h_coinW_open->Write();
    h_singW_cut->Write();
    h_singW_inpeak->Write();
    h_singW_open->Write();
    h_singW_cut_wide->Write();
    h_singW_inpeak_wide->Write();
    h_singW_open_wide->Write();

    c->Write();

    fWrite.Close();
}
