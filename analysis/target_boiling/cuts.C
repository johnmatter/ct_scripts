{

    TCut insideDipoleExit = "P.dc.InsideDipoleExit==1";
    TCut betaCut = "P.gtr.beta > 0.6 && P.gtr.beta < 1.4";
    TCut deltaCut = "P.gtr.dp > -10 && P.gtr.dp < 12";
    TCut hodostartCut = "P.hod.goodstarttime==1";

    TCut calCut = "P.cal.etottracknorm > 0.7 && P.cal.eprtracknorm > 0.035";
    TCut cerCut = "P.ngcer.npeSum > 5.0";
    TCut pidCut = calCut && cerCut;

    TCut qualityCut = betaCut && hodostartCut && cerCut && calCut && insideDipoleExit;

    TCut pScinGood = "P.hod.goodscinhit==1";
    TCut pGoodBeta = " P.hod.betanotrack < 1.4";
    TCut pDC1NoLarge = "(P.dc.1x1.nhit + P.dc.1u2.nhit + P.dc.1u1.nhit + P.dc.1v1.nhit + P.dc.1x2.nhit + P.dc.1v2.nhit) < 25";
    TCut pDC2NoLarge = "(P.dc.2x1.nhit + P.dc.2u2.nhit + P.dc.2u1.nhit + P.dc.2v1.nhit + P.dc.2x2.nhit + P.dc.2v2.nhit) < 25";
    TCut pDCNoLarge = pDC1NoLarge && pDC2NoLarge;
    TCut pScinShould = pScinGood && pGoodBeta && pDCNoLarge;
    TCut pScinDid    = pScinShould && "P.dc.ntrack>0";


    // Deepak's tracking cuts
    TCut DeepakBetaCut = "P.gtr.beta < 1.4 && P.gtr.beta > 0.8";
    TCut DeepakCerCut = "P.ngcer.npeSum > 5.0";
    TCut DeepakCalCut = "P.cal.eprtracknorm > 0.035 && P.cal.etottracknorm > 0.7";
    TCut tightAcceptanceCut = "P.gtr.th < 0.05 && P.gtr.th > -0.05 && P.gtr.dp < 12 && P.gtr.dp > -10.0 && P.gtr.ph < 0.05 && P.gtr.ph > -0.05 && P.dc.y_fp > -20 && P.dc.y_fp < 25 && P.dc.x_fp > -25 && P.dc.x_fp < 20";
    TCut DeepakCalShould = DeepakBetaCut && pScinGood && insideDipoleExit && tightAcceptanceCut && DeepakCerCut;
    TCut DeepakCalDid    = DeepakBetaCut && pScinGood && insideDipoleExit && tightAcceptanceCut && DeepakCerCut && DeepakCalCut;
    TCut DeepakCerShould = DeepakBetaCut && pScinGood && insideDipoleExit && tightAcceptanceCut && DeepakCalCut;
    TCut DeepakCerDid    = DeepakBetaCut && pScinGood && insideDipoleExit && tightAcceptanceCut && DeepakCalCut && DeepakCerCut;

    // TCut calShouldCut = DeepakCalShould;
    // TCut calDidCut = DeepakCalDid;
    // TCut cerShouldCut = DeepakCerShould;
    // TCut cerDidCut = DeepakCerDid;

    TCut calShouldCut = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && cerCut;
    TCut calDidCut    = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && cerCut && calCut;
    TCut cerShouldCut = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && calCut;
    TCut cerDidCut    = tightAcceptanceCut && insideDipoleExit && betaCut && deltaCut && hodostartCut && calCut && cerCut;

    TCut DeepakTrackCer = "P.ngcer.npeSum > 5.0";
    TCut DeepakTrackCal = "P.cal.etotnorm > 0.7";
    TCut DeepakBetanotrack = "P.hod.betanotrack < 1.4";

    TCut DeepakTrackShould = DeepakTrackCer && DeepakTrackCal && DeepakBetanotrack && pDC1NoLarge && pDC2NoLarge && pScinGood && hodostartCut && insideDipoleExit;

    TCut DeepakDelta4 = "-20 < P.gtr.dp && P.gtr.dp < 20";
    TCut DeepakDelta5 = "-15 < P.gtr.dp && P.gtr.dp < 15";
    TCut DeepakDelta7 = "-15 < P.gtr.dp && P.gtr.dp < 15";
    TCut DeepakY = "-5 < P.gtr.y && P.gtr.y < 5";
    TCut DeepakAngle = "-0.2 < P.gtr.th && P.gtr.th < 0.2 && -0.2 < P.gtr.ph && P.gtr.ph< 0.2 ";
    TCut fewNegADChits = "P.hod.1x.totNumGoodNegAdcHits<5 && P.hod.1y.totNumGoodNegAdcHits<5 && P.hod.2x.totNumGoodNegAdcHits<5 && P.hod.2y.totNumGoodNegAdcHits<5";
    TCut goodFpTime = "-10 < P.hod.1x.fptime && P.hod.1x.fptime < 50 && -10 < P.hod.1y.fptime && P.hod.1y.fptime < 50 && -10 < P.hod.2x.fptime && P.hod.2x.fptime < 50 && -10 < P.hod.2y.fptime && P.hod.2y.fptime < 50";

    TCut oneTrack = "P.dc.ntrack==1";
    TCut multiTrack = "P.dc.ntrack>1";

    TCut DeepakTrackDid4 = DeepakTrackShould && ( (oneTrack && DeepakDelta4) || (multiTrack && DeepakDelta4 && DeepakY) );
    TCut DeepakTrackDid5 = DeepakTrackShould && ( (oneTrack && DeepakDelta5) || (multiTrack && DeepakDelta5 && DeepakY) );
    TCut DeepakTrackDid7 = DeepakTrackShould && ( (oneTrack                ) || (multiTrack && DeepakDelta7 && DeepakY && DeepakAngle && goodFpTime && fewNegADChits) );

    // Which tracking cuts should we use?
    // TODO: this should be a command line argument instead
    TCut trackShouldCut = DeepakTrackShould;
    TCut trackDidCut = DeepakTrackDid7;

    TCut goodCut = betaCut && deltaCut && hodostartCut && pidCut && insideDipoleExit;

}
