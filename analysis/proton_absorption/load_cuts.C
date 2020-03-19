{
    std::map<Double_t, TCut> ppidcut, hpidcut, emisscut, pdeltacut, hdeltacut, pslopecut, hslopecut, ztarcut, hfpcut;
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

    hfpcut[8.0]  = "";
    hfpcut[9.5]  = "0.0<H.dc.x[0] && H.dc.x[0]<20.0 && -3.0<H.dc.y[0] && H.dc.y[0]<8.0";
    hfpcut[11.5] = "0.0<H.dc.x[0] && H.dc.x[0]<20.0 && -3.0<H.dc.y[0] && H.dc.y[0]<8.0";
    hfpcut[14.3] = "";

    coinWcut = "0.90 < H.kin.primary.W && H.kin.primary.W < 1.00";
    singWcut = "0.90 < H.kin.W         && H.kin.W         < 1.00";
}
