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

    std::map<Double_t, TFile*> fcoin, fsing, fdummy;
    TTree *tcoin, *tsing, *tdummy;
    std::vector<Double_t> q2s = {9.5, 11.5};

    // Q^2 = 8
    fcoin[8.0]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/coin_replay_production_LH2_8_smallcoll.root");
    fsing[8.0]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/lh2_hms_singles_q2_8.root");
    fdummy[8.0] = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/dummy_hms_q2_8.root");

    // Q^2 = 9.5
    fcoin[9.5]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/coin_replay_production_LH2_9.5_smallcoll.root");
    fsing[9.5]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/lh2_hms_singles_q2_9.5.root");
    fdummy[9.5] = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/dummy_hms_q2_9.5.root");

    // Q^2 = 11.5
    fcoin[11.5]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/coin_replay_production_LH2_11.5_largecoll.root");
    fsing[11.5]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/lh2_hms_singles_q2_11.5.root");
    fdummy[11.5] = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/dummy_hms_q2_11.5.root");

    // Q^2 = 14.3
    fcoin[14.3]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/coin_replay_production_LH2_14.3_largecoll.root");
    fsing[14.3]  = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/lh2_hms_singles_q2_14.3.root");
    fdummy[14.3] = new TFile("/Volumes/ssd750/ct/ct_scripts/analysis/proton_absorption/dummy_hms_q2_14.3.root");

    std::map<Double_t, TCut> ppidcut, hpidcut, emisscut, pdeltacut, hdeltacut, pslopecut, hslopecut, ztarcut;
    std::map<Double_t, TCutG*> hfpcutg;
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
    hslopecut[9.5]  = "-0.035 < H.gtr.th && H.gtr.th < 0.040 && -0.015 < H.gtr.ph && H.gtr.ph < 0.015";
    hslopecut[11.5] = "-0.050 < H.gtr.th && H.gtr.th < 0.060 && -0.015 < H.gtr.ph && H.gtr.ph < 0.020";
    hslopecut[14.3] = "-0.060 < H.gtr.th && H.gtr.th < 0.060 && -0.020 < H.gtr.ph && H.gtr.ph < 0.020";

    pslopecut[8.0]  = "-0.020 < P.gtr.th && P.gtr.th < 0.020 && -0.010 < P.gtr.ph && P.gtr.ph < 0.010";
    pslopecut[9.5]  = "-0.045 < P.gtr.th && P.gtr.th < 0.040 && -0.020 < P.gtr.ph && P.gtr.ph < 0.020";
    pslopecut[11.5] = "-0.045 < P.gtr.th && P.gtr.th < 0.040 && -0.020 < P.gtr.ph && P.gtr.ph < 0.020";
    pslopecut[14.3] = "-0.025 < P.gtr.th && P.gtr.th < 0.020 && -0.010 < P.gtr.ph && P.gtr.ph < 0.010";

    ztarcut[8.0]  = "abs(H.react.z)<3";
    ztarcut[9.5]  = "abs(H.react.z)<3";
    ztarcut[11.5] = "abs(H.react.z)<3";
    ztarcut[14.3] = "abs(H.react.z)<3";

    hfpcutg[8.0] = new TCutG("hfpcutg_8", 15);
    hfpcutg[8.0]->SetVarX("H.dc.y[0]");
    hfpcutg[8.0]->SetVarY("H.dc.x[0]");
    hfpcutg[8.0]->SetPoint(0, -4.44874, -1.43035);
    hfpcutg[8.0]->SetPoint(1, -4.0036, 5.20531);
    hfpcutg[8.0]->SetPoint(2, 1.98561, 13.1681);
    hfpcutg[8.0]->SetPoint(3, 9.47212, 18.2725);
    hfpcutg[8.0]->SetPoint(4, 10.7671, 16.9453);
    hfpcutg[8.0]->SetPoint(5, 10.8885, 12.7598);
    hfpcutg[8.0]->SetPoint(6, 5.95144, 5.00113);
    hfpcutg[8.0]->SetPoint(7, 6.03237, -1.32827);
    hfpcutg[8.0]->SetPoint(8, 6.96313, -4.39088);
    hfpcutg[8.0]->SetPoint(9, 6.03237, -11.2307);
    hfpcutg[8.0]->SetPoint(10, 3.80665, -14.0892);
    hfpcutg[8.0]->SetPoint(11, -1.81835, -11.9453);
    hfpcutg[8.0]->SetPoint(12, -4.32734, -6.02427);
    hfpcutg[8.0]->SetPoint(13, -4.44874, -1.22618);
    hfpcutg[8.0]->SetPoint(14, -4.44874, -1.43035);

    hfpcutg[9.5] = new TCutG("hfpcutg_95", 11);
    hfpcutg[9.5]->SetVarX("H.dc.y[0]");
    hfpcutg[9.5]->SetVarY("H.dc.x[0]");

    hfpcutg[9.5]->SetPoint(0, 2.99893, 14.7566);
    hfpcutg[9.5]->SetPoint(1, -1.59456, 7.29605);
    hfpcutg[9.5]->SetPoint(2, -2.17479, 1.96711);
    hfpcutg[9.5]->SetPoint(3, -2.07808, -2.65132);
    hfpcutg[9.5]->SetPoint(4, 2.03188, -3.24342);
    hfpcutg[9.5]->SetPoint(5, 1.88682, 3.15132);
    hfpcutg[9.5]->SetPoint(6, 4.11103, 9.19079);
    hfpcutg[9.5]->SetPoint(7, 4.4495, 15.1118);
    hfpcutg[9.5]->SetPoint(8, 2.99893, 14.7566);
    hfpcutg[9.5]->SetPoint(9, 2.99893, 14.7566);

    hfpcutg[11.5] = new TCutG("hfpcutg_115", 11);
    hfpcutg[11.5]->SetVarX("H.dc.y[0]");
    hfpcutg[11.5]->SetVarY("H.dc.x[0]");

    hfpcutg[11.5]->SetPoint(0, 2.99893, 14.7566);
    hfpcutg[11.5]->SetPoint(1, -1.59456, 7.29605);
    hfpcutg[11.5]->SetPoint(2, -2.17479, 1.96711);
    hfpcutg[11.5]->SetPoint(3, -2.07808, -2.65132);
    hfpcutg[11.5]->SetPoint(4, 2.03188, -3.24342);
    hfpcutg[11.5]->SetPoint(5, 1.88682, 3.15132);
    hfpcutg[11.5]->SetPoint(6, 4.11103, 9.19079);
    hfpcutg[11.5]->SetPoint(7, 4.4495, 15.1118);
    hfpcutg[11.5]->SetPoint(8, 2.99893, 14.7566);
    hfpcutg[11.5]->SetPoint(9, 2.99893, 14.7566);

    hfpcutg[14.3] = new TCutG("hfpcutg_143", 10);
    hfpcutg[14.3]->SetVarX("H.dc.y[0]");
    hfpcutg[14.3]->SetVarY("H.dc.x[0]");
    hfpcutg[14.3]->SetPoint(0, -4.08453, -3.26792);
    hfpcutg[14.3]->SetPoint(1, -3.88219, 4.28652);
    hfpcutg[14.3]->SetPoint(2, 4.85881, 20.6205);
    hfpcutg[14.3]->SetPoint(3, 10.4029, 17.4558);
    hfpcutg[14.3]->SetPoint(4, 4.29227, 2.04061);
    hfpcutg[14.3]->SetPoint(5, 9.63399, -13.7829);
    hfpcutg[14.3]->SetPoint(6, 5.06115, -17.7643);
    hfpcutg[14.3]->SetPoint(7, -2.10162, -13.5787);
    hfpcutg[14.3]->SetPoint(8, -4.125, -3.16583);
    hfpcutg[14.3]->SetPoint(9, -4.08453, -3.26792);

    coinWcut = "0.90 < H.kin.primary.W && H.kin.primary.W < 1.00";
    singWcut = "0.90 < H.kin.W         && H.kin.W         < 1.00";

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

    singlescut = hpidcut[q2] && hdeltacut[q2] && hslopecut[q2] && ztarcut[q2] && hfpcutg[q2]->GetName();
    coincut    = hpidcut[q2] && hdeltacut[q2] && hslopecut[q2] && ztarcut[q2] && hfpcutg[q2]->GetName() && pdeltacut[q2] && pslopecut[q2] && ppidcut[q2] && emisscut[q2];

    // W
    // Wcoin
    tcoin->Draw("H.kin.primary.W>>h_coinW_open(120,0.8,1.2)");
    tcoin->Draw("H.kin.primary.W>>h_coinW_singlescut(120,0.8,1.2)", singlescut);
    tcoin->Draw("H.kin.primary.W>>h_coinW_coincut(120,0.8,1.2)", coincut);
    tcoin->Draw("H.kin.primary.W>>h_coinW_inpeak(120,0.8,1.2)", coincut && coinWcut);

    // Wcoin_wide
    tcoin->Draw("H.kin.primary.W>>h_coinW_open_wide(100,0.0,2.0)");
    tcoin->Draw("H.kin.primary.W>>h_coinW_singlescut_wide(100,0.0,2.0)", singlescut);
    tcoin->Draw("H.kin.primary.W>>h_coinW_coincut_wide(100,0.0,2.0)", coincut);
    tcoin->Draw("H.kin.primary.W>>h_coinW_inpeak_wide(100,0.0,2.0)", coincut && coinWcut);

    // Wsing
    tsing->Draw("H.kin.W>>h_singW_open(80,0.8,1.2)");
    tsing->Draw("H.kin.W>>h_singW_cut(80,0.8,1.2)", singlescut);
    tsing->Draw("H.kin.W>>h_singW_inpeak(80,0.8,1.2)", singlescut && singWcut);

    // Wsingwide
    tsing->Draw("H.kin.W>>h_singW_open_wide(80,0.0,2.0)");
    tsing->Draw("H.kin.W>>h_singW_cut_wide(80,0.0,2.0)", singlescut);
    tsing->Draw("H.kin.W>>h_singW_inpeak_wide(80,0.0,2.0)", singlescut && singWcut);

    // SHMS NGC NPE distribution
    tcoin->Draw("P.ngcer.npeSum>>h_ngc_open(150,0,15)", "");
    tcoin->Draw("P.ngcer.npeSum>>h_ngc_coincut(150,0,15)", coincut);
    tcoin->Draw("P.ngcer.npeSum>>h_ngc_singlescut(150,0,15)", singlescut);

    // HMS focal plane coordinates
    tcoin->Draw("H.dc.x[0]:H.dc.y[0]>>h_focalplane(27,-12,15,45,-20,25)", hpidcut[q2] && hdeltacut[q2] && hslopecut[q2] && ztarcut[q2] && coinWcut);

    // These are used for calculating yields in the absorption script
    tcoin->Draw("H.kin.primary.W>>h_coin_W_count(160,0.8,1.2)", hpidcut[q2] && hdeltacut[q2] && hslopecut[q2] && coinWcut && ztarcut[q2] && hfpcutg[q2]->GetName());
    tsing->Draw("H.kin.W>>h_sing_W_count(80,0.8,1.2)",          hpidcut[q2] && hdeltacut[q2] && hslopecut[q2] && singWcut && ztarcut[q2] && hfpcutg[q2]->GetName());

    // Get histos from memory
    TH1F* h_emiss                 = (TH1F*) gDirectory->Get("h_emiss");
    TH1F* h_delta                 = (TH1F*) gDirectory->Get("h_delta");
    TH1F* h_hslope                = (TH1F*) gDirectory->Get("h_hslope");
    TH1F* h_pslope                = (TH1F*) gDirectory->Get("h_pslope");
    TH1F* h_react_cut             = (TH1F*) gDirectory->Get("h_react_cut");
    TH1F* h_react_open            = (TH1F*) gDirectory->Get("h_react_open");
    TH1F* h_coinW_singlescut      = (TH1F*) gDirectory->Get("h_coinW_singlescut");
    TH1F* h_coinW_coincut         = (TH1F*) gDirectory->Get("h_coinW_coincut");
    TH1F* h_coinW_inpeak          = (TH1F*) gDirectory->Get("h_coinW_inpeak");
    TH1F* h_coinW_open            = (TH1F*) gDirectory->Get("h_coinW_open");
    TH1F* h_coinW_singlescut_wide = (TH1F*) gDirectory->Get("h_coinW_singlescut_wide");
    TH1F* h_coinW_coincut_wide    = (TH1F*) gDirectory->Get("h_coinW_coincut_wide");
    TH1F* h_coinW_inpeak_wide     = (TH1F*) gDirectory->Get("h_coinW_inpeak_wide");
    TH1F* h_coinW_open_wide       = (TH1F*) gDirectory->Get("h_coinW_open_wide");
    TH1F* h_singW_cut             = (TH1F*) gDirectory->Get("h_singW_cut");
    TH1F* h_singW_inpeak          = (TH1F*) gDirectory->Get("h_singW_inpeak");
    TH1F* h_singW_open            = (TH1F*) gDirectory->Get("h_singW_open");
    TH1F* h_singW_cut_wide        = (TH1F*) gDirectory->Get("h_singW_cut_wide");
    TH1F* h_singW_inpeak_wide     = (TH1F*) gDirectory->Get("h_singW_inpeak_wide");
    TH1F* h_singW_open_wide       = (TH1F*) gDirectory->Get("h_singW_open_wide");
    TH1F* h_ngc_open              = (TH1F*) gDirectory->Get("h_ngc_open");
    TH1F* h_ngc_coincut           = (TH1F*) gDirectory->Get("h_ngc_coincut");
    TH1F* h_ngc_singlescut        = (TH1F*) gDirectory->Get("h_ngc_singlescut");
    TH1F* h_focalplane            = (TH1F*) gDirectory->Get("h_focalplane");
    TH1F* h_sing_W_count          = (TH1F*) gDirectory->Get("h_sing_W_count");
    TH1F* h_coin_W_count          = (TH1F*) gDirectory->Get("h_coin_W_count");


    // Make things pretty
    h_singW_open->SetMinimum(0);
    h_singW_open_wide->SetMinimum(0);

    std::cout << "draw histos on canvas" << std::endl;
    TString canvasName = Form("c_q2_%.1f", q2);
    TString padName = Form("pad_q2_%.1f", q2);
    TCanvas *c = new TCanvas(canvasName.Data(), canvasName.Data(), 1600, 600);
    c->cd();
    TPad *pad = new TPad(padName.Data(), padName.Data(), 0, 0, 1, 1);
    pad->Divide(5,2);
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
    h_react_open->SetLineColor(30);
    h_react_open->SetFillColor(30);
    h_react_open->SetFillStyle(3354);
    h_react_open->Draw();
    h_react_cut->Draw("SAME");

    pad->cd(6);
    h_focalplane->SetTitle("HMS focal plane coordinates [cm];y_{fp};x_{fp}");
    h_focalplane->Draw("colz");
    pad->GetPad(7)->SetGrid();

    pad->cd(7);
    h_ngc_singlescut->SetTitle("SHMS NGC NPE Sum;NPE Sum;Counts");
    h_ngc_coincut->SetTitle("SHMS NGC NPE Sum;NPE Sum;Counts");
    h_ngc_open->SetTitle("SHMS NGC NPE Sum;NPE Sum;Counts");
    h_ngc_singlescut->SetLineColor(46);
    h_ngc_singlescut->SetFillColor(46);
    h_ngc_singlescut->SetFillStyle(3345);
    h_ngc_coincut->SetLineColor(38);
    h_ngc_coincut->SetFillColor(38);
    h_ngc_coincut->SetFillStyle(3354);
    h_ngc_open->SetLineColor(30);
    h_ngc_open->SetFillColor(30);
    h_ngc_open->SetFillStyle(3002);
    h_ngc_open->Draw();
    h_ngc_singlescut->Draw("SAME");
    h_ngc_coincut->Draw("SAME");
    gPad->SetLogy();

    pad->cd(8);
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

    pad->cd(9);
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

    h_coinW_singlescut_wide->SetLineColor(46);
    h_coinW_singlescut_wide->SetFillColor(46);
    h_coinW_singlescut_wide->SetFillStyle(3345);
    h_coinW_coincut_wide->SetLineColor(38);
    h_coinW_coincut_wide->SetFillColor(38);
    h_coinW_coincut_wide->SetFillStyle(3354);
    h_coinW_inpeak_wide->SetLineColor(1);
    h_coinW_inpeak_wide->SetFillColor(1);
    h_coinW_inpeak_wide->SetFillStyle(3345);
    h_coinW_open_wide->SetLineColor(30);
    h_coinW_open_wide->SetFillColor(30);
    h_coinW_open_wide->SetFillStyle(3002);

    pad->cd(10);
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
    h_ngc_open->Write();
    h_ngc_singlescut->Write();
    h_ngc_coincut->Write();
    h_focalplane->Write();
    h_coinW_singlescut->Write();
    h_coinW_coincut->Write();
    h_coinW_inpeak->Write();
    h_coinW_open->Write();
    h_coinW_singlescut_wide->Write();
    h_coinW_coincut_wide->Write();
    h_coinW_inpeak_wide->Write();
    h_coinW_open_wide->Write();
    h_singW_cut->Write();
    h_singW_inpeak->Write();
    h_singW_open->Write();
    h_singW_cut_wide->Write();
    h_singW_inpeak_wide->Write();
    h_singW_open_wide->Write();
    h_sing_W_count->Write();
    h_coin_W_count->Write();

    c->Write();

    fWrite.Close();
}
