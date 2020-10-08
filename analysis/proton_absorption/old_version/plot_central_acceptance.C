#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TH1F.h>
#include <TH2F.h>

// Before calculate absorption, we need to know where good coin events'
// electrons lie in the HMS acceptance. To that end, I place tight cuts
// on SHMS xptar, yptar, and dp; that defines a region of SHMS acceptance
// to be replicated by using tighter HMS-only cuts.

void plot_central_acceptance() {
    gStyle->SetOptStat(11111);

    TString pdfFilename = "acceptance.pdf";
    TString drawMe, drawMeTemplate;
    TString histoName, histoNameTemplate;

    // Load data and cuts
    TFile *fcoin = new TFile("/home/jmatter/ROOTfiles/pass5/coin_replay_production_LH2_11.5_largecoll.root");
    TFile *fsing = new TFile("/home/jmatter/ROOTfiles/pass5/hms_coin_replay_production_2454_-1.root");
    TTree *tcoin = (TTree*) fcoin->Get("T");
    TTree *tsing = (TTree*) fsing->Get("T");
    TTree *tshcoin = (TTree*) fcoin->Get("TSH");
    TTree *tshsing = (TTree*) fsing->Get("TSH");


    // Good ep coincidences
    TCut coinCut  = "((((((((P.gtr.beta < 1.4)&&(H.gtr.beta > 0.8 &&  H.gtr.beta < 1.2))&&((P.gtr.dp > -10 && P.gtr.dp < 12)&&(H.gtr.dp > -10 && H.gtr.dp < 10)))&&(((H.cal.etottracknorm > 0.8 && H.cal.etottracknorm < 1.15)&&(H.cer.npeSum > 0.0))&&(P.ngcer.npeSum < 0.1)))&&((P.hod.goodstarttime==1)&&(H.hod.goodstarttime==1)))&&(-0.03 < P.kin.secondary.emiss && P.kin.secondary.emiss < 0.09 && abs(P.kin.secondary.pmiss) < 0.09))&&(H.dc.InsideDipoleExit==1))&&(P.dc.InsideDipoleExit==1))&&(8.5 < CTime.epCoinTime_ROC2 && CTime.epCoinTime_ROC2 < 12.5)";

    // Tight SHMS cuts: for determining tight HMS-only cuts
    TCut tightCut = "abs(P.gtr.th)<.04 && abs(P.gtr.ph)<.015 && (P.gtr.dp<4) && (P.gtr.dp>-3)";

    // Tight HMS cuts
    TCut hmsCut = "H.gtr.th>-.06 && H.gtr.th<.06 && H.gtr.ph>-.02 && H.gtr.ph<.03";
    hmsCut = hmsCut && "H.gtr.dp>-4 && H.gtr.dp<8";
    hmsCut = hmsCut && "H.cer.npeSum>0 && H.cal.etottracknorm>0.9 && H.cal.etottracknorm<1.1";
    hmsCut = hmsCut && "H.kin.primary.W > 0.85 && H.kin.primary.W < 1.03";

    //-------------------------------------------------------------------------
    // Plot
    TCanvas *ctight = new TCanvas("ctight", "ctight", 1200, 600);
    TCanvas *ccoin = new TCanvas("ccoin", "ccoin", 1200, 600);
    TCanvas *chms = new TCanvas("chms", "chms", 1200, 600);
    ctight->Divide(4,2);
    ccoin->Divide(4,2);
    chms->Divide(4,2);
    std::map<TString, TH1*> histos;
    std::map<TString, TString> histoDrawStrings;

    //---------------------
    // Set up draw strings. This might be a little convoluted.
    // The names themselves are formats with an %s specifying the cut
    // SHMS xptar vs yptar
    histoName = "SHMS_xptar_vs_yptar_%s";
    histoDrawStrings[histoName] = Form("P.gtr.th:P.gtr.ph>>%s(100,-.05,.05,100,-.1,.1)", histoName.Data());

    // SHMS delta
    histoName = "SHMS_delta_%s";
    histoDrawStrings[histoName] = Form("P.gtr.dp>>%s(58,-12,17)", histoName.Data());

    // SHMS Cherenkov
    histoName = "SHMS_cherenkov_%s";
    histoDrawStrings[histoName] = Form("P.ngcer.npeSum:P.hgcer.npeSum>>%s(60,0,30,60,0,30)", histoName.Data());

    // SHMS Calorimeter
    histoName = "SHMS_calorimeter_%s";
    histoDrawStrings[histoName] = Form("P.cal.eprtracknorm:(P.cal.etottracknorm-P.cal.eprtracknorm)>>%s(100,0,1.0,100,0,1.0)", histoName.Data());

    // HMS xptar vs yptar
    histoName = "HMS_xptar_vs_yptar_%s";
    histoDrawStrings[histoName] = Form("H.gtr.th:H.gtr.ph>>%s(100,-.05,.05,100,-.1,.1)", histoName.Data());

    // HMS delta
    histoName = "HMS_delta_%s";
    histoDrawStrings[histoName] = Form("H.gtr.dp>>%s(40,-10,10)", histoName.Data());

    // HMS Cherenkov
    histoName = "HMS_cherenkov_%s";
    histoDrawStrings[histoName] = Form("H.cer.npeSum>>%s(60,0,30)", histoName.Data());

    // HMS Calorimeter
    histoName = "HMS_calorimeter_%s";
    histoDrawStrings[histoName] = Form("H.cal.etottracknorm>>%s(150,0,1.5)", histoName.Data());

    //---------------------
    // Draw histos for both cuts
    for (auto &h : histoDrawStrings) {
        // h.first is the key
        histoNameTemplate = h.first;
        drawMeTemplate = histoDrawStrings[histoNameTemplate];

        // Coin cut
        histoName = Form(histoNameTemplate,"coincut");
        drawMe    = Form(drawMeTemplate, "coincut");
        tcoin->Draw(drawMe.Data(), coinCut, "goff");
        histos[histoName] = (TH2F*) gDirectory->Get(histoName.Data());

        // Tight cut
        histoName = Form(histoNameTemplate,"tightcut");
        drawMe    = Form(drawMeTemplate, "tightcut");
        tcoin->Draw(drawMe.Data(), coinCut && tightCut, "goff");
        histos[histoName] = (TH2F*) gDirectory->Get(histoName.Data());

        // HMS cuts only
        histoName = Form(histoNameTemplate,"hmscut");
        drawMe    = Form(drawMeTemplate, "hmscut");
        tcoin->Draw(drawMe.Data(), hmsCut, "goff");
        histos[histoName] = (TH2F*) gDirectory->Get(histoName.Data());
    }

    //---------------------
    // Put histos on the canvases
    ccoin->cd(1); histos["SHMS_xptar_vs_yptar_coincut"]->Draw("colz");
    ccoin->cd(2); histos["SHMS_delta_coincut"]->Draw();
    ccoin->cd(3); histos["SHMS_cherenkov_coincut"]->Draw("colz");
    ccoin->cd(4); histos["SHMS_calorimeter_coincut"]->Draw("colz");
    ccoin->cd(5); histos["HMS_xptar_vs_yptar_coincut"]->Draw("colz");
    ccoin->cd(6); histos["HMS_delta_coincut"]->Draw();
    ccoin->cd(7); histos["HMS_cherenkov_coincut"]->Draw("colz");
    ccoin->cd(8); histos["HMS_calorimeter_coincut"]->Draw("colz");

    ctight->cd(1); histos["SHMS_xptar_vs_yptar_tightcut"]->Draw("colz");
    ctight->cd(2); histos["SHMS_delta_tightcut"]->Draw();
    ctight->cd(3); histos["SHMS_cherenkov_tightcut"]->Draw("colz");
    ctight->cd(4); histos["SHMS_calorimeter_tightcut"]->Draw("colz");
    ctight->cd(5); histos["HMS_xptar_vs_yptar_tightcut"]->Draw("colz");
    ctight->cd(6); histos["HMS_delta_tightcut"]->Draw();
    ctight->cd(7); histos["HMS_cherenkov_tightcut"]->Draw("colz");
    ctight->cd(8); histos["HMS_calorimeter_tightcut"]->Draw("colz");

    chms->cd(1); histos["SHMS_xptar_vs_yptar_hmscut"]->Draw("colz");
    chms->cd(2); histos["SHMS_delta_hmscut"]->Draw();
    chms->cd(3); histos["SHMS_cherenkov_hmscut"]->Draw("colz");
    chms->cd(4); histos["SHMS_calorimeter_hmscut"]->Draw("colz");
    chms->cd(5); histos["HMS_xptar_vs_yptar_hmscut"]->Draw("colz");
    chms->cd(6); histos["HMS_delta_hmscut"]->Draw();
    chms->cd(7); histos["HMS_cherenkov_hmscut"]->Draw("colz");
    chms->cd(8); histos["HMS_calorimeter_hmscut"]->Draw("colz");
}
