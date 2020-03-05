{
    std::vector<Double_t> q2s = {8.0, 9.5, 11.5, 14.3};
    std::map<Double_t, TFile*> file;
    std::map<Double_t, TCanvas*> canvas;
    std::map<Double_t, std::map<TString, TH1*>> histo;

    std::map<Double_t, Double_t> singN, coinN;
    std::map<Double_t, Double_t> singQ, coinQ;
    std::map<Double_t, Double_t> singY, coinY;
    std::map<Double_t, Double_t> absorption;
    std::map<Double_t, Double_t> uncertainty;

    std::vector<TString> histoNames = {"h_emiss", "h_delta", "h_hslope", "h_pslope",
                                       "h_react_cut", "h_react_open", "h_coinW_singlescut",
                                       "h_coinW_coincut", "h_coinW_inpeak", "h_coinW_open",
                                       "h_singW_cut", "h_singW_inpeak", "h_singW_open",
                                       "h_singW_cut_wide", "h_singW_inpeak_wide",
                                       "h_singW_open_wide", "h_sing_W_count", "h_coin_W_count"};

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

    // Singles charge
    singQ[8.0]  = 69051.0/1e3;
    singQ[9.5]  = 89364.0/1e3;
    singQ[11.5] = 88359.6/1e3;
    singQ[14.3] = 245645.0/1e3;

    // Coin charge
    coinQ[8.0]  = 2465.7;
    coinQ[9.5]  = 381.221;
    coinQ[11.5] = 386.161;
    coinQ[14.3] = 2332.18;

    // ------------------------------------------------------------------------
    // Get counts, yield, absorption, and uncertainty
    // For uncertainty, I use Poisson statistics for N and ignore uncertainty in Q.
    TString singHistogramName = "h_sing_W_count";
    TString coinHistogramName = "h_coin_W_count";
    for (auto q2: q2s) {
        singN[q2] = histo[q2][singHistogramName]->Integral();
        coinN[q2] = histo[q2][coinHistogramName]->Integral();

        singY[q2] = singN[q2]/singQ[q2];
        coinY[q2] = coinN[q2]/coinQ[q2];

        absorption[q2] = 100*(1-(coinY[q2]/singY[q2]));
        uncertainty[q2] = absorption[q2]*sqrt( (1/coinN[q2]) + (1/Double_t(singN[q2])) );
    }

    // ------------------------------------------------------------------------
    // Print
    Int_t q2Width          = 12;
    Int_t absorptionWidth  = 15;
    Int_t uncertaintyWidth = 15;
    Int_t countWidth       = 8;
    Int_t chargeWidth      = 17;
    Int_t yieldWidth       = 12;

    // header
    std::cout << std::left
              << " | " << std::setw(q2Width)          << "Q^2 [Gev^2]"
              << " | " << std::setw(absorptionWidth)  << "absorption [%]"
              << " | " << std::setw(uncertaintyWidth) << "uncertainty [%]"
              << " | " << std::setw(countWidth)       << "Coin N"
              << " | " << std::setw(chargeWidth)      << "Coin charge [mC]"
              << " | " << std::setw(yieldWidth)       << "Coin yield"
              << " | " << std::setw(countWidth)       << "Sing N"
              << " | " << std::setw(chargeWidth)      << "Sing charge [mC]"
              << " | " << std::setw(yieldWidth)       << "Sing yield"
              << " | " << std::endl;

    for (auto q2: q2s) {
        std::cout << std::left
              << " | " << std::setw(q2Width)          << q2
              << " | " << std::setw(absorptionWidth)  << absorption[q2]
              << " | " << std::setw(uncertaintyWidth) << uncertainty[q2]
              << " | " << std::setw(countWidth)       << coinN[q2]
              << " | " << std::setw(chargeWidth)      << coinQ[q2]
              << " | " << std::setw(yieldWidth)       << coinY[q2]
              << " | " << std::setw(countWidth)       << singN[q2]
              << " | " << std::setw(chargeWidth)      << singQ[q2]
              << " | " << std::setw(yieldWidth)       << singY[q2]
              << " | " << std::endl;
    }
}
