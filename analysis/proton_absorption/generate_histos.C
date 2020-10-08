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

    std::map<Double_t, TCut> ppidcut, hpidcut, emisscut, pdeltacut, hdeltacut,  ztarcut, ytarcut;
    std::map<Double_t, TCutG*> pslopecut, hslopecut, hfpcutg;
    TCut singlescut, coincut, coinWcut, singWcut;

    hpidcut[8.0]  = "H.cer.npeSum>1 && 0.90 < H.cal.etottracknorm && H.cal.etottracknorm < 1.10";
    hpidcut[9.5]  = "H.cer.npeSum>1 && 0.90 < H.cal.etottracknorm && H.cal.etottracknorm < 1.10";
    hpidcut[11.5] = "H.cer.npeSum>1 && 0.90 < H.cal.etottracknorm && H.cal.etottracknorm < 1.10";
    hpidcut[14.3] = "H.cer.npeSum>1 && 0.90 < H.cal.etottracknorm && H.cal.etottracknorm < 1.10";

    ppidcut[8.0]  = "P.ngcer.npeSum == 0";
    ppidcut[9.5]  = "P.ngcer.npeSum == 0";
    ppidcut[11.5] = "P.ngcer.npeSum == 0";
    ppidcut[14.3] = "P.ngcer.npeSum == 0";

    emisscut[8.0]  = "P.kin.secondary.emiss<0.04";
    emisscut[9.5]  = "P.kin.secondary.emiss<0.04";
    emisscut[11.5] = "P.kin.secondary.emiss<0.04";
    emisscut[14.3] = "P.kin.secondary.emiss<0.04";

    pdeltacut[8.0]  = "-0.0 < P.gtr.dp && P.gtr.dp < 4.0";
    pdeltacut[9.5]  = "-3.5 < P.gtr.dp && P.gtr.dp < 7.5";
    pdeltacut[11.5] = "-4.0 < P.gtr.dp && P.gtr.dp < 6.0";
    pdeltacut[14.3] = "-0.5 < P.gtr.dp && P.gtr.dp < 3.5";

    hdeltacut[8.0]  = "-4.0 < H.gtr.dp && H.gtr.dp < 5.0";
    hdeltacut[9.5]  = "-5.0 < H.gtr.dp && H.gtr.dp < 6.5";
    hdeltacut[11.5] = "-6.0 < H.gtr.dp && H.gtr.dp < 8.0";
    hdeltacut[14.3] = "-4.0 < H.gtr.dp && H.gtr.dp < 6.0";

    // hslopecut[8.0]  = "-0.050 < H.gtr.th && H.gtr.th < 0.050 && -0.020 < H.gtr.ph && H.gtr.ph < 0.025";
    // hslopecut[9.5]  = "-0.035 < H.gtr.th && H.gtr.th < 0.040 && -0.015 < H.gtr.ph && H.gtr.ph < 0.015";
    // hslopecut[11.5] = "-0.050 < H.gtr.th && H.gtr.th < 0.060 && -0.015 < H.gtr.ph && H.gtr.ph < 0.020";
    // hslopecut[14.3] = "-0.060 < H.gtr.th && H.gtr.th < 0.060 && -0.020 < H.gtr.ph && H.gtr.ph < 0.020";

    hslopecut[11.5] = new TCutG("hslopecut_115", 11);
    hslopecut[11.5]->SetVarX("H.gtr.ph");
    hslopecut[11.5]->SetVarY("H.gtr.th");

    hslopecut[11.5]->SetPoint(0,-0.0266854,-0.0496894);
    hslopecut[11.5]->SetPoint(1,-0.0290262,0.0139752);
    hslopecut[11.5]->SetPoint(2,-0.0154494,0.0690994);
    hslopecut[11.5]->SetPoint(3,0.0135768,0.0737578);
    hslopecut[11.5]->SetPoint(4,0.0346442,0.0434783);
    hslopecut[11.5]->SetPoint(5,0.0318352,-0.0263975);
    hslopecut[11.5]->SetPoint(6,0.0224719,-0.0659938);
    hslopecut[11.5]->SetPoint(7,-0.0145131,-0.0636646);
    hslopecut[11.5]->SetPoint(8,-0.0262172,-0.0504658);
    hslopecut[11.5]->SetPoint(9,-0.0262172,-0.0504658);
    hslopecut[11.5]->SetPoint(10,-0.0266854,-0.0496894);

    // pslopecut[8.0]  = "-0.020 < P.gtr.th && P.gtr.th < 0.020 && -0.010 < P.gtr.ph && P.gtr.ph < 0.010";
    // pslopecut[9.5]  = "-0.045 < P.gtr.th && P.gtr.th < 0.040 && -0.020 < P.gtr.ph && P.gtr.ph < 0.020";
    // pslopecut[11.5] = "-0.045 < P.gtr.th && P.gtr.th < 0.040 && -0.020 < P.gtr.ph && P.gtr.ph < 0.020";
    // pslopecut[14.3] = "-0.025 < P.gtr.th && P.gtr.th < 0.020 && -0.010 < P.gtr.ph && P.gtr.ph < 0.010";

    pslopecut[11.5] = new TCutG("pslopecut_115", 12);
    pslopecut[11.5]->SetVarX("P.gtr.ph");
    pslopecut[11.5]->SetVarY("P.gtr.th");

    pslopecut[11.5]->SetPoint(0,-0.0139148,-0.0415263);
    pslopecut[11.5]->SetPoint(1,-0.0184277,-0.0121579);
    pslopecut[11.5]->SetPoint(2,-0.0178009,0.0219474);
    pslopecut[11.5]->SetPoint(3,-0.0129119,0.0408947);
    pslopecut[11.5]->SetPoint(4,0.00940186,0.0418421);
    pslopecut[11.5]->SetPoint(5,0.0172994,0.0191053);
    pslopecut[11.5]->SetPoint(6,0.0201827,-0.0115263);
    pslopecut[11.5]->SetPoint(7,0.0130372,-0.0367895);
    pslopecut[11.5]->SetPoint(8,0.00313395,-0.0472105);
    pslopecut[11.5]->SetPoint(9,-0.0139148,-0.0418421);
    pslopecut[11.5]->SetPoint(10,-0.0139148,-0.0418421);
    pslopecut[11.5]->SetPoint(11,-0.0139148,-0.0415263);

    ztarcut[8.0]  = "abs(H.react.z)<3";
    ztarcut[9.5]  = "abs(H.react.z)<3";
    ztarcut[11.5] = "abs(H.react.z)<3";
    ztarcut[14.3] = "abs(H.react.z)<3";

    ytarcut[8.0]  = "abs(H.gtr.y)<3";
    ytarcut[9.5]  = "abs(H.gtr.y)<3";
    ytarcut[11.5] = "abs(H.gtr.y)<2";
    ytarcut[14.3] = "abs(H.gtr.y)<3";

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

    hfpcutg[9.5] = new TCutG("hfpcutg_95", 17);
    hfpcutg[9.5]->SetVarX("H.dc.y[0]");
    hfpcutg[9.5]->SetVarY("H.dc.x[0]");

    hfpcutg[9.5]->SetPoint(0, -4.54061, -1.67403);
    hfpcutg[9.5]->SetPoint(1, -3.68073, 4.38825);
    hfpcutg[9.5]->SetPoint(2, -1.10111, 9.05919);
    hfpcutg[9.5]->SetPoint(3, 2.46736, 13.3326);
    hfpcutg[9.5]->SetPoint(4, 7.45462, 13.432);
    hfpcutg[9.5]->SetPoint(5, 8.05653, 12.7363);
    hfpcutg[9.5]->SetPoint(6, 6.59475, 8.36352);
    hfpcutg[9.5]->SetPoint(7, 5.00398, 4.88516);
    hfpcutg[9.5]->SetPoint(8, 4.48806, -0.978357);
    hfpcutg[9.5]->SetPoint(9, 5.21895, -6.04682);
    hfpcutg[9.5]->SetPoint(10, 6.33678, -8.82951);
    hfpcutg[9.5]->SetPoint(11, 5.82086, -12.2085);
    hfpcutg[9.5]->SetPoint(12, 3.41322, -14.4943);
    hfpcutg[9.5]->SetPoint(13, -1.61704, -12.606);
    hfpcutg[9.5]->SetPoint(14, -4.66959, -9.12765);
    hfpcutg[9.5]->SetPoint(15, -4.5836, -1.77341);
    hfpcutg[9.5]->SetPoint(16, -4.54061, -1.67403);

    hfpcutg[11.5] = new TCutG("hfpcutg_115", 4);
    hfpcutg[11.5]->SetVarX("H.dc.y[0]");
    hfpcutg[11.5]->SetVarY("H.dc.x[0]");

    hfpcutg[11.5]->SetPoint(0,-10,-25);
    hfpcutg[11.5]->SetPoint(1,-10, 30);
    hfpcutg[11.5]->SetPoint(2, 25, 30);
    hfpcutg[11.5]->SetPoint(3, 25,-25);
    hfpcutg[11.5]->SetPoint(4,-10,-25);

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

    coinWcut = "0.85 < H.kin.primary.W && H.kin.primary.W < 1.04";
    singWcut = "0.85 < H.kin.W         && H.kin.W         < 1.04";

    singlescut = hpidcut[q2] && hdeltacut[q2] && hslopecut[q2]->GetName() && ztarcut[q2] && ytarcut[q2] && hfpcutg[q2]->GetName();
    coincut    = hpidcut[q2] && hdeltacut[q2] && hslopecut[q2]->GetName() && ztarcut[q2] && ytarcut[q2] && hfpcutg[q2]->GetName() && pdeltacut[q2] && pslopecut[q2]->GetName() && ppidcut[q2] && emisscut[q2];

    TFile fWrite(Form("q2_%.1f_canvas.root", q2), "recreate");


    std::cout << "get trees" << std::endl;
    tcoin  = (TTree*) fcoin[q2]->Get("T");
    tsing  = (TTree*) fsing[q2]->Get("T");
    tdummy = (TTree*) fdummy[q2]->Get("T");

    std::cout << "generate histos" << std::endl;

    // Missing energy
    tcoin->Draw("P.kin.secondary.emiss>>h_emiss(140,-0.04,0.1)");

    // Delta
    tcoin->Draw("P.gtr.dp:H.gtr.dp>>h_delta(200,-10,10,200,-10,10)", emisscut[q2],"colz");

    // xptar and yptar
    tcoin->Draw("H.gtr.th:H.gtr.ph>>h_hslope_coin(50,-.1,.1,50,-.1,.1)", emisscut[q2]&&hdeltacut[q2]&&pdeltacut[q2],"colz");
    tsing->Draw("H.gtr.th:H.gtr.ph>>h_hslope_sing(50,-.1,.1,50,-.1,.1)", hdeltacut[q2]&&hpidcut[q2]&&ztarcut[q2]&&ytarcut[q2],"colz");
    tcoin->Draw("P.gtr.th:P.gtr.ph>>h_pslope_coin(40,-.040,.040,60,-.060,.060)", emisscut[q2]&&hdeltacut[q2]&&pdeltacut[q2]&&hslopecut[q2]->GetName(),"colz");

    // ztar
    tdummy->Draw("H.react.z>>h_dummy_reactz_open(100,-10,10)");
    tdummy->Draw("H.react.z>>h_dummy_reactz_cut(100,-10,10)", hdeltacut[q2]&&hslopecut[q2]->GetName());
    tcoin->Draw("H.react.z>>h_coin_reactz_open(100,-10,10)");
    tcoin->Draw("H.react.z>>h_coin_reactz_cut(100,-10,10)", hdeltacut[q2]&&hslopecut[q2]->GetName());

    // ytar
    tdummy->Draw("H.gtr.y>>h_dummy_ytar_open(100,-5,5)");
    tdummy->Draw("H.gtr.y>>h_dummy_ytar_cut(100,-5,5)", hdeltacut[q2]&&hslopecut[q2]->GetName());
    tcoin->Draw("H.gtr.y>>h_coin_ytar_open(100,-5,5)");
    tcoin->Draw("H.gtr.y>>h_coin_ytar_cut(100,-5,5)", hdeltacut[q2]&&hslopecut[q2]->GetName());

    // xtar
    tdummy->Draw("H.gtr.x>>h_dummy_xtar_open(100,-0.5,0.5)");
    tdummy->Draw("H.gtr.x>>h_dummy_xtar_cut(100,-0.5,0.5)", hdeltacut[q2]&&hslopecut[q2]->GetName());
    tcoin->Draw("H.gtr.x>>h_coin_xtar_open(100,-0.5,0.5)");
    tcoin->Draw("H.gtr.x>>h_coin_xtar_cut(100,-0.5,0.5)", hdeltacut[q2]&&hslopecut[q2]->GetName());

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
    tcoin->Draw("H.dc.x[0]:H.dc.y[0]>>h_focalplane_coin(40,-15,25,65,-30,35)", hpidcut[q2] && hdeltacut[q2] && hslopecut[q2]->GetName() && ztarcut[q2] && ytarcut[q2] && coinWcut);
    tsing->Draw("H.dc.x[0]:H.dc.y[0]>>h_focalplane_sing(40,-15,25,65,-30,35)", hpidcut[q2] && hdeltacut[q2] && hslopecut[q2]->GetName() && ztarcut[q2] && ytarcut[q2] && singWcut);

    // These are used for calculating yields in the absorption script
    tcoin->Draw("H.kin.primary.W>>h_coin_W_count(160,0.8,1.2)", ytarcut[q2] && ztarcut[q2] && hpidcut[q2] && hdeltacut[q2] && coinWcut && hslopecut[q2]->GetName() && hfpcutg[q2]->GetName() && "-0.02 < P.kin.secondary.emiss" && "P.kin.secondary.emiss < 0.04");
    tsing->Draw("H.kin.W>>h_sing_W_count(80,0.8,1.2)",          ytarcut[q2] && ztarcut[q2] && hpidcut[q2] && hdeltacut[q2] && singWcut && hslopecut[q2]->GetName() && hfpcutg[q2]->GetName());

    // Get histos from memory
    TH1F* h_emiss                 = (TH1F*) gDirectory->Get("h_emiss");
    TH1F* h_delta                 = (TH1F*) gDirectory->Get("h_delta");
    TH1F* h_hslope_coin           = (TH1F*) gDirectory->Get("h_hslope_coin");
    TH1F* h_hslope_sing           = (TH1F*) gDirectory->Get("h_hslope_sing");
    TH1F* h_pslope_coin           = (TH1F*) gDirectory->Get("h_pslope_coin");
    TH1F* h_dummy_reactz_cut      = (TH1F*) gDirectory->Get("h_dummy_reactz_cut");
    TH1F* h_dummy_reactz_open     = (TH1F*) gDirectory->Get("h_dummy_reactz_open");
    TH1F* h_coin_reactz_cut       = (TH1F*) gDirectory->Get("h_coin_reactz_cut");
    TH1F* h_coin_reactz_open      = (TH1F*) gDirectory->Get("h_coin_reactz_open");
    TH1F* h_dummy_ytar_cut        = (TH1F*) gDirectory->Get("h_dummy_ytar_cut");
    TH1F* h_dummy_ytar_open       = (TH1F*) gDirectory->Get("h_dummy_ytar_open");
    TH1F* h_coin_ytar_cut         = (TH1F*) gDirectory->Get("h_coin_ytar_cut");
    TH1F* h_coin_ytar_open        = (TH1F*) gDirectory->Get("h_coin_ytar_open");
    TH1F* h_dummy_xtar_cut        = (TH1F*) gDirectory->Get("h_dummy_xtar_cut");
    TH1F* h_dummy_xtar_open       = (TH1F*) gDirectory->Get("h_dummy_xtar_open");
    TH1F* h_coin_xtar_cut         = (TH1F*) gDirectory->Get("h_coin_xtar_cut");
    TH1F* h_coin_xtar_open        = (TH1F*) gDirectory->Get("h_coin_xtar_open");
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
    TH1F* h_focalplane_sing       = (TH1F*) gDirectory->Get("h_focalplane_sing");
    TH1F* h_focalplane_coin       = (TH1F*) gDirectory->Get("h_focalplane_coin");
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
    h_pslope_coin->SetTitle("SHMS target variables [rad];yptar;xptar");
    h_pslope_coin->Draw("colz");
    pslopecut[q2]->Draw("SAME");
    pslopecut[q2]->SetLineColor(kRed);
    pslopecut[q2]->SetLineWidth(3);
    pad->GetPad(4)->SetGrid();

    pad->cd(4);
    h_hslope_coin->SetTitle("HMS target variables [rad];yptar;xptar");
    h_hslope_coin->Draw("colz");
    hslopecut[q2]->Draw("SAME");
    hslopecut[q2]->SetLineColor(kRed);
    hslopecut[q2]->SetLineWidth(3);
    pad->GetPad(3)->SetGrid();

    pad->cd(5);
    h_dummy_reactz_cut->SetTitle("ztar [cm];ztar;Count");
    h_dummy_reactz_open->SetTitle("ztar [cm];ztar;Count");
    h_dummy_reactz_cut->SetLineColor(46);
    h_dummy_reactz_cut->SetFillColor(46);
    h_dummy_reactz_cut->SetFillStyle(3345);
    h_dummy_reactz_open->SetLineColor(30);
    h_dummy_reactz_open->SetFillColor(30);
    h_dummy_reactz_open->SetFillStyle(3354);
    h_dummy_reactz_open->Draw();
    h_dummy_reactz_cut->Draw("SAME");

    pad->cd(6);
    h_focalplane_coin->SetTitle("HMS focal plane coordinates [cm];y_{fp};x_{fp}");
    h_focalplane_coin->Draw("colz");
    hfpcutg[q2]->Draw("SAME");
    hfpcutg[q2]->SetLineColor(kRed);
    hfpcutg[q2]->SetLineWidth(3);
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
    h_coinW_singlescut->SetTitle("W ;W;Counts");
    h_coinW_coincut->SetTitle("W [GeV];W;Counts");
    h_coinW_inpeak->SetTitle("W [GeV];W;Counts");
    h_coinW_open->SetTitle("W (coincidence);W;Counts");
    h_coinW_singlescut->SetLineColor(46); // red
    h_coinW_singlescut->SetFillColor(46);
    h_coinW_singlescut->SetFillStyle(3345);
    h_coinW_coincut->SetLineColor(38); // blue
    h_coinW_coincut->SetFillColor(38);
    h_coinW_coincut->SetFillStyle(3354);
    h_coinW_inpeak->SetLineColor(1);
    h_coinW_inpeak->SetFillColor(1);
    h_coinW_inpeak->SetFillStyle(3345);
    h_coinW_open->SetLineColor(30); // green
    h_coinW_open->SetFillColor(30);
    h_coinW_open->SetFillStyle(3002);
    h_coinW_open->Draw();
    h_coinW_singlescut->Draw("SAME");
    h_coinW_coincut->Draw("SAME");

    pad->cd(9);
    h_singW_cut->SetTitle("W [GeV];W;Counts");
    h_singW_inpeak->SetTitle("W [GeV];W;Counts");
    h_singW_open->SetTitle("W (singles);W;Counts");
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
    h_singW_open_wide->SetTitle("W (singles);W;Counts");
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
    h_hslope_coin->Write();
    h_hslope_sing->Write();
    h_pslope_coin->Write();
    h_dummy_reactz_cut->Write();
    h_dummy_reactz_open->Write();
    h_coin_reactz_cut->Write();
    h_coin_reactz_open->Write();
    h_dummy_ytar_cut->Write();
    h_dummy_ytar_open->Write();
    h_coin_ytar_cut->Write();
    h_coin_ytar_open->Write();
    h_dummy_xtar_cut->Write();
    h_dummy_xtar_open->Write();
    h_coin_xtar_cut->Write();
    h_coin_xtar_open->Write();
    h_ngc_open->Write();
    h_ngc_singlescut->Write();
    h_ngc_coincut->Write();
    h_focalplane_sing->Write();
    h_focalplane_coin->Write();
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
