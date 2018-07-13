#ifndef CTCUTS_H
#define CTCUTS_H

#include <map>

#include <TString.h>

// ------------------------------------------------------------------------
// PID etc cuts
TCut hdeltaCut   = "H.gtr.dp > -10 && H.gtr.dp < 10";
TCut pdeltaCut   = "P.gtr.dp > -12 && P.gtr.dp < 12";
TCut deltaCut    = pdeltaCut && hdeltaCut;
TCut pBetaCut    = "P.gtr.beta > 0.8 && P.gtr.beta < 1.3";                        
TCut hBetaCut    = "H.gtr.beta > 0.8 &&  H.gtr.beta < 1.3";
TCut cerCut      = "P.hgcer.npeSum < 0.1 && H.cer.npeSum > 0.5";                    
TCut calCut      = "H.cal.etottracknorm > 0.8 && H.cal.etottracknorm < 1.5"; // && H.cal.eprtracknorm  > 0.2";         
TCut hodoTimeCut = "P.hod.goodstarttime == 1 && H.hod.goodstarttime == 1";      
TCut Wcut        = "H.kin.primary.W > 0.75 && H.kin.primary.W < 1.15";
TCut Pmisscut    = "abs(P.kin.secondary.pmiss) < 0.4";

// cuts is a map that takes a string, TARGET, and returns a TCut
std::map<TString, TCut> cuts;
cuts["C12"] = hBetaCut && pBetaCut && cerCut && calCut && hodoTimeCut && Pmisscut;
cuts["LH2"] = hBetaCut && pBetaCut && cerCut && calCut && hodoTimeCut && Wcut;
cuts["Al"]  = hBetaCut && pBetaCut && cerCut && calCut && hodoTimeCut && Wcut;

// ------------------------------------------------------------------------
// Tracking efficiency cuts
TCut pScinGood   = "P.hod.goodscinhit == 1";
TCut pGoodBeta   = "P.hod.betanotrack > 0.5 && P.hod.betanotrack < 1.4";
TCut pDC1NoLarge = "(P.dc.1x1.nhit + P.dc.1u2.nhit + P.dc.1u1.nhit + P.dc.1v1.nhit + P.dc.1x2.nhit + P.dc.1v2.nhit) < 21";
TCut pDC2NoLarge = "(P.dc.2x1.nhit + P.dc.2u2.nhit + P.dc.2u1.nhit + P.dc.2v1.nhit + P.dc.2x2.nhit + P.dc.2v2.nhit) < 21";
TCut pDCNoLarge  = pDC1NoLarge && pDC1NoLarge;
TCut pScinShould = pScinGood && pGoodBeta && pDCNoLarge;
TCut pScinDid    = pScinShould && "P.dc.ntrack>0";

TCut pSinglesPID  = "P.hgcer.npeSum < 0.1";
TCut pScinShouldh = pScinShould && pSinglesPID;
TCut pScinDidh    = pScinDid && pSinglesPID;

TCut hScinGood   = "H.hod.goodscinhit == 1";
TCut hGoodBeta   = "H.hod.betanotrack > 0.5 && H.hod.betanotrack < 1.4";
TCut hDC1NoLarge = "(H.dc.1x1.nhit + H.dc.1u2.nhit + H.dc.1u1.nhit + H.dc.1v1.nhit + H.dc.1x2.nhit + H.dc.1v2.nhit) < 21";
TCut hDC2NoLarge = "(H.dc.2x1.nhit + H.dc.2u2.nhit + H.dc.2u1.nhit + H.dc.2v1.nhit + H.dc.2x2.nhit + H.dc.2v2.nhit) < 21";
TCut hDCNoLarge  = hDC1NoLarge && hDC1NoLarge;
TCut hScinShould = hScinGood && hGoodBeta && hDCNoLarge;
TCut hScinDid    = hScinShould && "H.dc.ntrack>0";

TCut hSinglesPID  = "H.cer.npeSum > 0.5 && H.cal.etotnorm > 0.8 && H.cal.etotnorm < 1.5";
TCut hScinShoulde = hScinShould && hSinglesPID;
TCut hScinDide    = hScinDid && hSinglesPID;

// ------------------------------------------------------------------------
// Cherenkov efficiency cuts
TCut hCalPID = "H.cal.etotnorm > 0.8 && H.cal.etotnorm < 1.5";

#endif
