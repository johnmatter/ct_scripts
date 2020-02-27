{
    std::vector<Double_t> q2s = {8.0, 9.5, 11.5, 14.3};
    std::map<Double_t, TFile*> file;
    std::map<Double_t, TCanvas*> canvas;
    std::map<Double_t, std::map<TString, TH1*>> histo;

    std::map<Double_t, Double_t> singN, coinN;
    std::map<Double_t, Double_t> singQ, coinQ;
    std::map<Double_t, Double_t> singY, coinY;
    std::map<Double_t, Double_t> absorption;

    std::vector<TString> histoNames = {"h_emiss", "h_delta", "h_hslope", "h_pslope",
                                       "h_react_cut", "h_react_open", "h_coinW_singlescut",
                                       "h_coinW_coincut", "h_coinW_inpeak", "h_coinW_open",
                                       "h_singW_cut", "h_singW_inpeak", "h_singW_open",
                                       "h_singW_cut_wide", "h_singW_inpeak_wide",
                                       "h_singW_open_wide"};

    TString canvasName;

    // ------------------------------------------------------------------------
    // Get stuff from canvas root files
    for (auto q2: q2s) {
        file[q2] = TFile::Open(Form("q2_%.1f_canvas.root", q2));

        // Get canvas
        canvasName = Form("c_q2_%.1f", q2);
        file[q2]->GetObject(canvasName.Data(), canvas[q2]);

        // Get histograms
        for (auto histoName: histoNames) {
        file[q2]->GetObject(histoName.Data(), histo[q2][histoName]);
        }
    }

    // ------------------------------------------------------------------------
    // Load trees for calculating total charge
    std::map<Double_t, TFile*> fcoin, fsing, fdummy;
    std::map<Double_t, TTree*> tcoin, tsing;
    TTree* t;

    // Q^2 = 8
    fcoin[8.0]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_8_smallcoll.root");
    fsing[8.0]  = new TFile("/Volumes/ssd750/ct/pass3/hms_coin_replay_production_2049_500000.root");

    // Q^2 = 9.5
    fcoin[9.5]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_9.5_smallcoll.root");
    fsing[9.5]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_hms_singles_q2_9.5.root");

    // Q^2 = 11.5
    fcoin[11.5]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_11.5_largecoll.root");
    fsing[11.5]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_hms_singles_q2_11.5.root");

    // Q^2 = 14.3
    fcoin[14.3]  = new TFile("/Volumes/ssd750/ct/pass3/coin_replay_production_LH2_14.3_largecoll.root");
    fsing[14.3]  = new TFile("/Volumes/ssd750/ct/pass3/lh2_hms_singles_q2_14.3.root");

    // Read
    for (auto q2: q2s) {
        fcoin[q2]->GetObject("TSH", tcoin[q2]);
        fsing[q2]->GetObject("TSH", tsing[q2]);
    }

    // ------------------------------------------------------------------------
    // Get charge
    TString bcmBranchName = "H.BCM4A.scalerChargeCut";
    Double_t Q, Qprev, Qtot;
    for (auto q2: q2s) {
        // Singles
        t = tsing[q2];
        t->SetBranchAddress(bcmBranchName.Data(), &Q);
        Q = Qprev = Qtot = 0;
        for (int i=1; i<t->GetEntries(); t->GetEntry(i++)) {
            if ( (Q<Qprev) || (i==(t->GetEntries()-1)) ) {
               Qtot += Qprev;
            }
            Qprev = Q;
        }
        singQ[q2] = Qtot/1e3;

        // Coin
        t = tcoin[q2];
        t->SetBranchAddress(bcmBranchName.Data(), &Q);
        Q = Qprev = Qtot = 0;
        for (int i=1; i<t->GetEntries(); t->GetEntry(i++)) {
            if ( (Q<Qprev) || (i==(t->GetEntries()-1)) ) {
               Qtot += Qprev;
            }
            Qprev = Q;
        }
        coinQ[q2] = Qtot/1e3;
    }

    // ------------------------------------------------------------------------
    // Get counts and yield
    TString singHistogramName = "h_singW_inpeak";
    TString coinHistogramName = "h_coinW_inpeak";
    for (auto q2: q2s) {
        singN[q2] = histo[q2][singHistogramName]->Integral();
        coinN[q2] = histo[q2][coinHistogramName]->Integral();

        singY[q2] = singN[q2]/singQ[q2];
        coinY[q2] = coinN[q2]/coinQ[q2];
    }

    // ------------------------------------------------------------------------
    // Calculate and print absorption
    for (auto q2: q2s) {
        absorption[q2] = 100*(1-(coinY[q2]/singY[q2]));
    }

    Int_t q2width     = 12;
    Int_t absorpwidth = 18;
    Int_t countwidth  = 8;
    Int_t chargewidth = 20;
    Int_t yieldwidth  = 16;

    // header
    std::cout << std::left
              << std::setw(q2width)     << "Q^2 [Gev^2]"
              << std::setw(absorpwidth) << "absorption [%]"
              << std::setw(countwidth)  << "Coin N"
              << std::setw(chargewidth) << "Coin charge [mC]"
              << std::setw(yieldwidth)  << "Coin yield"
              << std::setw(countwidth)  << "Sing N"
              << std::setw(chargewidth) << "Sing charge [mC]"
              << std::setw(yieldwidth)  << "Sing yield"
              << std::endl;

    for (auto q2: q2s) {
        std::cout << std::left
                  << std::setw(q2width)     << q2
                  << std::setw(absorpwidth) << absorption[q2]
                  << std::setw(countwidth)  << coinN[q2]
                  << std::setw(chargewidth) << coinQ[q2]
                  << std::setw(yieldwidth)  << coinY[q2]
                  << std::setw(countwidth)  << singN[q2]
                  << std::setw(chargewidth) << singQ[q2]
                  << std::setw(yieldwidth)  << singY[q2]
                  << std::endl;
    }
}
