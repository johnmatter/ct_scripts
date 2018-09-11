// This plots tracking efficiency for the SHMS and HMS
// as a function of Q^2 for C12 and LH2 targets
void tracking() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *hms_data  = new CTData("/home/jmatter/ct_scripts/ct_hms_singles_data.json");
    CTData *shms_data = new CTData("/home/jmatter/ct_scripts/ct_shms_singles_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::vector<TString>  spectrometers = {"HMS","SHMS"};
    std::vector<TString>  targets       = {"LH2","C12"};
    std::vector<Int_t>    Q2s           = {8, 10, 12, 14}; // rounded for filenames
    std::vector<Double_t> Q2vals        = {8, 9.5, 11.5, 14.3}; // actual values

    // This map takes <target,Q^2> as a key
    std::map<std::pair<TString, Int_t>, TGraphAsymmErrors*> efficiencyGraphs;
    std::map<std::pair<TString, Int_t>, Efficiency0D*> efficiencyCalculators;

    // Set up our cuts
    TCut hCutShould = cuts->Get("hScinShould");
    TCut pCutShould = cuts->Get("pScinShould");
    TCut hCutDid    = cuts->Get("hScinDid");
    TCut pCutDid    = cuts->Get("pScinDid");

    // This will contain all our graphs
    TMultiGraph *multiGraph = new TMultiGraph("multiGraph", "Tracking Efficiency");

    // ------------------------------------------------------------------------
    // Calculate and plot for both spectrometers
    for (auto const &s : spectrometers) {
        // Get this spectrometer's data
        CTData* data;
        if (s=="SHMS") { data = shms_data; }
        if (s=="HMS")  { data = hms_data;  }

        // Loop over <target,Q2> pairs
        for (auto const &t : targets)   {
            // These will be used to construct our TGraphAsymmErrors
            std::vector<Double_t> e;      // efficiency
            std::vector<Double_t> eUp;    // upper error bar
            std::vector<Double_t> eLow;   // lower error bar
            std::vector<Double_t> eZeros; // zeros to get rid of X error bars

            for (auto const &q : Q2s)   {
                // Create efficiency object
                TString efficiencyName = Form("teff_%s_%s_%d", s.Data(), t.Data(), q, p);
                efficiencyCalculators[key] = new Efficiency0D(efficiencyName.Data());

                // Set chain
                TString dataKey = Form("%s_Q2_%d", t.Data(), q);
                TChain* chain = data->GetChain(dataKey);
                efficiencyCalculators[key]->SetChain(chain);

                // Set cuts
                efficiencyCalculators[key]->SetShouldCut(cutShould);
                TCut cutDid = Form(cutDidString, p);
                efficiencyCalculators[key]->SetDidCut(cutDid);

                TString status = Form("-------\nSTATUS: %s, %s, Q^2=%d",
                        s.Data(), t.Data(), q);
                std::cout << status << std::endl;

                // Calculate
                efficiencyCalculators[key]->SetEvents(-1);
                efficiencyCalculators[key]->Init();
                efficiencyCalculators[key]->Calculate();

                // Get efficiencies and error, then add to our vectors
                e.push_back(efficiencyCalculators[key]->GetEfficiency());
                eUp.push_back(efficiencyCalculators[key]->GetEfficiencyErrorUp());
                eLow.push_back(efficiencyCalculators[key]->GetEfficiencyErrorLow());
                eZeros.push_back(0);
            }

            auto key = std::make_pair(t,s);

            // &v[0] "converts" a vector<double> named v into a double*
            efficiencyGraphs[key] = new TGraphAsymmErrors(4, &Q2vals[0], &e[0],
                                              &eZeros[0], &eZeros[0],
                                              &eLow[0], &eUp[0]);

            TString title = Form("%s %s", s.Data(), t.Data());
            efficiencyGraphs[key]->SetTitle(title);
        }
    }

    // ------------------------------------------------------------------------
    // Format and draw
    TLegend *legend = new TLegend(0.15, 0.3, 0.35, 0.2);

    TString target, spect;

    target="LH2"; spect="SHMS";
    key = std::make_pair(target, spect);
    efficiencyGraphs[key]->SetMarkerStyle(26);
    efficiencyGraphs[key]->SetMarkerColor(38);
    efficiencyGraphs[key]->SetLineStyle(9);
    efficiencyGraphs[key]->SetLineColor(38);
    legend->AddEntry(efficiencyGraphs[key],
            Form("%s %s",target.Data(), spect.Data()), "lp");

    target="LH2"; spect="HMS";
    key = std::make_pair(target, spect);
    efficiencyGraphs[key]->SetMarkerStyle(26);
    efficiencyGraphs[key]->SetMarkerColor(46);
    efficiencyGraphs[key]->SetLineStyle(9);
    efficiencyGraphs[key]->SetLineColor(46);
    legend->AddEntry(efficiencyGraphs[key],Form("%s %s",
                target.Data(), spect.Data()), "lp");

    target="C12"; spect="SHMS";
    key = std::make_pair(target, spect);
    efficiencyGraphs[key]->SetMarkerStyle(20);
    efficiencyGraphs[key]->SetMarkerColor(38);
    efficiencyGraphs[key]->SetLineStyle(1);
    efficiencyGraphs[key]->SetLineColor(38);
    legend->AddEntry(efficiencyGraphs[key],
            Form("%s %s",target.Data(), spect.Data()), "lp");

    target="C12"; spect="HMS";
    key = std::make_pair(target, spect);
    efficiencyGraphs[key]->SetMarkerStyle(20);
    efficiencyGraphs[key]->SetMarkerColor(46);
    efficiencyGraphs[key]->SetLineStyle(1);
    efficiencyGraphs[key]->SetLineColor(46);
    legend->AddEntry(efficiencyGraphs[key],
            Form("%s %s",target.Data(), spect.Data()), "lp");

    multiGraph->Draw("LZAP");
    multiGraph->GetXaxis()->SetTitle("Q^{2} (GeV^{2})");
    multiGraph->GetYaxis()->SetTitle("Efficiency");
    multiGraph->SetMinimum(0);
    multiGraph->SetMaximum(1.02);
    cEff->Modified();
    legend->Draw();

}
