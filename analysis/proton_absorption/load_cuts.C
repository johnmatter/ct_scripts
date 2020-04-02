{
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

    hfpcutg[9.5] = new TCutG("hfpcutg_95", 15);
    hfpcutg[9.5]->SetVarX("H.dc.y[0]");
    hfpcutg[9.5]->SetVarY("H.dc.x[0]");
    hfpcutg[9.5]->SetPoint(0, -2.99191, 0.509301);
    hfpcutg[9.5]->SetPoint(1, -1.93975, 6.43035);
    hfpcutg[9.5]->SetPoint(2, 0.366906, 13.7806);
    hfpcutg[9.5]->SetPoint(3, 3.24011, 18.3745);
    hfpcutg[9.5]->SetPoint(4, 5.99191, 18.3745);
    hfpcutg[9.5]->SetPoint(5, 6.19424, 13.2702);
    hfpcutg[9.5]->SetPoint(6, 3.11871, 4.59279);
    hfpcutg[9.5]->SetPoint(7, 3.28058, -1.53244);
    hfpcutg[9.5]->SetPoint(8, 6.72032, -9.59732);
    hfpcutg[9.5]->SetPoint(9, 5.66817, -12.4558);
    hfpcutg[9.5]->SetPoint(10, 0.569245, -12.3537);
    hfpcutg[9.5]->SetPoint(11, -2.38489, -7.14723);
    hfpcutg[9.5]->SetPoint(12, -3.07284, -4.59505);
    hfpcutg[9.5]->SetPoint(13, -2.95144, 0.815563);
    hfpcutg[9.5]->SetPoint(14, -2.99191, 0.509301);

    hfpcutg[11.5] = new TCutG("hfpcutg_115", 14);
    hfpcutg[11.5]->SetVarX("H.dc.y[0]");
    hfpcutg[11.5]->SetVarY("H.dc.x[0]");
    hfpcutg[11.5]->SetPoint(0, -3.15378, -0.919918);
    hfpcutg[11.5]->SetPoint(1, -2.99191, 5.20531);
    hfpcutg[11.5]->SetPoint(2, 2.9973, 19.2933);
    hfpcutg[11.5]->SetPoint(3, 10.0791, 20.8246);
    hfpcutg[11.5]->SetPoint(4, 8.90558, 15.7203);
    hfpcutg[11.5]->SetPoint(5, 4.08993, 4.18444);
    hfpcutg[11.5]->SetPoint(6, 4.04946, -2.24705);
    hfpcutg[11.5]->SetPoint(7, 9.35072, -13.0683);
    hfpcutg[11.5]->SetPoint(8, 5.06115, -17.2539);
    hfpcutg[11.5]->SetPoint(9, 1.74281, -15.9267);
    hfpcutg[11.5]->SetPoint(10, -2.14209, -9.59732);
    hfpcutg[11.5]->SetPoint(11, -3.31565, -5.10549);
    hfpcutg[11.5]->SetPoint(12, -3.11331, -0.51157);
    hfpcutg[11.5]->SetPoint(13, -3.15378, -0.919918);

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
}
