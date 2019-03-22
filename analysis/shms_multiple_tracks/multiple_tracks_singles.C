#include <utility>
#include <vector>
#include <tuple>
#include <map>

#include <TEventList.h>

#include <CTData.h>
#include <CTCuts.h>

// Compare how many multiple track events are in our large and
// pion collimator singles runs.
void multiple_tracks_singles() {
    // ------------------------------------------------------------------------
    // 0) Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Get TChains
    TChain* pionTree  = data->GetChain("C12_Q2_14_pion_collimator");
    TChain* largeTree = data->GetChain("C12_Q2_14_large_collimator");

    // What cuts?
    TCut cleanUp = cuts->Get("pCerCut") && cuts->Get("pDeltacut") && cuts->Get("pBetaCut");
    TCut cutSingleTracks   = "P.dc.ntrack==1";
    TCut cutMultipleTracks = "P.dc.ntrack>1";

    // ------------------------------------------------------------------------
    // 1) Make TEventLists

    // Pion collimator
    pionTree->Draw(">>singleTracksPionList", cutSingleTracks);
    TEventList* singleTracksPionList = (TEventList*) gDirectory->Get("singleTracksPionList");
    pionTree->Draw(">>multipleTracksPionList", cutMultipleTracks);
    TEventList* multipleTracksPionList  = (TEventList*) gDirectory->Get("multipleTracksPionList");

    // Large collimator
    largeTree->Draw(">>singleTracksLargeList", cutSingleTracks);
    TEventList* singleTracksLargeList = (TEventList*) gDirectory->Get("singleTracksLargeList");
    largeTree->Draw(">>multipleTracksLargeList", cutMultipleTracks);
    TEventList* multipleTracksLargeList = (TEventList*) gDirectory->Get("multipleTracksLargeList");

    // ------------------------------------------------------------------------
    // 2) Get number of entries
    Int_t numSingleTracksPion   = singleTracksPionList->GetN();
    Int_t numMultipleTracksPion = multipleTracksPionList->GetN();
    Int_t numSingleTracksLarge   = singleTracksLargeList->GetN();
    Int_t numMultipleTracksLarge = multipleTracksLargeList->GetN();

    // ------------------------------------------------------------------------
    // 3) Get beam charge
    // For one run, the last entry in the scaler tree is the total charge.
    // However, I've chained runs together, so we have to find all the local maxima.
    // Fortunately, scalerCharge is monotonically increasing within a run
    // So we can find where Q(n)<Q(n-1) and keep Q(n-1)
    Double_t Q, Qprev, Qtot;
    Int_t NEntries;
    TChain* t;

    Double_t pionQ, largeQ;

    // Pion collimator
    t = data->GetChain("C12_Q2_14_pion", "TSP");
    t->SetBranchAddress("P.BCM4A.scalerChargeCut",&Q);

    Q = Qprev = Qtot = 0;
    for (int i=1; i<t->GetEntries(); t->GetEntry(i++)) {
        if ( (Q<Qprev) || (i==(t->GetEntries()-1)) ) {
           Qtot += Qprev;
        }
        Qprev=Q;
    }
    pionQ = Qtot;

    // Large collimator
    t = data->GetChain("C12_Q2_14_large", "TSP");
    t->SetBranchAddress("P.BCM4A.scalerChargeCut",&Q);

    Q = Qprev = Qtot = 0;
    for (int i=1; i<t->GetEntries(); t->GetEntry(i++)) {
        if ( (Q<Qprev) || (i==(t->GetEntries()-1)) ) {
           Qtot += Qprev;
        }
        Qprev=Q;
    }
    largeQ = Qtot;

    // ------------------------------------------------------------------------
    // 4) Print results


    std::cout << "collimator,numSingleTrackEvents,numMultipleTrackEvents,Q" << std::endl;
    std::cout << "large"
              << "," << numSingleTracksLarge
              << "," << numMultipleTracksLarge
              << "," << largeQ
              << std::endl;
    std::cout << "pion"
              << "," << numSingleTracksPion
              << "," << numMultipleTracksPion
              << "," << pionQ
              << std::endl;

}
