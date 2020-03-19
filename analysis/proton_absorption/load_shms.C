{
    std::map<Double_t, TFile*> fcoin, fsing_shms, fdummy_shms;
    std::map<Double_t, TTree*> tcoin, tsing_shms, tdummy_shms;

    // Q^2 = 8
    fcoin[8.0]       = new TFile("coin_replay_production_LH2_8_smallcoll.root");
    fsing_shms[8.0]  = new TFile("lh2_shms_singles_q2_8.root");
    fdummy_shms[8.0] = new TFile("dummy_shms_q2_8.root");

    // Q^2 = 9.5
    fcoin[9.5]       = new TFile("coin_replay_production_LH2_9.5_smallcoll.root");
    fsing_shms[9.5]  = new TFile("lh2_shms_singles_q2_9.5.root");
    fdummy_shms[9.5] = new TFile("dummy_shms_q2_9.5.root");

    // Q^2 = 11.5
    fcoin[11.5]       = new TFile("coin_replay_production_LH2_11.5_largecoll.root");
    fsing_shms[11.5]  = new TFile("lh2_shms_singles_q2_11.5.root");
    fdummy_shms[11.5] = new TFile("dummy_shms_q2_11.5.root");

    // Q^2 = 14.3
    fcoin[14.3]       = new TFile("coin_replay_production_LH2_14.3_largecoll.root");
    fsing_shms[14.3]  = new TFile("lh2_shms_singles_q2_14.3.root");
    fdummy_shms[14.3] = new TFile("dummy_shms_q2_14.3.root");

    for (Double_t q2: {8.0, 9.5, 11.5, 14.3}) {
        fcoin[q2]->GetObject("T",tcoin[q2]);
        fsing_shms[q2]->GetObject("T",tsing_shms[q2]);
        fdummy_shms[q2]->GetObject("T",tdummy_shms[q2]);
    }

    std::map<Double_t, TFile*> canvas_file;
    std::map<Double_t, TCanvas*> canvas;
    std::map<Double_t, std::map<TString, TH1F*>> histo;
    std::vector<TString> histoNames = {"h_emiss", "h_delta", "h_hslope", "h_pslope",
                                       "h_react_cut", "h_react_open", "h_coinW_singlescut",
                                       "h_ngc_open", "h_ngc_singlescut", "h_ngc_coincut",
                                       "h_coinW_coincut", "h_coinW_inpeak", "h_coinW_open",
                                       "h_singW_cut", "h_singW_inpeak", "h_singW_open",
                                       "h_singW_cut_wide", "h_singW_inpeak_wide",
                                       "h_singW_open_wide", "h_sing_W_count", "h_coin_W_count"};

    TString canvasName;

    // ------------------------------------------------------------------------
    // Get stuff from canvas root files
    for (auto q2: {8.0, 9.5, 11.5, 14.3}) {
        canvas_file[q2] = TFile::Open(Form("q2_%.1f_canvas.root", q2));

        // Get canvas
        canvasName = Form("c_q2_%.1f", q2);
        canvas_file[q2]->GetObject(canvasName.Data(), canvas[q2]);

        // Get histograms
        for (auto histoName: histoNames) {
            canvas_file[q2]->GetObject(histoName.Data(), histo[q2][histoName]);
        }
    }
}
