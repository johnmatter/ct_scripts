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
#include <TEventList.h>

// Using tighter HMS-only cuts, we calculate the yield per charge for singles
// and coincidence runs. The absorption should be 1-(coin yield / singles yield)

void calculate_absorption() {

    TString kinematics = "LH2_Q2_12";

    // Load data
    TFile *fcoin = new TFile("/home/jmatter/ROOTfiles/pass5/coin_replay_production_LH2_11.5_largecoll.root");
    TFile *fsing = new TFile("/home/jmatter/ROOTfiles/pass5/hms_coin_replay_production_2454_-1.root");
    TTree *tcoin = (TTree*) fcoin->Get("T");
    TTree *tsing = (TTree*) fsing->Get("T");
    TTree *tshcoin = (TTree*) fcoin->Get("TSH");
    TTree *tshsing = (TTree*) fsing->Get("TSH");

    // Tight HMS cuts
    TCut hmsCut = "H.gtr.th>-.06 && H.gtr.th<.06 && H.gtr.ph>-.02 && H.gtr.ph<.03";
    hmsCut = hmsCut && "H.gtr.dp>-4 && H.gtr.dp<8";
    hmsCut = hmsCut && "H.cer.npeSum>0 && H.cal.etottracknorm>0.9 && H.cal.etottracknorm<1.1";

    // Need different cuts because the kinematics modules are named differently
    TCut coinCut    = hmsCut && "H.kin.primary.W > 0.85 && H.kin.primary.W < 1.03";
    TCut singlesCut = hmsCut && "H.kin.W > 0.85 && H.kin.W < 1.03";

    // Used below to access yield, charge, etc.
    TChain* t;
    Int_t NEntries;
    Int_t singlesN, coinN;
    Double_t Q, Qprev, Qtot;
    Double_t singlesQ, coinQ;
    Double_t singlesY, coinY;
    Double_t absorption;
    TString elistName, elistDraw;
    TEventList* elist;

    // Efficiency; HMS cancels out in ratio so we only need SHMS
    Double_t cer = 0.993903;
    Double_t hod = 0.9998273;
    Double_t track = 0.972022;
    Double_t eff = cer*hod*track;

    //--------------------------------------------------------------------------
    // Get yield

    // Singles
    elistName.Form("%s_singlesElist", kinematics.Data());
    elistDraw.Form(">>%s", elistName.Data());
    tsing->Draw(elistDraw, singlesCut);
    elist = (TEventList*) gDirectory->Get(elistName.Data());
    singlesN = elist->GetN();

    // Coin
    elistName.Form("%s_coinElist", kinematics.Data());
    elistDraw.Form(">>%s", elistName.Data());
    tcoin->Draw(elistDraw, coinCut);
    elist = (TEventList*) gDirectory->Get(elistName.Data());
    coinN = elist->GetN();

    //--------------------------------------------------------------------------
    // Get beam charge
    //
    // For one run, the last entry in the scaler tree is the total charge.
    // However, I've chained runs together, so we have to find all the local
    // maxima. Fortunately, scalerCharge is monotonically increasing within
    // a run and resets to zero for the next run in the chain.
    // So we can find where Q(n)<Q(n-1) and add Q(n-1) to a total

    // // Singles
    // tshsing->SetBranchAddress("H.BCM4A.scalerChargeCut",&Q);
    // Q = Qprev = Qtot = 0;
    // for (int i=1; i<tshsing->GetEntries(); tshsing->GetEntry(i++)) {
    //     if ( (Q<Qprev) || (i==(tshsing->GetEntries()-1)) ) {
    //        Qtot += (Q-Qprev);
    //     }
    //     Qprev=Q;
    // }
    // singlesQ = Qtot;

    // // Coin
    // tshcoin->SetBranchAddress("H.BCM4A.scalerChargeCut",&Q);
    // Q = Qprev = Qtot = 0;
    // for (int i=1; i<tshcoin->GetEntries(); tshcoin->GetEntry(i++)) {
    //     if ( (Q<Qprev) || (i==(tshcoin->GetEntries()-1)) ) {
    //        Qtot += (Q-Qprev);
    //     }
    //     Qprev=Q;
    // }
    // coinQ = Qtot;

    // coinQ = 386.160;
    // singlesQ = 88.3596;

    coinQ = 371.492;
    singlesQ = 85.0219;


    //--------------------------------------------------------------------------
    // Calculate yield and absorption
    singlesY = singlesN/singlesQ;
    coinY = coinN/coinQ;

    absorption = 1-(coinY/singlesY);

    //--------------------------------------------------------------------------
    // Print
    std::cout << "kinematics,type,Q,N,Y" << std::endl;
    std::cout << kinematics << ",singles," << singlesQ << "," << singlesN << "," << singlesY << std::endl;
    std::cout << kinematics << ",coin," << coinQ << "," << coinN << "," << coinY << std::endl;
    std::cout << std::endl;
    std::cout << "absorption = " << absorption << std::endl;

}

