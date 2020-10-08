{
    std::map<Double_t, TFile*> fcoin, fsing_hms, fdummy_hms;
    std::map<Double_t, TTree*> tcoin, tsing_hms, tdummy_hms;

    // // Q^2 = 8
    // fcoin[8.0]      = new TFile("coin_replay_production_LH2_8_smallcoll.root");
    // fsing_hms[8.0]  = new TFile("lh2_hms_singles_q2_8.root");
    // fdummy_hms[8.0] = new TFile("dummy_hms_q2_8.root");

    // // Q^2 = 9.5
    // fcoin[9.5]      = new TFile("coin_replay_production_LH2_9.5_smallcoll.root");
    // fsing_hms[9.5]  = new TFile("lh2_hms_singles_q2_9.5.root");
    // fdummy_hms[9.5] = new TFile("dummy_hms_q2_9.5.root");

    // Q^2 = 11.5
    fcoin[11.5]      = new TFile("coin_replay_production_LH2_11.5_largecoll.root");
    fsing_hms[11.5]  = new TFile("lh2_hms_singles_q2_11.5.root");
    fdummy_hms[11.5] = new TFile("dummy_hms_q2_11.5.root");

    // // Q^2 = 14.3
    // fcoin[14.3]      = new TFile("coin_replay_production_LH2_14.3_largecoll.root");
    // fsing_hms[14.3]  = new TFile("lh2_hms_singles_q2_14.3.root");
    // fdummy_hms[14.3] = new TFile("dummy_hms_q2_14.3.root");

    for (Double_t q2: {11.5}) {
        fcoin[q2]->GetObject("T",tcoin[q2]);
        fsing_hms[q2]->GetObject("T",tsing_hms[q2]);
        fdummy_hms[q2]->GetObject("T",tdummy_hms[q2]);
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
    for (auto q2: {11.5}) {
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
