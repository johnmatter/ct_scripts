#include "CTCuts.h"

CTCuts::CTCuts() {
    // ------------------------------------------------------------------------
    // SHMS cuts
    TCut pHodoTimeCut = "P.hod.goodstarttime == 1";
    TCut pDeltaCut    = "P.gtr.dp > -12 && P.gtr.dp < 12";
    TCut pBetaCut     = "P.gtr.beta > 0.8 && P.gtr.beta < 1.3";
    TCut pCalCut      = "P.cal.eprtracknorm < 0.2";
    TCut pCerCut      = "P.hgcer.npeSum < 0.1";
    TCut pPMissCut    = "abs(P.kin.secondary.pmiss) < 0.4";

    // ------------------------------------------------------------------------
    // HMS cuts
    TCut hHodoTimeCut = "H.hod.goodstarttime == 1";
    TCut hDeltaCut    = "H.gtr.dp > -10 && H.gtr.dp < 10";
    TCut hBetaCut     = "H.gtr.beta > 0.8 &&  H.gtr.beta < 1.3";
    TCut hCalCut      = "H.cal.etottracknorm > 0.8 && H.cal.etottracknorm < 1.5"; // && H.cal.eprtracknorm  > 0.2";
    TCut hCerCut      = "H.cer.npeSum > 0.5";
    TCut hWCut        = "H.kin.primary.W > 0.75 && H.kin.primary.W < 1.15";

    // ------------------------------------------------------------------------
    // Coincidence run cuts
    TCut betaCut     = pBetaCut && hBetaCut;
    TCut deltaCut    = pDeltaCut && hDeltaCut;
    TCut PIDCut      = pCerCut && hCalCut && hCerCut;
    TCut hodoTimeCut = pHodoTimeCut && hHodoTimeCut;

    fCoinCuts[TString("C12")] = betaCut && deltaCut && PIDCut && hodoTimeCut && pPMissCut;
    fCoinCuts[TString("LH2")] = betaCut && deltaCut && PIDCut && hodoTimeCut && hWCut;
    fCoinCuts[TString("Al")]  = betaCut && deltaCut && PIDCut && hodoTimeCut && hWCut;

    // ------------------------------------------------------------------------
    // Tracking efficiency cuts
    TCut pScinGood   = "P.hod.goodscinhit == 1";
    TCut pGoodBeta   = "P.hod.betanotrack > 0.5 && P.hod.betanotrack < 1.4";
    TCut pDC1NoLarge = "(P.dc.1x1.nhit + P.dc.1u2.nhit + P.dc.1u1.nhit + P.dc.1v1.nhit + P.dc.1x2.nhit + P.dc.1v2.nhit) < 21";
    TCut pDC2NoLarge = "(P.dc.2x1.nhit + P.dc.2u2.nhit + P.dc.2u1.nhit + P.dc.2v1.nhit + P.dc.2x2.nhit + P.dc.2v2.nhit) < 21";
    TCut pDCNoLarge  = pDC1NoLarge && pDC1NoLarge;
    TCut pScinShould = pScinGood && pGoodBeta && pDCNoLarge;
    TCut pScinDid    = pScinShould && "P.dc.ntrack>0";

    TCut pSinglesPID  = pCerCut;
    TCut pScinShouldh = pScinShould && pSinglesPID;
    TCut pScinDidh    = pScinDid && pSinglesPID;

    fPTrackShould = pScinShouldh;
    fPTrackDid    = pScinDidh;

    TCut hScinGood   = "H.hod.goodscinhit == 1";
    TCut hGoodBeta   = "H.hod.betanotrack > 0.5 && H.hod.betanotrack < 1.4";
    TCut hDC1NoLarge = "(H.dc.1x1.nhit + H.dc.1u2.nhit + H.dc.1u1.nhit + H.dc.1v1.nhit + H.dc.1x2.nhit + H.dc.1v2.nhit) < 21";
    TCut hDC2NoLarge = "(H.dc.2x1.nhit + H.dc.2u2.nhit + H.dc.2u1.nhit + H.dc.2v1.nhit + H.dc.2x2.nhit + H.dc.2v2.nhit) < 21";
    TCut hDCNoLarge  = hDC1NoLarge && hDC1NoLarge;
    TCut hScinShould = hScinGood && hGoodBeta && hDCNoLarge;
    TCut hScinDid    = hScinShould && "H.dc.ntrack>0";

    TCut hSinglesPID  = hCalCut && hCerCut;
    TCut hScinShoulde = hScinShould && hSinglesPID;
    TCut hScinDide    = hScinDid && hSinglesPID;

    fHTrackShould = hScinShoulde;
    fHTrackDid    = hScinDide;

    // ------------------------------------------------------------------------
    // Cherenkov efficiency cuts
    fPCerShould = pBetaCut && pCalCut && "P.dc.ntrack>0";
    fPCerDid    = fPCerShould && pCerCut;
    fHCerShould = hBetaCut && hCalCut && "H.dc.ntrack>0";
    fHCerDid    = fHCerShould && hCerCut;

    // ------------------------------------------------------------------------
    // Calorimeter efficiency cuts
    fPCalShould = pBetaCut && pCerCut && "P.dc.ntrack>0";
    fPCalDid    = fPCalShould && pCalCut;
    fHCalShould = hBetaCut && hCerCut && "H.dc.ntrack>0";
    fHCalDid    = fHCalShould && hCalCut;
}


