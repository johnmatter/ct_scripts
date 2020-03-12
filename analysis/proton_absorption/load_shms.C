{
    std::map<Double_t, TFile*> fcoin, fsing_shms, fdummy_shms;

    // Q^2 = 8
    fcoin[8.0]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_8_smallcoll.root");
    fsing_shms[8.0]  = new TFile("/Volumes/ssd750/ct/pass3/shms_coin_replay_production_2049_500000.root");
    fdummy_shms[8.0] = new TFile("/Volumes/ssd750/ct/pass3/dummy_shms_q2_8.root");

    // Q^2 = 9.5
    fcoin[9.5]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_9.5_smallcoll.root");
    fsing_shms[9.5]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_shms_singles_q2_9.5.root");
    fdummy_shms[9.5] = new TFile("/Volumes/ssd750/ct/pass3/dummy_shms_q2_9.5.root");

    // Q^2 = 11.5
    fcoin[11.5]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_11.5_largecoll.root");
    fsing_shms[11.5]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_shms_singles_q2_11.5.root");
    fdummy_shms[11.5] = new TFile("/Volumes/ssd750/ct/pass3/dummy_shms_q2_11.5.root");

    // Q^2 = 14.3
    fcoin[14.3]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_14.3_largecoll.root");
    fsing_shms[14.3]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_shms_singles_q2_14.3.root");
    fdummy_shms[14.3] = new TFile("/Volumes/ssd750/ct/pass3/dummy_shms_q2_14.3.root");

}
